#include <tuple>
#include <iostream>
#include <unistd.h>

#include "threadpool.h"

using std::cout;
using std::endl;

bool ThreadPool::active;
sem_t sem; //global unnamed semaphore
pthread_mutex_t ThreadPool::mutex_queue;
pthread_mutex_t ThreadPool::mutex_active;
std::queue<Task> ThreadPool::work_queue;

//"writer"
int ThreadPool::enqueue(Task t) {
    
	pthread_mutex_lock(&mutex_queue);
		work_queue.push(t);	
	pthread_mutex_unlock(&mutex_queue);

    return 0;
}

//executed by worker!
Task ThreadPool::dequeue()
{	
	pthread_mutex_lock(&mutex_queue);
		Task t = work_queue.front();
		work_queue.pop();
	pthread_mutex_unlock(&mutex_queue);
	
	return t;
    
}

//run by every single thread after creation?
void *ThreadPool::worker(void *param)
{
    /*
     * pthread_create() expects a C function, not a C++ member function,
     * so we're going to have to do this ourselves...
     *
     */
	
	ThreadPool *self = reinterpret_cast<ThreadPool *>(param);
	Task t;
	
	while(check_active())
	{
		//take task from queue
		sem_wait(&sem);
		if(check_active())
		{
			t = self->dequeue();
			self->execute(t.function, t.data);
		}
		
    }
    pthread_exit(NULL);
    
}

bool ThreadPool::check_active()
{
	pthread_mutex_lock(&mutex_active);
		bool temp = active;
	pthread_mutex_unlock(&mutex_active);
	
	return temp;
}

void ThreadPool::execute(void (*somefunction)(void *p), void *p)
{
	//pthread_mutex_lock(&mutex_queue);
    somefunction(p);
    //pthread_mutex_unlock(&mutex_queue);  
}

//CONSTRUCTOR (equivalent to pool_init() in textbook)
ThreadPool::ThreadPool() /*: ThreadPool(DEFAULT_NUMBER_OF_THREADS)*/
{
	//cout << "default constructor" << endl;
	pthread_mutex_init(&mutex_queue,NULL);
	pthread_mutex_init(&mutex_active,NULL);
	
	pthread_mutex_lock(&mutex_active);
	active = true;
	pthread_mutex_unlock(&mutex_active);
	
	sem_init(&sem,0,0);
	
	for(int i = 0; i < NUMBER_OF_THREADS; ++i)
	{
		pthread_create(&arr[i], NULL, ThreadPool::worker, this);
	}
}

//parameters: function name(?), data struct
int ThreadPool::submit(void (*somefunction)(void *), void *p)
{
	//make a task and pass it the function and data struct
	Task t;
	t.function = somefunction;
	t.data = p;
	int temp = enqueue(t);
	if(temp == 0)
	{
		//indicate new task in queue
		sem_post(&sem);
	}
	
    return 0;
}

void ThreadPool::shutdown()
{
	
	while(true) 
	{	
		pthread_mutex_lock(&mutex_queue);
		bool temp = work_queue.empty();
		pthread_mutex_unlock(&mutex_queue);
		
		//dont proceed until threads are done doing tasks
		if(temp)
		{
			//actual shutdown
			pthread_mutex_lock(&mutex_active);
				active = false;
			pthread_mutex_unlock(&mutex_active);
			
			for(int i = 0; i != NUMBER_OF_THREADS; ++i)
			{
				sem_post(&sem);
			}
			
			for(int i = 0; i != NUMBER_OF_THREADS; ++i)
			{
				pthread_join(arr[i],NULL);
			}

			pthread_mutex_destroy(&mutex_queue);
			pthread_mutex_destroy(&mutex_active);
			sem_destroy(&sem);
			break;
		}
    }
}
