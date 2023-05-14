#include <cstdio>
#include <chrono>
#include <iostream>
#include <cassert>
#include <pthread.h>
#include <vector>
#include <cmath>
#include <stack>
#include <unistd.h>

struct CLIArgs {
    size_t thread_number {0};
    double accuracy {0};
};

struct IntegrateTaskInfo {
    double start;
    double end;

    double f_start;
    double f_end;

    double area;
};

class WorkerThread {
public:
    static double function(double x) {
        return std::sin(1.0 / (x + 5.0));
    }

    WorkerThread() = default;
    ~WorkerThread() = default;

    explicit WorkerThread(size_t task_id, size_t thread_number, double accuracy) {
        task_id_        = task_id;
        thread_number_  = thread_number;
        accuracy_       = accuracy;
    }

    static void* Integrate(void *worker);

    size_t task_id_ {0};
    size_t thread_number_ {0};

    double accuracy_ {0};
    double part_result_value_ {0};

    std::stack<IntegrateTaskInfo> local_stack_;
};

class MainThread {
public:
    double x_start  = -4.999;
    double x_end    = 0;
    double integration_segment = x_end - x_start;

    explicit MainThread(CLIArgs *cli_args) {
        assert(cli_args != nullptr);

        thread_number_ = cli_args->thread_number;
        accuracy_ = cli_args->accuracy;

        threads_.resize(thread_number_);
        workers_.resize(thread_number_);
    }

    ~MainThread() {}

    bool CreateThreads();
    void JoinThreads();

    double GetResult() {
        return result_value_;
    }

    static int ParseCLIArgs(int argc, char **argv, CLIArgs* cli_args);

private:
    size_t thread_number_ {0};
    double accuracy_ {0};
    double result_value_ {0};

    std::vector<pthread_t> threads_;
    std::vector<WorkerThread> workers_;
};

class ScopedTimeMeasure {
public:
    using chr_time_p = std::chrono::time_point<std::chrono::high_resolution_clock>;

    explicit ScopedTimeMeasure() {
        start_ = std::chrono::high_resolution_clock::now();
    }
    ~ScopedTimeMeasure() {
        end_ = std::chrono::high_resolution_clock::now();
        auto us = std::chrono::duration_cast<std::chrono::microseconds>(end_ - start_).count();
        std::cout << us << " us" << std::endl;
    }

private:
    chr_time_p start_;
    chr_time_p end_;
};

static std::stack<IntegrateTaskInfo> stack;
static size_t active_threads = 0;

static pthread_mutex_t stack_present_lock;
static pthread_mutex_t stack_access_lock;

static constexpr int local_to_global_stack_count = 15;
static constexpr int max_stack_size = 50;

int main(int argc, char **argv) {
    CLIArgs cli_args;
    int parse_args = MainThread::ParseCLIArgs(argc, argv, &cli_args);

    if (parse_args > 0) {
        return parse_args;
    }
    
    MainThread main_thread(&cli_args);
    {
        ScopedTimeMeasure stm;
        main_thread.CreateThreads();
        main_thread.JoinThreads();
    }

    std::cout.precision(std::numeric_limits<double>::max_digits10);
    std::cout << "I = " << main_thread.GetResult() << std::endl;

    return 0;
}

int MainThread::ParseCLIArgs(int argc, char **argv, CLIArgs* cli_args) {
    
    if (argc != 3) {
        std::cout << "input [threads] [accuracy]" << std::endl;
        return 1;
    }

    cli_args->thread_number = std::atoi(argv[1]);
    if (cli_args->thread_number < 1) {
        std::cout << "Thread number should be more than 1" << std::endl;
        return 2;
    }

    cli_args->accuracy = std::abs(std::atof(argv[2]));
    if (cli_args->accuracy > 1) {
        std::cout << "Accuracy should be <= 1" << std::endl;
        return 3;
    }

    return 0;
}

bool MainThread::CreateThreads() {
    double f_a = WorkerThread::function(x_start);
    double f_b = WorkerThread::function(x_end);

    stack.push(IntegrateTaskInfo{x_start, x_end, f_a, f_b, (f_a + f_b) * (x_end - x_start) / 2});

    for (size_t i = 0; i < thread_number_; ++i) {
        pthread_t thread;
        WorkerThread worker(i, thread_number_, accuracy_);
        workers_[i] = worker;

        if (pthread_create(&thread, nullptr, WorkerThread::Integrate, &workers_[i])) {
            std::cerr << "[ERROR:] Can't create thread" << std::endl;
            return false;
        }
        threads_[i] = thread;
    }
    return true;
}

void MainThread::JoinThreads()
{
    void *status = nullptr;
    for (size_t i = 0; i < thread_number_; ++i) {
        pthread_join(threads_[i], &status);
        WorkerThread *worker = reinterpret_cast<WorkerThread *>(status);
        result_value_ += worker->part_result_value_;
    }
}

void *WorkerThread::Integrate(void *wrkr)
{
    WorkerThread *worker = (WorkerThread *)wrkr;

    // Architecture specific mapping on cpu's
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(worker->task_id_, &cpuset);
    pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);

    double a = 0;
    double b = 0;
    double f_a = 0;
    double f_b = 0;
    double int_ab = 0;

    while (true) {
        pthread_mutex_lock(&stack_present_lock);
        pthread_mutex_lock(&stack_access_lock);

        IntegrateTaskInfo new_task = stack.top();

        a      = new_task.start;
        b      = new_task.end;
        f_a    = new_task.f_start;
        f_b    = new_task.f_end;
        int_ab = new_task.area;

        stack.pop();

        if (!stack.empty()) {
            pthread_mutex_unlock(&stack_present_lock);
        }

        if (a <= b) {
            ++active_threads;
        }

        pthread_mutex_unlock(&stack_access_lock);

        if (a > b) {
            break;
        }

        while (true) {
            const double c = (a + b) / 2;
            const double f_c = WorkerThread::function(c);

            const double int_ac = (f_a + f_c) * (c - a) / 2;
            const double int_cb = (f_c + f_b) * (b - c) / 2;
            const double int_acb = int_ac + int_cb;

            if (std::abs(int_ab - int_acb) >= worker->accuracy_ * std::abs(int_acb)) {
                worker->local_stack_.push(IntegrateTaskInfo{a, c, f_a, f_c, int_ac});
                a      = c;
                f_a    = f_c;
                int_ab = int_cb;
            } else {
                worker->part_result_value_ += int_acb;
                if (worker->local_stack_.empty()) {
                    break;
                }

                IntegrateTaskInfo new_task = worker->local_stack_.top();

                a      = new_task.start;
                b      = new_task.end;
                f_a    = new_task.f_start;
                f_b    = new_task.f_end;
                int_ab = new_task.area;

                worker->local_stack_.pop();
            }

            if (worker->local_stack_.size() > local_to_global_stack_count) {
                pthread_mutex_lock(&stack_access_lock);
                if (stack.empty()) {
                    while (worker->local_stack_.size() > 1 && stack.size() < max_stack_size) {
                        stack.push(worker->local_stack_.top());
                        worker->local_stack_.pop();
                    }
                    pthread_mutex_unlock(&stack_present_lock);
                }
                pthread_mutex_unlock(&stack_access_lock);
            }
        }
        
        pthread_mutex_lock(&stack_access_lock);
        active_threads--;

        if (active_threads == 0 && stack.empty()) {
            for(size_t i = 0; i < worker->thread_number_; ++i) {
                stack.push(IntegrateTaskInfo{3.0, 1.0, 0.0, 0.0, 0.0});
            }
            pthread_mutex_unlock(&stack_present_lock);
        }

        pthread_mutex_unlock(&stack_access_lock);
    }
    pthread_exit(worker);
}