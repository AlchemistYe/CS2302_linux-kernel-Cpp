#include "buffer.h"
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>

#define TRUE 1

pthread_mutex_t mutex;
sem_t empty,full;
int shutflag;

void *producer(void *param);
void *consumer(void *param);

int main(int argc,char *argv[]){
     if(argc!=4){
         printf("Invalid input\n");
         exit(1);
     }
     int sleeptime,producer_number,consumer_number;
     sleeptime=atoi(argv[1]);
     producer_number=atoi(argv[2]);
     consumer_number=atoi(argv[3]);
     
     buffer_initialization();
     shutflag=0;
     
     static int error;
     error=pthread_mutex_init(&mutex,NULL);
     if(error){
         printf("Mutex initialization failed\n");
         exit(1);
     }
     error=sem_init(&empty,0,BUFFER_SIZE);
     if(error){
         printf("Semaphore 'empty' initialization failed\n");
         exit(1);
     }
     error=sem_init(&full,0,0);
     if(error){
         printf("Semaphore 'full' initialization failed\n");
         exit(1);
     }
     
     pthread_t *pro,*con;
     pro=(pthread_t *)malloc(sizeof(pthread_t)*producer_number);
     con=(pthread_t *)malloc(sizeof(pthread_t)*consumer_number);
     for(int i=0;i<producer_number;i++){
         error=pthread_create(&pro[i],NULL,&producer,NULL);
         if(error){
              printf("pthread_create failed\n");
              exit(1);
         }
     }
     for(int i=0;i<consumer_number;i++){
         error=pthread_create(&con[i],NULL,&consumer,NULL);
         if(error){
              printf("pthread_create failed\n");
              exit(1);
         }
     }
     
     sleep(sleeptime);
     
     shutflag=1;
     for(int i=0;i<producer_number;i++)
         sem_post(&empty);     
     for(int i=0;i<consumer_number;i++)
         sem_post(&full);
     for(int i= 0;i<producer_number;i++){
	 error=pthread_join(pro[i], NULL);
         if(error){
              printf("pthread_join failed\n");
              exit(1);
         }
     }
     for(int i=0;i<consumer_number;i++){
         error=pthread_join(con[i],NULL);
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
     error=sem_destroy(&empty);
     if(error){
         printf("Destroying semaphore failed\n");
         exit(1);
     }
     error=sem_destroy(&full);
     if(error){
         printf("Destroying semaphore failed\n");
         exit(1);
     }
     printf("The program has terminated\n");
     free(pro);
     free(con);
     return 0;   
}


void *producer(void *param){
     buffer_item item;
     while(TRUE){
         static int sleeptime;
         sleeptime=rand()%5;
         sleep(sleeptime);
         
         item=rand();
         
         sem_wait(&empty);
         if(shutflag)break;
         pthread_mutex_lock(&mutex);
         if(insert_item(item)){
             printf("Producer failed to insert\n");
             exit(1);         
         }
         else printf("Producer inserted item %d\n",item);
         pthread_mutex_unlock(&mutex);
         sem_post(&full);         
     }
     pthread_exit(0);
}


void *consumer(void *param){
     buffer_item item; 
     while(TRUE){
         static int sleeptime;
         sleeptime=rand()%5;
         sleep(sleeptime);
         
         item=rand();
         
         sem_wait(&full);
         if(shutflag)break;
         pthread_mutex_lock(&mutex);
         if(remove_item(&item)){
             printf("Consumer failed to remove\n");
             exit(1);
         }
         else printf("Consumer removed item %d\n",item);
         pthread_mutex_unlock(&mutex);
         sem_post(&empty);     
     }
     pthread_exit(0);
}











