NAMES
	
	Ethan Kamus (ethanjpkamus@csu.fullerton.edu)

SUMMARY OF PROJECT 3

	The purpose of this project is to utilize the POSIX API and create a thread
	pool to take in multiple tasks and execute them. If there are more tasks su-
	bmitted than there are threads, add them to a task queue to be taken by thr-
	eads once they are able to. It was also important to use mutexes and semaph-
	ores throughout this project to prevent impropper output and protect against
	deadlocks.
	
FILES INCLUDED

	README.txt
	Makefile
	client.cpp
	threadpool.cpp
	threadpool.h

HOW TO COMPILE AND RUN

	To Compile, enter:
		
		make
		
	To Run, enter:
		
		./project3
