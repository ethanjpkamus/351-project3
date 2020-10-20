#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <queue>
//#include <vector>
#include <pthread.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>

//represents a task to be done by a thread
struct Task {
    void (*function)(void *);
    void *data;
};

class ThreadPool {
    public:
        ThreadPool();
        int submit(void (*)(void *), void *);
        void shutdown();

    private:
        static const int NUMBER_OF_THREADS = 10;
        static bool active;
        

        static pthread_mutex_t mutex_queue, mutex_active;
        
        static std::queue<Task> work_queue;
		pthread_t arr[NUMBER_OF_THREADS];
		
        int enqueue(Task t);
        Task dequeue();
        void execute(void (*)(void *), void *);
        static bool check_active();

        static void *worker(void *);
};

#endif
