#include <stdio.h>
#include <cstdio>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <iostream>


using namespace std;
#define SIZE  5
#define RUNTIME 10
#define UPPERBOUNDTIME 1.0
#define LOWERBOUNDTIME 0.1
#define bufferSize 100
bool isRunning = true;
pthread_mutex_t forks[SIZE];
//the function that will be used for this simulation
void* philosopher(void*);
void eat(int);
void think(int);
void pickMessage(int,const char*,const char*,int);
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
    pthread_t thread[SIZE];
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
    char buffer[bufferSize];
    snprintf(buffer,bufferSize,"Philosopher %d is eating\n",phnum);
    cout<<buffer;
}
///////////////////////////////////////////////////////////////
void think(int phnum){
    char buffer[bufferSize];
    snprintf(buffer,bufferSize,"Philosopher %d is thinking\n",phnum);
    cout<<buffer;
}
///////////////////////////////////////////////////////////////
void pickMessage(int phnum,const char* action,const char* direction, int position){
    char buffer[bufferSize];
    snprintf(buffer,bufferSize,"Philosopher %d %s his %s fork from philosopher[%d]\n",phnum,action,direction,position);
    cout<<buffer;
}
///////////////////////////////////////////////////////////////
bool pickRightFork(int phnum){
    bool success = false;
    float think_time;
    int right = RIGHT(phnum);
    think_time = RanFloat(UPPERBOUNDTIME,LOWERBOUNDTIME);
    sleep(think_time);
    success = pthread_mutex_trylock(&forks[right])==0;
    if(success){
        pickMessage(phnum,"pick","right",right);
        return success;
    }

    pickMessage(phnum,"is waiting","right",right);
    think(phnum);
    think_time = RanFloat(UPPERBOUNDTIME,LOWERBOUNDTIME);
    sleep(think_time);
    return success;
    }

///////////////////////////////////////////////////////////////
bool pickLeftFork(int phnum){
    bool success = false;
    float think_time;
    int left = LEFT(phnum);
    think_time = RanFloat(UPPERBOUNDTIME,LOWERBOUNDTIME);
    sleep(think_time);
    success = pthread_mutex_trylock(&forks[left])==0;
    if(success){
        pickMessage(phnum,"pick","left",left);
        return success;
    }
    pickMessage(phnum,"is waiting","left",left);
    think(phnum);
    think_time = RanFloat(UPPERBOUNDTIME,LOWERBOUNDTIME);
    sleep(think_time);
    return success;

}
///////////////////////////////////////////////////////////////
void putFork(int phnum){
    pthread_mutex_unlock(&forks[LEFT(phnum)]);
    pthread_mutex_unlock(&forks[RIGHT(phnum)]);
}
///////////////////////////////////////////////////////////////
void philosopherMsg(int phnum,const char* message){
    char buffer[bufferSize];
    snprintf(buffer,bufferSize,"Philosopher %d %s\n",phnum,message);
    cout<<buffer;
}
///////////////////////////////////////////////////////////////
void* philosopher(void * num){
    int phnum =*(int *) num;
    while(isRunning){
        bool successRight,successLeft = false;
        philosopherMsg(phnum,"enter the table");
        successRight = pickRightFork(phnum);
        successLeft  = pickLeftFork(phnum);
        if(successRight && successLeft){
            eat(phnum);
            float eat_time = RanFloat(UPPERBOUNDTIME,LOWERBOUNDTIME);
            sleep(eat_time);
            philosopherMsg(phnum,"has finished eating");
            philosopherMsg(phnum,"left the table");
        }
        else{
            philosopherMsg(phnum,"leave the table because folks not available");
        }
        putFork(phnum); 
    }
    return EXIT_SUCCESS;
}
