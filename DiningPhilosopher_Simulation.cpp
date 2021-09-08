#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <iostream>
#define SIZE  5
#define RUNTIME 10
#define UPPERBOUNDTIME 1.0
#define LOWERBOUNDTIME 0.1

bool isRunning = true;
pthread_mutex_t forks[SIZE];
pthread_t thread[SIZE];
int secondForkPosition[SIZE] = {4,0,1,2,3};

//the function that will be used for this simulation

void* philosopher(void*);
void eat(int);
void think(int);
bool pickFirstFork(int);
bool pickSecondFork(int);
float RanFloat(float,float);
void putFork(int);
///////////////////////////////////////////////////////////////
int main(int argc, char** argv){
    
    srand(time(NULL));
    int i,a[5];
    for(i=0;i<SIZE;i++){
        pthread_mutex_init(&forks[i],NULL);
    }

    for(i=0;i<SIZE;i++){
        a[i]=i;
        pthread_create(&thread[i],NULL,philosopher,(void*)&a[i]);
    }
    sleep(RUNTIME);
    isRunning = false;
    for(i=0;i<SIZE;i++){
        pthread_join(thread[i],NULL);
    }
    for(i=0;i<SIZE;i++){
        pthread_mutex_destroy(&forks[i]);
    }
    printf("The program exit after %d seconds\n",RUNTIME);
    return EXIT_SUCCESS;
}
///////////////////////////////////////////////////////////////
float RanFloat(float a, float b) {
    float random = ((float) rand()) / (float) RAND_MAX;
    float diff = b - a;
    float r = random * diff;
    return a + r;
}
///////////////////////////////////////////////////////////////
void eat(int phnum){
    printf("Philosopher %d is eating\n",phnum);
}
///////////////////////////////////////////////////////////////
void think(int phnum){
    printf("Philosopher %d is thinking\n",phnum);
}
///////////////////////////////////////////////////////////////
bool pickFirstFork(int phnum){
    bool success = false;
    float think_time;
    think_time = RanFloat(UPPERBOUNDTIME,LOWERBOUNDTIME);
    sleep(think_time);
    success = pthread_mutex_trylock(&forks[phnum])==0;
    if(success){
        printf("Philosopher %d pick his own fork\n",phnum);
        return success;
    }

    printf("Philosopher %d waiting for his own fork\n",phnum);
    think(phnum);
    think_time = RanFloat(UPPERBOUNDTIME,LOWERBOUNDTIME);
    sleep(think_time);
    return success;
    }

///////////////////////////////////////////////////////////////
bool pickSecondFork(int phnum){
    bool success = false;
    float think_time;
    think_time = RanFloat(UPPERBOUNDTIME,LOWERBOUNDTIME);
    sleep(think_time);
    success = pthread_mutex_trylock(&forks[secondForkPosition[phnum]])==0;
    if(success){
        printf("Philosopher %d pick his second fork from philosopher[%d]\n",phnum,secondForkPosition[phnum]);
        return success;
    }
    
    printf("Philosopher %d is waiting for his second fork from philosopher[%d]\n",phnum,secondForkPosition[phnum]);
    think(phnum);
    think_time = RanFloat(UPPERBOUNDTIME,LOWERBOUNDTIME);
    sleep(think_time);
    return success;

}
///////////////////////////////////////////////////////////////
void putFork(int phnum){
    pthread_mutex_unlock(&forks[secondForkPosition[phnum]]);
    pthread_mutex_unlock(&forks[phnum]);
    printf("Philosopher %d has finished eating\n",phnum);
    printf("Philosopher %d left the table\n",phnum);
}
///////////////////////////////////////////////////////////////
void* philosopher(void * num){
    int phnum =*(int *) num;
    while(isRunning){
        bool success1,success2 = false;
        printf("Philosopher %d enter the table\n",phnum);
        success1 = pickFirstFork(phnum);
        success2 = pickSecondFork(phnum);
        if(success1 && success2){
            eat(phnum);
            float eat_time = RanFloat(UPPERBOUNDTIME,LOWERBOUNDTIME);
            sleep(eat_time);
        }
        putFork(phnum); 
    }
    return EXIT_SUCCESS;
}