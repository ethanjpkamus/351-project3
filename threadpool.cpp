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

	//cout << "enqueue" << endl;
    
	pthread_mutex_lock(&mutex_queue);
		
		/*if(work_queue.size() == QUEUE_SIZE)
		{
			pthread_mutex_unlock(&mutex_queue);
			return 1;
		}*/
		work_queue.push(t);
		//cout << "queue size: " << work_queue.size() << endl;
		//sem++;
		
	pthread_mutex_unlock(&mutex_queue);

    return 0;
}

//executed by worker!
Task ThreadPool::dequeue()
{
	//cout << "dequeue" << endl;
	
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
     * dequeue at some point in this function?
     */
	//cout << "\n------\nworker\n------" << endl;
	
	ThreadPool *self = reinterpret_cast<ThreadPool *>(param);
	Task t;
	
	//cout << "start while loop" << endl;
	while(check_active())
	{
		//if (!self->work_queue.empty())
		//{
		sem_wait(&sem);
		//}

		//pthread_mutex_lock(&mutex_active);
		if(check_active()){
			//self->worktodo = self->dequeue();
			//self->execute(self->worktodo.function, self->worktodo.data);
			t = self->dequeue();
			self->execute(t.function, t.data);
		}
		//pthread_mutex_unlock(&mutex_active);
		//sem_post(&sem);
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
	//cout << "execute" << endl;
	pthread_mutex_lock(&mutex_queue);
    somefunction(p);
    pthread_mutex_unlock(&mutex_queue);  
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
	
	//arr = new pthread_t[NUMBER_OF_THREADS];
	
	for(int i = 0; i < NUMBER_OF_THREADS; ++i)
	{
		//pthread_t tid;
		pthread_create(&arr[i], NULL, ThreadPool::worker, this);
		//cout << "thread [" << i << "] created" << endl;
		 
		//pool_of_threads.push_back(tid);
		//cout << "thread [" << i << "] put in to vector" << endl;
	}
}
/*
ThreadPool::ThreadPool(int val) : pool_size(val)
{	
	
	//cout << "constructor(int)" << endl;
	
	//initialize mutex
	pthread_mutex_init(&mutex_queue,NULL);
	active = true;
	//initialize semaphore
	//sem = sem_open("SEM", O_CREAT, 0666, 0);
	sem_init(&sem,0,0);
	//int *temp;
	//sem_getvalue(sem,temp);
	//cout << "sem: " << *temp << endl;
	//make fill thread pool
	arr = new pthread_t[val];
	
	for(int i = 0; i < val; ++i)
	{
		//pthread_t tid;
		pthread_create(&arr[i], NULL, ThreadPool::worker, this);
		//cout << "thread [" << i << "] created" << endl;
		 
		//pool_of_threads.push_back(tid);
		//cout << "thread [" << i << "] put in to vector" << endl;
	}
	
	//cout << "thread vector size: " << arr.size() << "\n" << endl;
}
*/
//parameters: function name(?), data struct
int ThreadPool::submit(void (*somefunction)(void *), void *p)
{
	//cout << "submit" << endl;
	
	//make a task and pass it the function and data struct
	Task t;
	t.function = somefunction;
	t.data = p;
	int temp = enqueue(t);
	if(temp == 0)
	{
		//pthread_mutex_lock(&mutex_queue);
		//cout << "post!" << endl;
		//pthread_mutex_unlock(&mutex_queue);
		
		sem_post(&sem);
		
		
	} /*else {
		pthread_mutex_lock(&mutex_queue);
		cout << "no post" << endl;
		pthread_mutex_unlock(&mutex_queue);
	}*/
	
    return 0;
}

void ThreadPool::shutdown()
{
	//cout << "shutdown" << endl;
	while(true)
	{
		pthread_mutex_lock(&mutex_queue);
		bool temp = work_queue.empty();
		pthread_mutex_unlock(&mutex_queue);
		
		if(temp)
		{
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
    //loop through threads and join them? make an array of threads?
}
