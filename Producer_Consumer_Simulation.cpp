#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <iostream>
#define THREAD_NUM  5
#define SIZE 10
#define EMPTY -1
#define RUNTIME 10
int bucket[SIZE];
pthread_mutex_t mutex;
sem_t consumed, produced;
int producer_counter,consumer_counter = 0;
int count;
bool ISRUNNING = true;

void* producer(void* param) {
	while(ISRUNNING)
	{	
		sem_wait(&consumed);
		pthread_mutex_lock(&mutex);

		// Produce
		int x = rand()%100;
		//Add to bucket
		bucket[count] = x;
		producer_counter++;
		printf("Thread producerThread[%d] producer value %d in the bucket[%d]\n", (int)(long)param, x, count);
		count++;
		pthread_mutex_unlock(&mutex);
		sem_post(&produced);
	}
	return EXIT_SUCCESS;
}

void* consumer(void* param){
	//Consumer print out bucket
	while(ISRUNNING){
		sem_wait(&produced);
		pthread_mutex_lock(&mutex);
		int y = bucket[count-1];
		count--;
		bucket[count]=EMPTY;
		printf("Thread consumerThread[%d] consumed value %d in the bucket[%d]\n", (int)(long)param, y, count);
		consumer_counter++;
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
		if (pthread_create(&producerthread[i], NULL, producer, (void*)(long)i)) {
			perror("Fail to create consumer thread");
		}
		if (pthread_create(&consumerthread[i], NULL, consumer, (void*)(long)i)) {
			perror("Fail to create consumer thread");
		}
	}
	sleep(RUNTIME);
	ISRUNNING = false;

	for (int i = 0; i < THREAD_NUM; i++) {
		if (pthread_join(producerthread[i], NULL) != 0) {
			perror("Failed to join thread");
		}
		if (pthread_join(consumerthread[i], NULL) != 0) {
			perror("Failed to join thread");
			}
		}
		
		pthread_mutex_destroy(&mutex);
		sem_destroy(&consumed);
		sem_destroy(&produced);
		printf("Program exit after %d seconds",RUNTIME);
		return EXIT_SUCCESS;
	
	
}

