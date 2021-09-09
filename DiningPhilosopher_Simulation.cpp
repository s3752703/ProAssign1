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
int timeEat[SIZE] = {0,0,0,0,0};
//the function that will be used for this simulation
void* philosopher(void*);
void eat(int);
void think(int);
bool pickRightFork(int);
bool pickLeftFork(int);
float RanFloat(float,float);
void putFork(int);
int LEFT(int);
int RIGHT(int);
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
    int sum=0;
    for(i=0;i<SIZE;i++){
        printf("Philosopher[%d] eat %d times\n",i,timeEat[i]);
        sum = sum+ timeEat[i];
    }
    printf("total time eat: %d\n",sum);
    printf("The program exit after %d seconds\n",RUNTIME);
    return EXIT_SUCCESS;
}
///////////////////////////////////////////////////////////////
int LEFT(int phnum){
    return (phnum+4)%SIZE;
}
///////////////////////////////////////////////////////////////
int RIGHT(int phnum){
    return (phnum+1)%SIZE;
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
bool pickRightFork(int phnum){
    bool success = false;
    float think_time;
    think_time = RanFloat(UPPERBOUNDTIME,LOWERBOUNDTIME);
    sleep(think_time);
    success = pthread_mutex_trylock(&forks[RIGHT(phnum)])==0;
    if(success){
        printf("Philosopher %d pick his right fork from philosopher[%d]\n",phnum,RIGHT(phnum));
        return success;
    }

    printf("Philosopher %d waiting for his right fork from philosopher[%d]\n",phnum,RIGHT(phnum));
    think(phnum);
    think_time = RanFloat(UPPERBOUNDTIME,LOWERBOUNDTIME);
    sleep(think_time);
    return success;
    }

///////////////////////////////////////////////////////////////
bool pickLeftFork(int phnum){
    bool success = false;
    float think_time;
    think_time = RanFloat(UPPERBOUNDTIME,LOWERBOUNDTIME);
    sleep(think_time);
    success = pthread_mutex_trylock(&forks[LEFT(phnum)])==0;
    if(success){
        printf("Philosopher %d pick his left fork from philosopher[%d]\n",phnum,LEFT(phnum));
        return success;
    }
    
    printf("Philosopher %d is waiting for his left fork from philosopher[%d]\n",phnum,LEFT(phnum));
    think(phnum);
    think_time = RanFloat(UPPERBOUNDTIME,LOWERBOUNDTIME);
    sleep(think_time);
    return success;

}
///////////////////////////////////////////////////////////////
void putFork(int phnum){
    pthread_mutex_unlock(&forks[LEFT(phnum)]);
    pthread_mutex_unlock(&forks[RIGHT(phnum)]);
    printf("Philosopher %d has finished eating\n",phnum);
    printf("Philosopher %d left the table\n",phnum);
}
///////////////////////////////////////////////////////////////
void* philosopher(void * num){
    int phnum =*(int *) num;
    while(isRunning){
        bool successRight,successLeft = false;
        printf("Philosopher %d enter the table\n",phnum);
        successRight = pickRightFork(phnum);
        successLeft  = pickLeftFork(phnum);
        if(successRight && successLeft){
            eat(phnum);
            float eat_time = RanFloat(UPPERBOUNDTIME,LOWERBOUNDTIME);
            sleep(eat_time);
            timeEat[phnum]++;
        }
        putFork(phnum); 
    }
    return EXIT_SUCCESS;
}