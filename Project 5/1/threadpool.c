#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>
#include "threadpool.h"

#define QUEUE_SIZE 10
#define NUMBER_OF_THREADS 3

#define TRUE 1

typedef struct{
    void (*function)(void *p);
    void *data;
}
task;

struct node{
    task worktodo;
    struct node* next;
}*head,*tail;

pthread_t bee[NUMBER_OF_THREADS];

pthread_mutex_t mutex;

sem_t sem;

int shutflag;

int enqueue(task t){   
    tail->next=(struct node *)malloc(sizeof(struct node));

    if(tail->next==NULL)return 1;
    tail=tail->next;
    tail->worktodo=t;
    return 0;
}

task dequeue(){
    if(head==tail){
        printf("The queue is empty\n");
        exit(1);
    }
    
    static struct node* temp;
    temp=head;
    head=head->next;
    free(temp);
    return head->worktodo;
}


void *worker(void *param){
    static task work;
    while(TRUE){
         sem_wait(&sem);
         if(shutflag)break;
         
         pthread_mutex_lock(&mutex);
         work=dequeue();
         pthread_mutex_unlock(&mutex);
         execute(work.function, work.data);
    }
    pthread_exit(0);
}


void execute(void (*somefunction)(void *p), void *p){
    (*somefunction)(p);
}


int pool_submit(void (*somefunction)(void *p), void *p){
    static task work;
    work.function = somefunction;
    work.data = p;
    
    pthread_mutex_lock(&mutex);
    int error=enqueue(work);
    pthread_mutex_unlock(&mutex);
    
    if(error==0){
        sem_post(&sem);
    }
    return error;
}


void pool_init(void){
    shutflag=0;
   
    head=(struct node *)malloc(sizeof(struct node));
    if(head==NULL){
         printf("Memory allocation failed\n");
         exit(1);
    }
    head->next=NULL;
    tail=head;
    
    int error;
    error=pthread_mutex_init(&mutex,NULL);
    if(error){
         printf("Mutex initialization failed\n");
         exit(1);
    }
    
    error=sem_init(&sem,0,0);
    if(error){
         printf("Semaphore initialization failed\n");
         exit(1);
    }
    
    for(int i=0;i<NUMBER_OF_THREADS;i++){
         error=pthread_create(&bee[i],NULL,worker,NULL);
         if(error){
             printf("pthread_create failed\n");
             exit(1);
         }
    }
    printf("Initialization completed\n");     
}

void pool_shutdown(void){
    shutflag=1;
    
    int error;
    
    for(int i=0;i<NUMBER_OF_THREADS;i++)
         sem_post(&sem);
    
    for(int i=0;i<NUMBER_OF_THREADS;i++){
         error=pthread_join(bee[i],NULL);
         if(error){
             printf("pthread_join failed\n");
             exit(1);
         }
    }
    
    error=pthread_mutex_destroy(&mutex);  
    if(error){
         printf("Destroying mutex failed\n");
         exit(1);
    }
    
    error=sem_destroy(&sem);
    if(error){
         printf("Destroying semaphore failed\n");
         exit(1);
    }
    printf("Shutdown completed\n");
}






