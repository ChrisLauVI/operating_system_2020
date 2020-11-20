#include <Windows.h>
#include <process.h>
#include <iostream>
#include <stdio.h>
#include <cstdlib>
#include <string>
#include <time.h>
#include <math.h>

HANDLE  MUTEXLOCK = NULL;

#define NUMBER_OF_CUSTOMERS 5
#define NUMBER_OF_RESOURCES 3

int first_num = 0;
int second_num = 0;
int thrid_num = 0;
int available[NUMBER_OF_RESOURCES];
int maximum[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
int allocation[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
int need[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

int my_rand(int min, int max) {
	max ++;
	if (rand() == RAND_MAX)
		return max - 1;
	else
		return min + (int)((max - min) * rand() * 1.0 / RAND_MAX);
};

DWORD WINAPI customer(LPVOID lpParameter) {
	int num = *(int*)lpParameter;
	while (1) {
		int request[NUMBER_OF_RESOURCES];
		int i;
		WaitForSingleObject(MUTEXLOCK, INFINITE);//wait for teh mutexlock
		for (i = 0; i < NUMBER_OF_RESOURCES; i++) {
			request[i] = my_rand(0, need[num][i] + 1);
			//request[i] = need[num][i];
		}
		if (request[0] == 0 && request[1] == 0 && request[2] == 0) {
			ReleaseMutex(MUTEXLOCK);//release the mutexlock
			continue;
		}
		printf("%d request:%d %d %d", num + 1, request[0], request[1], request[2]);
		if (request[0] <= available[0] && request[1] <= available[1] && request[2] <= available[2]) {
			available[0] -= request[0];
			available[1] -= request[1];
			available[2] -= request[2];
			allocation[num][0] += request[0];
			allocation[num][1] += request[1];
			allocation[num][2] += request[2];
			need[num][0] -= request[0];
			need[num][1] -= request[1];
			need[num][2] -= request[2];
			if (need[num][0] == 0 && need[num][1] == 0 && need[num][2] == 0) {
				available[0] += allocation[num][0];
				available[1] += allocation[num][1];
				available[2] += allocation[num][2];
				mcount++;
				if (mcount == NUMBER_OF_CUSTOMERS) {
					printf(" complete\n");
					print();
					exit(0);
				}
				//return;
			}
			if (safe() == 0) {
				available[0] += request[0];
				available[1] += request[1];
				available[2] += request[2];
				allocation[num][0] -= request[0];
				allocation[num][1] -= request[1];
				allocation[num][2] -= request[2];
				need[num][0] += request[0];
				need[num][1] += request[1];
				need[num][2] += request[2];
				printf(" unsafe ");
			}
			else {
				printf(" safe ");
			}
			printf("available: %d %d %d ", available[0], available[1], available[2]);
			printf(" need: %d %d %d\n", need[num][0], need[num][1], need[num][2]);
			//printf("safe?%d\n", safe());
		}
		else {
			printf("\n");
		}
		ReleaseMutex(MUTEXLOCK);//release the mutexlock
		Sleep(1000);
	}

}

int main(int argc, char* argv[]) {
	if (argc != 4) {
		fprintf(stderr, "Please input 3 param!");
		return -1;
	}
	first_num = atoi(argv[1]);
	second_num = atoi(argv[2]);
	thrid_num = atoi(argv[3]);
	if (first_num < 0 || second_num < 0 || thrid_num < 0) {
		fprintf(stderr, "The param should not be negative!");
		return -1;
	}
	srand(time(NULL));
	init();
	if (safe() == 0) {
		printf_s("Unsafe at first\n");
		return 0;
	}

	int i = 0;
	int num[NUMBER_OF_CUSTOMERS] = { 0 };
	HANDLE* ThreadHandle;
	ThreadHandle = (HANDLE*)malloc(NUMBER_OF_CUSTOMERS * sizeof(HANDLE));
	MUTEXLOCK = CreateMutex(NULL, FALSE, NULL);//create the mutexlock
	
	for (i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
		num[i] = i;
		ThreadHandle[i] = CreateThread(NULL, 0, customer, num + i , 0, NULL);
		Sleep(1000);// because precision of time() is 1 second
	}

	WaitForMultipleObjects(NUMBER_OF_CUSTOMERS, ThreadHandle, TRUE, INFINITE);

	for (i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
		CloseHandle(ThreadHandle[i]);
	}
	return 0;
}

