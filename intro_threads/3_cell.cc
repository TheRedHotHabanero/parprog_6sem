#include <iostream>
#include <vector>
#include <array>
#include <pthread.h>
#include <sstream>

int THREADS;
int N;
int message = 0;
pthread_mutex_t pthread_mut = PTHREAD_MUTEX_INITIALIZER;

void *send_circle(void *self_num)
{
    std::ostringstream output;
    output << "Thread " << *(int *)(self_num) << ", message ";

    pthread_mutex_lock(&pthread_mut);
    int cur_msg = message++;
    pthread_mutex_unlock(&pthread_mut);

    output << cur_msg << std::endl;
    std::cout << output.str();

    return nullptr;
}

class MyThread {
public:
    int id{};
    pthread_t thread{};
};

int main(int argc, char *argv[]) {

    // Check if input is correct
    if (argc != 2 || std::atoi(argv[1]) <= 0) {
        std::cout << "Enter number of threads. Number of threads should be positive" << std::endl;
        return 1;
    }

    // Input is correct, so get number of threads
    THREADS = std::atoi(argv[1]);

    // Init vector for future threads
    std::vector<MyThread> my_lovely_threads;
    my_lovely_threads.resize(THREADS);
    
    pthread_mutex_unlock(&pthread_mut);

    // Creating threads
    for (int i = 0; i < THREADS; ++i) {
        my_lovely_threads[i].id = i;
        pthread_create(&my_lovely_threads[i].thread, nullptr, &send_circle, (void *)(&my_lovely_threads[i].id));
    }

    // Waits for a thread to terminate, detaches the thread, then returns the threads exit status
    for (auto thread : my_lovely_threads) {
        pthread_join(thread.thread, nullptr);
    }

    return 0;
}