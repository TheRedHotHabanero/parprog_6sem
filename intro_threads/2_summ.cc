#include <iostream>
#include <vector>
#include <array>
#include <pthread.h>
#include <sstream>

int THREADS;
int N;

void *summator(void *self_num)
{
    long double step = 0.0;

    int id = *(int *)(self_num) + 1;
    unsigned long int work_amount = N / THREADS;
    unsigned long int start = work_amount * (id - 1) + 1;
    
    if (id == THREADS) {
        work_amount += N % THREADS;
    }

    for (unsigned int i = start, end = start + work_amount; i < end; ++i) {
        step += 1.0 / i;
    }

    return new long double(step);
}

class MyThread {
public:
    int id{};
    pthread_t thread{};
};

int main(int argc, char *argv[]) {

    // Check if input is correct
    if (argc != 3 || std::atoi(argv[1]) <= 0) {
        std::cout << "Enter number of threads, then N. Number of threads, N should be positive" << std::endl;
        return 1;
    }
    // Input is correct, so get number of threads
    THREADS = std::atoi(argv[1]);

    N = std::atoi(argv[2]);
    if (N <= 0) {
        std::cout << "N must be positive" << std::endl;
        return 2;
    }

    // Init vector for future threads
    std::vector<MyThread> my_lovely_threads;
    my_lovely_threads.resize(THREADS);
    
    // Creating threads
    for (int i = 0; i < THREADS; ++i) {
        my_lovely_threads[i].id = i;
        pthread_create(&my_lovely_threads[i].thread, nullptr, &summator, (void *)(&my_lovely_threads[i].id));
    }

    long double summ = 0;
    // Waits for a thread to terminate, detaches the thread, then returns the threads exit status
    for (auto thread : my_lovely_threads) {
        long double *step = nullptr;
        pthread_join(thread.thread, (void **)(&step));
        summ += *step;
        delete step;
    }

    std::cout << summ << std::endl;

    return 0;
}