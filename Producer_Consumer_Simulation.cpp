#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <iostream>
#define THREAD_NUM  5
#define SIZE 10
#define EMPTY -1
#define RUNTIME 10
int bucket[SIZE];
pthread_mutex_t mutex;
pthread_cond_t mutex_consume;
pthread_cond_t mutex_produce;
int producer_counter,consumer_counter = 0;
int count=0;
bool isRunning = true;

void* producer(void* param) {
	while(isRunning)
	{	
		pthread_mutex_lock(&mutex);
		while(count>=SIZE){
			pthread_cond_wait(&mutex_consume,&mutex);
		}
		// Produce
		int x = rand()%100;
		//Add to bucket
		bucket[count] = x;
		producer_counter++;
		printf("Thread producerThread[%d] producer value %d in the bucket[%d]\n", (int)(long)param, x, count);
		count++;
		
		pthread_cond_signal(&mutex_produce);
		pthread_mutex_unlock(&mutex);
	}
	return EXIT_SUCCESS;
}

void* consumer(void* param){
	//Consumer print out bucket
	while(isRunning){
		pthread_mutex_lock(&mutex);
		while(count<=0){
			pthread_cond_wait(&mutex_produce,&mutex);
		}
		int y = bucket[count-1];
		count--;
		bucket[count]=EMPTY;
		printf("Thread consumerThread[%d] consumed value %d in the bucket[%d]\n", (int)(long)param, y, count);
		consumer_counter++;
		pthread_cond_signal(&mutex_consume);
		pthread_mutex_unlock(&mutex);
	}
	return EXIT_SUCCESS;
}



int main(int argc, char** argv) {
	
	srand(time(NULL));
	pthread_t producerthread[THREAD_NUM], consumerthread[THREAD_NUM];
	pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&mutex_produce, NULL);
    pthread_cond_init(&mutex_consume, NULL);
    
	for (int i = 0; i < SIZE; i++)
    {
        bucket[i] = EMPTY;
    }
	for (int i = 0; i < THREAD_NUM; i++) {
		if (pthread_create(&producerthread[i], NULL, producer, (void*)(long)i) != 0) {
			perror("Fail to create consumer thread");
		}
		if (pthread_create(&consumerthread[i], NULL, consumer, (void*)(long)i) != 0) {
			perror("Fail to create consumer thread");
		}
	}
	sleep(RUNTIME);
	isRunning = false;

	for (int i = 0; i < THREAD_NUM; i++) {
		if (pthread_join(producerthread[i], NULL) != 0) {
			perror("Failed to join thread");
		}
		if (pthread_join(consumerthread[i], NULL) != 0) {
			perror("Failed to join thread");
			}
		}
		
		pthread_mutex_destroy(&mutex);
		pthread_cond_destroy(&mutex_produce);
		pthread_cond_destroy(&mutex_consume);
		printf("Program exit after %d seconds",RUNTIME);
		return EXIT_SUCCESS;
	
	
}

