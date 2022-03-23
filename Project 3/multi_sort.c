#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct{
   int begin;
   int end;
}parameters;

int length;
int *array;
int *merge;

void *sorting(void *param);
void *merging(void *param);
int mycmp(const void* p1, const void* p2);

int main(){
     printf("Insert the length of the array\n");
     scanf("%d",&length);
      
     if(length<=0){
        printf("Illegal input\n");
        exit(1);
     }
     array = (int *) malloc (length * sizeof(int)); 
     merge = (int *) malloc (length * sizeof(int));
     printf("Please input the elements of the array\n");
     for(int i=0;i<length;i++)
        scanf("%d",&array[i]);
             
     parameters range[2];
     range[0].begin=0;
     range[0].end=(length-1)/2;
     range[1].begin=(length-1)/2+1;
     range[1].end=length-1;
     
     pthread_t tid[2];
     for(int i=0;i<2;i++){
        if(pthread_create(&tid[i],NULL,sorting,&range[i])){
             printf("Createing thread failed");
             exit(1);
        }
     }
     
     for(int i=0;i<2;i++){
        if(pthread_join(tid[i],NULL)){
             printf("Joining thread failed");
             exit(1);
        }
     }
     
     parameters range_merge;
     range_merge.begin=0;
     range_merge.end=length-1;
     
     pthread_t tid_merge;
     if(pthread_create(&tid_merge,NULL,merging,&range_merge)){
             printf("Createing thread failed");
             exit(1);
     }
     
     if(pthread_join(tid_merge,NULL)){
             printf("Joining thread failed");
             exit(1);
     }
     
     printf("The array after sorting :\n");
     for(int i=0;i<length;i++)printf("%d ",merge[i]);
     
     free(array);
     free(merge);
     return 0;  
}

int mycmp(const void* p1, const void* p2){
     const int * a = (const int *) p1;
     const int * b = (const int *) p2;
     int value = 0;
     if(*a < *b)return -1; 
     if(*a == *b)return 0;
     return 1;
} 

void *sorting(void *param){
     parameters *data=(parameters *)param;
     if(data->end<data->begin)return NULL;
      
     qsort(array+data->begin,data->end-data->begin+1,sizeof(int),mycmp);
      
     return NULL;
}

void *merging(void *param){
     parameters *data=(parameters *)param;
     if(data->end<data->begin)return NULL;
     int begin=data->begin;
     int end=data->end;
     int mid=(begin+end)/2+1;
     int i=0,j=mid,m=0;
     while(i<=mid-1&&j<=end){
          if(array[i]<array[j]){
                merge[m]=array[i];
                i++;m++;
          }    
          else{
                merge[m]=array[j];
                j++;m++;
          }          
     }
     while(i<=mid-1)merge[m++]=array[i++];
     while(j<=end)merge[m++]=array[j++];
     
     if(m!=end+1){
           printf("Merging failed\n");
           exit(1);
     }
     return NULL;
}  
