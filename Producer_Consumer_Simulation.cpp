#include <stdio.h>
#include <cstdio>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <iostream>

using namespace std;
#define THREAD_NUM  5
#define SIZE 10
#define EMPTY -1
#define RUNTIME 10
#define bufferSize 100

int bucket[SIZE];
pthread_mutex_t mutex;
sem_t consumed, produced;
int count;
bool ISRUNNING = true;
void* producer(void* param);
void* consumer(void* param);
void printMsg(const char*,int,const char*,int,int);
//////////////////////////////////////////////////////////////////////
void printMsg(const char* type,int p1,const char* action,int value,int p2){
	char buffer[bufferSize];
    snprintf(buffer,bufferSize,"Thread %sThread[%d] %s value %d in bucket[%d]\n",type,p1,action,value,p2);
    cout<<buffer;
}
//////////////////////////////////////////////////////////////////////
void* producer(void* param) {
	while(ISRUNNING)
	{	
		sem_wait(&consumed);
		pthread_mutex_lock(&mutex);

		// Produce
		int x = rand()%100;
		//Add to bucket
		bucket[count] = x;
		printMsg("producer",(int)(long)param,"produce",x,count);
		count++;
		pthread_mutex_unlock(&mutex);
		sem_post(&produced);
	}
	return EXIT_SUCCESS;
}
//////////////////////////////////////////////////////////////////////
void* consumer(void* param){
	//Consumer print out bucket
	while(ISRUNNING){
		sem_wait(&produced);
		pthread_mutex_lock(&mutex);
		int y = bucket[count-1];
		count--;
		bucket[count]=EMPTY;
		printMsg("consumer",(int)(long)param,"consume",y,count);
		pthread_mutex_unlock(&mutex);
		sem_post(&consumed);
	}
	return EXIT_SUCCESS;
}



int main(int argc, char** argv) {
	
	srand(time(NULL));
	pthread_t producerthread[THREAD_NUM], consumerthread[THREAD_NUM];
	pthread_mutex_init(&mutex, NULL);
    sem_init(&consumed, 0, SIZE);
    sem_init(&produced, 0, 0);
    
	for (int i = 0; i < SIZE; i++)
    {
        bucket[i] = EMPTY;
    }
	for (int i = 0; i < THREAD_NUM; i++) {
		pthread_create(&producerthread[i], NULL, producer, (void*)(long)i);
		pthread_create(&consumerthread[i], NULL, consumer, (void*)(long)i);
		
	}
	sleep(RUNTIME);
	ISRUNNING = false;

	for (int i = 0; i < THREAD_NUM; i++) {
		pthread_join(producerthread[i], NULL);
		pthread_join(consumerthread[i], NULL);
		
	}
	pthread_mutex_destroy(&mutex);
	sem_destroy(&consumed);
	sem_destroy(&produced);
	return EXIT_SUCCESS;
	
	
}

