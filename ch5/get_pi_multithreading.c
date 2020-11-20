#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define RANGE 2333333 //attempt limit

HANDLE  MUTEXLOCK = NULL;
double sum; //number of spots in the circle

//get the random number within a certain range
double my_rand(double min, double max) {
	return min + (max - min) * rand() * 1.0 / RAND_MAX;
};

//the thread runs in this separate function
DWORD WINAPI get_sum(LPVOID lpParam) {
	int cnt = 0;

	WaitForSingleObject(MUTEXLOCK, INFINITE);//wait for teh mutexlock
	srand(time(NULL)); //remake seed of random number based on current time 
	for (int i = 0; i < RANGE; i++) {
		double x = my_rand(-1, 1);
		double y = my_rand(-1, 1);
		if (x * x + y * y <= 1)
			cnt++;
	}  //get the number of spots in the circle
	sum += cnt;
	ReleaseMutex(MUTEXLOCK);//release the mutexlock
	return 0;
}

int main(int argc, char* argv[]) {

	int i = 0;
	int param = 0;

	//basic error checking
	if (argc != 2) {
		fprintf(stderr, "An integer parameter is required\n");
		return -1;
	}
	param = atoi(argv[1]);
	if (param < 0) {
		fprintf(stderr, "an integer >= 0 is required \n");
		return -1;
	}

	HANDLE* ThreadHandle;
	ThreadHandle = (HANDLE*)malloc(param * sizeof(HANDLE));
	MUTEXLOCK = CreateMutex(NULL, FALSE, NULL);//create the mutexlock

	//create the threads
	for (i = 0; i < param; i++) {
		ThreadHandle[i] = CreateThread(NULL, 0, get_sum, NULL, 0, NULL);
		if (ThreadHandle[i] != NULL) {
			WaitForSingleObject(ThreadHandle[i], INFINITE);
			printf_s("Create thread%d successfully!\n", i);
			CloseHandle(ThreadHandle[i]);
		}
		Sleep(1000);// because precision of time() is 1 second
	}
	CloseHandle(MUTEXLOCK);//close the mutexlock
	printf_s("Pi = %lf", sum * 4.0 / RANGE / param); // calculate and output the value of Pi
}
