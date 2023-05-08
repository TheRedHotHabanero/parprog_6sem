#include <iostream>
#include <vector>
#include <array>
#include <pthread.h>
#include <sstream>

int THREADS;

// Greeting
void *say_hello(void *self_num) {
    std::ostringstream output;
    output << "Hello world from " << *(int *)(self_num) << " out of " << THREADS << std::endl;
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
        std::cout << "Enter number of threads. Number should be positive" << std::endl;
        return 1;
    }
    // Input is correct, so get number of threads
    THREADS = std::atoi(argv[1]);

    // Init vector for future threads
    std::vector<MyThread> my_lovely_threads;
    my_lovely_threads.resize(THREADS);

    // Creating threads
    for (int i = 0; i < THREADS; ++i) {
        my_lovely_threads[i].id = i;
        pthread_create(&my_lovely_threads[i].thread, nullptr, &say_hello, (void *)(&my_lovely_threads[i].id));
    }

    // Waits for a thread to terminate, detaches the thread, then returns the threads exit status
    for (auto thread_iter : my_lovely_threads) {
        pthread_join(thread_iter.thread, nullptr);
    }

    return 0;
}