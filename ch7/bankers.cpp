#include <Windows.h>
#include <process.h>
#include <iostream>
#include <stdio.h>
#include <cstdlib>
#include <string>
#include <time.h>
#include <math.h>
#include <conio.h>


HANDLE  MUTEXLOCK = NULL;

#define NUMBER_OF_CUSTOMERS 5
#define NUMBER_OF_RESOURCES 3

int first_num = 0;
int second_num = 0;
int thrid_num = 0;
int count = 0;

int status[NUMBER_OF_CUSTOMERS] = { 0 };
int available[NUMBER_OF_RESOURCES];
int maximum[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
int allocation[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
int need[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

int my_rand(int min, int max) {
	max ++;
	if (rand() == RAND_MAX)
		return max - 1;
	else
		return min + (int)(((double)max - (double)min) * rand() * 1.0 / RAND_MAX);
};

void output() {
	int i = 0, j = 0;
	printf("Available:\nrec1  rec2  rec3\n");
	for (i = 0; i < NUMBER_OF_RESOURCES; i++) {
		printf("%-5d ", available[i]);
	}
	printf("\n\nMaximum:\n\t   rec1  rec2  rec3\n");
	for (i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
		printf("Customer%d: ",i);
		for (j = 0; j < NUMBER_OF_RESOURCES; j++) {
			printf("%-5d ", maximum[i][j]);
		}
		printf("\n");
	}
	printf("\nAllocation:\n\t   rec1  rec2  rec3\n");
	
	for (i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
		printf("Customer%d: ", i);
		for (j = 0; j < NUMBER_OF_RESOURCES; j++) {
			printf("%-5d ", allocation[i][j]);
		}
		printf("\n");
	}
	printf("\nNeed:\n\t   rec1  rec2  rec3\n");
	for (i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
		printf("Customer%d: ", i);
		for (j = 0; j < NUMBER_OF_RESOURCES; j++) {
			printf("%-5d ", need[i][j]);
		}
		printf("\n");
	}
}

void init() {
	int i = 0, j = 0;
	available[0] = first_num;
	available[1] = second_num;
	available[2] = thrid_num;
	MUTEXLOCK = CreateMutex(NULL, FALSE, NULL);//create the mutexlock

	for (i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
		for (j = 0; j < NUMBER_OF_RESOURCES; j++) {
			maximum[i][j] = my_rand(0, available[j]);
			allocation[i][j] = 0;
			need[i][j] = maximum[i][j] - allocation[i][j];
		}
	}
	for (i = 0; i < NUMBER_OF_RESOURCES; i++) {
		int sum = 0;
		for (j = 0; j < NUMBER_OF_CUSTOMERS; j++) {
			sum += allocation[j][i];
		}
		available[i] -= sum;
	}
	output();
}

//0 for false, 1 for true
int safe() {
	int i = 0;
	int flag = 1;
	int count_c = 0;
	int finish[NUMBER_OF_CUSTOMERS] = { 0 };
	int work[NUMBER_OF_RESOURCES] = { 0 };
	
	for (i = 0; i < NUMBER_OF_RESOURCES; i++) 
		work[i] = available[i];
	for (i = 0; i < NUMBER_OF_CUSTOMERS; i++)
		finish[i] = 0;
	while (count_c < NUMBER_OF_CUSTOMERS - count) {
		flag = 0;
		for (i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
			if (status[i] == 0 && finish[i] == 0 && need[i][0] <= work[0] && need[i][1] <= work[1] && need[i][2] <= work[2]) {
				work[0] += allocation[i][0];
				work[1] += allocation[i][1];
				work[2] += allocation[i][2];
				finish[i] = 1;
				count_c++;
				flag = 1;
			}
		}
		if (flag == 0)
			break;
	}
	if (count_c == NUMBER_OF_CUSTOMERS - count)
		return 1;
	else
		return 0;
}

DWORD WINAPI customer(LPVOID lpParameter) {
	int num = *(int*)lpParameter;
	while (1) {
		int i = 0;
		int request[NUMBER_OF_RESOURCES];
		WaitForSingleObject(MUTEXLOCK, INFINITE);//wait for teh mutexlock
		for (i = 0; i < NUMBER_OF_RESOURCES; i++) {
			request[i] = my_rand(0, need[num][i]);
		}
		if (request[0] == 0 && request[1] == 0 && request[2] == 0) {
			ReleaseMutex(MUTEXLOCK);//release the mutexlock
			continue;
		}
		printf("\nCustomer%d request:%d %d %d", num, request[0], request[1], request[2]);
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
				printf(" \nUnsafe1! Request denied!\n");

			}
			else {
				printf(" \nSafe! Request accepted!\n");
				printf("Available: %d %d %d ", available[0], available[1], available[2]);
				printf(" Need: %d %d %d\n", need[num][0], need[num][1], need[num][2]);
			}
		}
		else {
			printf("\nUnsafe2! Request denied!\n");
		}
		if (need[num][0] == 0 && need[num][1] == 0 && need[num][2] == 0) {
			available[0] += allocation[num][0];
			available[1] += allocation[num][1];
			available[2] += allocation[num][2];
			status[num] = 1;
			count++;
			printf("Customer%d complete!\n", num);
			printf("Available: %d %d %d\n", available[0], available[1], available[2]);
			break;
		}
		ReleaseMutex(MUTEXLOCK);//release the mutexlock
		Sleep(20);
	}
	return 0;
}

int main(int argc, char* argv[]) {
	int i = 0;
	int num[NUMBER_OF_CUSTOMERS] = { 0 };
	HANDLE* ThreadHandle;
	ThreadHandle = (HANDLE*)malloc(NUMBER_OF_CUSTOMERS * sizeof(HANDLE));

	if (argc != 4) {
		fprintf(stderr, "Please enter the number of the 3 resources in the form of three parameters");
		return -1;
	}
	first_num = atoi(argv[1]);
	second_num = atoi(argv[2]);
	thrid_num = atoi(argv[3]);
	if (first_num < 0 || second_num < 0 || thrid_num < 0) {
		fprintf(stderr, "The parameters should not be negative!");
		return -1;
	}
	srand((unsigned)time(NULL));
	init();

	fprintf(stderr, "\nInitialization completed!\nPlease press any key to start trying the request...");
	_getch();
	fflush(stdin);
	printf("\n\n");

	for (i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
		num[i] = i;
		ThreadHandle[i] = CreateThread(NULL, 0, customer, num + i , 0, NULL);
	}

	WaitForMultipleObjects(NUMBER_OF_CUSTOMERS, ThreadHandle, TRUE, INFINITE);

	for (i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
		CloseHandle(ThreadHandle[i]);
	}
	return 0;
}