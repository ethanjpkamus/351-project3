#include <cstdlib>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include "threadpool.h"

using std::cout;
using std::endl;

pthread_mutex_t mutex_cout;

struct data {
    int a, b;
};

void add(void *param)
{
    data *temp = reinterpret_cast<data *>(param);

    pthread_mutex_lock(&mutex_cout);
    cout << "I add two values " << temp->a << " and " << temp->b
         << " result = " << temp->a + temp->b << endl;
    pthread_mutex_unlock(&mutex_cout);
}

const int n = 100;
data work[n];

int main()
{   
    //initialize mutex for cout
    pthread_mutex_init(&mutex_cout,NULL);

	ThreadPool pool;
	
    //submit 100 tasks to be done
	for(int i = 0; i < n; ++i)
	{  
    	work[i].a = i;
    	work[i].b = i+1;
    	
    	pool.submit(add,&work[i]);
	}
	
    pool.shutdown();
    pthread_mutex_destroy(&mutex_cout);

    return EXIT_SUCCESS;
}
