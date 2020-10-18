#include <cstdlib>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include "threadpool.h"

using std::cout;
using std::endl;

struct data {
    int a, b;
};

void add(void *param)
{
    data *temp = reinterpret_cast<data *>(param);

    cout << "I add two values " << temp->a << " and " << temp->b
         << " result = " << temp->a + temp->b << endl;
}

const int n = 100;
data work[n];

int main()
{
	ThreadPool pool;
	
	//submit 100 tasks to be done
	
	for(int i = 0; i < n; ++i)
	{
		//int val1 = i,
			//val2 = i+1;
		//cout << "val1: " << val1 << endl;
		//cout << "val2: " << val2 << endl;
		// create some work to do
		//data work = {val1, val2};
		//cout << "[" << i << "] ";
		//pool.submit(add, &work);
		
		// may be helpful
    	//sleep(3);
    	
    	work[i].a = i;
    	work[i].b = i+1;
    	
    	pool.submit(add,&work[i]);
	}
	
	//data work = {5, 10};
	/*for(int i = 0; i < 20; ++i)
	{
    	pool.submit(add, &work);
    }*/
    /*
    data work1 = {0, 1};
    pool.submit(add, &work1);
    data work2 = {1, 2};
    pool.submit(add, &work2);
    data work3 = {2, 3};
    pool.submit(add, &work3);
    data work4 = {3, 4};
    pool.submit(add, &work4);
    data work5 = {4, 5};
    pool.submit(add, &work5);
    
    data work6 = {5, 6};
    pool.submit(add, &work6);
    data work7 = {6, 7};
    pool.submit(add, &work7);
    data work8 = {7, 8};
    pool.submit(add, &work8);
    data work9 = {8, 9};
    pool.submit(add, &work9);
    data work10 = {9, 10};
    pool.submit(add, &work10);
    */
    //sleep(1);
    pool.shutdown();

    return EXIT_SUCCESS;
}
