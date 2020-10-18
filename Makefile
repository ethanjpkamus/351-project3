CXXFLAGS = -g -std=c++17 -Wall -Wextra -Wpedantic -pthread

.PHONY: check check-leaks check-threads clean

example: client.o threadpool.o
	$(CXX) $(CXXFLAGS) -o $@ $^

threadpool.o: threadpool.cpp threadpool.h
	$(CXX) $(CXXFLAGS) -c $<

check: check-leaks check-threads

check-leaks: example
	valgrind --quiet ./$<

check-threads: example
	valgrind --tool=helgrind --quiet ./$<

clean:
	rm -f *.o example
