#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define RANGE 2333333 //attempt limit

double sum; //number of spots in the circle

//get the random number within a certain range
double my_rand(double min, double max) {
	return min + (max - min) * rand() * 1.0 / RAND_MAX;
};

//the thread runs in this separate function
DWORD WINAPI get_sum(LPVOID lpParam) {
	int cnt = 0;

	srand(time(NULL)); //remake seed of random number based on current time 
	for (int i = 0; i < RANGE; i++) {
		double x = my_rand(-1, 1);
		double y = my_rand(-1, 1);
		if (x * x + y * y <= 1)
			cnt++;
	}  //get the number of spots in the circle
	sum = cnt;
	return 0;
}

int main(int argc, char* argv[]) {
	DWORD ThreadId;
	HANDLE ThreadHandle;

	//create the thread
	ThreadHandle = CreateThread(NULL, 0, get_sum, NULL, 0, &ThreadId);
	if (ThreadHandle != NULL) {
		WaitForSingleObject(ThreadHandle, INFINITE);
		CloseHandle(ThreadHandle);
		printf_s("Pi = %lf", sum * 4.0 / RANGE); // calculate and output the value of Pi
	}
}
