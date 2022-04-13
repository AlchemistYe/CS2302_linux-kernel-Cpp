#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>


int customers;
int capacity;
int resources;
int *available;
int **maximum;
int **allocation;
int **need;
int *input_array;

int request_resources(int customer, int request[]);

void release_resources(int customer, int release[]);

void update(int **need,int **maximum,int **allocation);

void get_output();

int simplify(char *inst,char *op,int *argn);

int main(int argc, char *argv[]){
     customers=0;
     capacity=20;
     resources=argc-1;
     if(resources<=0){
         printf("Invalid input\n");
         exit(1);
     }
          
     maximum=(int **)malloc(sizeof(int *)*capacity);
     available=(int *)malloc(sizeof(int)*capacity);
     allocation=(int **)malloc(sizeof(int *)*capacity);
     need=(int **)malloc(sizeof(int *)*capacity);
     
     for(int i=1;i<=resources;i++)
         available[i-1]=atoi(argv[i]);
     
     FILE *file=fopen("maximum.txt","r");
     int number=0;
     
     while(fscanf(file,"%d",&number)!=-1){
         if(customers==capacity){
              int **temp;
              temp=(int **)malloc(sizeof(int *)*capacity*2);
              for(int i=0;i<capacity;i++){
                    temp[i]=(int *)malloc(sizeof(int)*resources); 
                    for(int j=0;j<resources;j++)
                         temp[i][j]=maximum[i][j];
                    free(maximum[i]);
              }    
              free(maximum);   
              maximum=temp;
              capacity=capacity*2;
         }      
        
         maximum[customers]=(int *)malloc(sizeof(int)*resources);
         maximum[customers][0]=number;
         for(int j=1;j<resources;j++){
              fscanf(file,",%d",&number);
              maximum[customers][j]=number;         
         }   
         customers++;
     }    
     fclose(file);
         
     for(int i=0;i<customers;i++){
         allocation[i]=(int *)malloc(sizeof(int)*resources);
         for(int j=0;j<resources;j++)
              allocation[i][j]=0;
     }
     
     for(int i=0;i<customers;i++)
         need[i]=(int *)malloc(sizeof(int)*resources);
        
     update(need,maximum,allocation);
     
     for(int i=0;i<customers;i++)
        for(int j=0;j<resources;j++)
             if(maximum[i][j]>available[j]){
                  printf("Initial state is unsafe\n");
                  exit(1);
             }
     
     get_output();
     
     char inst[50],op[2];
     int argn;
     input_array=(int *)malloc(sizeof(int)*(resources+1));
     
     while(1){
          printf("Input your instruction>>  ");
          fgets(inst,50,stdin);  
          argn=-1;
          if(simplify(inst,op,&argn)){
                printf("Invalid instruction\n");
                continue;        
          }         
          
          if(strcmp(op,"exit")==0&&argn==0)break;
          if(strcmp(op,"*")==0&&argn==0)get_output();
          else{
             if(strcmp(op,"RQ")==0&&argn==resources+1){
                  if(request_resources(input_array[0],input_array+1)==0)printf("Request command granted\n");
                  else printf("Request command denied\n");
             }
             else if(strcmp(op,"RL")==0&&argn==resources+1)
                       release_resources(input_array[0],input_array+1);          
                  else{
                       printf("Invalid instruction\n");           
                       continue;
                 }
          }         
     } 
     
     free(available);
     free(input_array);
     for(int i=0;i<customers;i++){
          free(maximum[i]);
          free(allocation[i]);
          free(need[i]);
     }
     free(maximum);
     free(allocation);
     free(need);
     
     return 0;
}



void update(int **need,int **maximum,int **allocation){
     for(int i=0;i<customers;i++)
         for(int j=0;j<resources;j++)
              need[i][j]=maximum[i][j]-allocation[i][j];
}



void get_output(){
     printf("The current state is:\n");
     printf(" Number of customers: %d\n",customers);
     printf(" Number of resources: %d\n",resources);
 
     printf(" Available = [ ");
     for(int i=0;i<resources;i++)
          if(i<resources-1)printf("%d, ",available[i]);
          else printf("%d ]\n",available[i]);
     
     for(int i=0;i<customers;i++){
          if(i==0){
              printf(" Maximum = \n");    
              printf("        [ ");
          }
          else printf("        [ ");
          for(int j=0;j<resources;j++){
              if(j<resources-1)printf("%d, ",maximum[i][j]);
              else printf("%d ]\n",maximum[i][j]);
          }
     }
     
     for(int i=0;i<customers;i++){
          if(i==0){
              printf(" Allocation = \n");    
              printf("        [ ");
          }
          else printf("        [ ");
          for(int j=0;j<resources;j++){
              if(j<resources-1)printf("%d, ",allocation[i][j]);
              else printf("%d ]\n",allocation[i][j]);
          }
     }
     
     for(int i=0;i<customers;i++){
          if(i==0){
              printf(" Need = \n");    
              printf("        [ ");
          }  
          else printf("        [ ");
          for(int j=0;j<resources;j++){
              if(j<resources-1)printf("%d, ",need[i][j]);
              else printf("%d ]\n",need[i][j]);
          }
     }    
}



int simplify(char *inst,char *op,int *argn){
      int flag,op_index,temp;
      flag=0;op_index=0;temp=0;
      for(int i=0;inst[i];i++){
           if(inst[i]==' '||inst[i]=='\t'||inst[i]=='\n'){
                if(flag==0)continue;
                flag=0;
                if(*argn!=-1){
                    if(*argn==resources+1)return 1;
                    input_array[*argn]=temp;
                    temp=0;
                }
                (*argn)++;
           }
           else{
                flag=1;
                if(*argn==-1)op[op_index++]=inst[i];
                else{
                     if(inst[i]>='0'&&inst[i]<='9')temp=temp*10+inst[i]-'0';
                     else return 1;
                }
           }      
      }
      if(flag){
           if(*argn!=-1){
                if(*argn==resources+1)return 1;
                input_array[*argn]=temp;
                temp=0;
           }     
           (*argn)++; 
      }
      op[op_index]=0;
      return 0;
}



int request_resources(int customer, int request[]){
       int *temp_available,*sequence;
       
       for(int i=0;i<resources;i++)
            if(request[i]>available[i]){
                 printf("Available resources are not enough\n");
                 return -1;
            }
          
       for(int i=0;i<resources;i++)     
            if(request[i]>need[customer][i]){
                 printf("Request is larger than need\n");
                 return -1;
            }  
       
       temp_available=(int *)malloc(sizeof(int)*resources);
       sequence=(int *)malloc(sizeof(int)*customers);
       
       for(int i=0;i<resources;i++){
            temp_available[i]=available[i]-request[i];
            allocation[customer][i]+=request[i];
       }
       for(int i=0;i<customers;i++)
            sequence[i]=0;
       update(need,maximum,allocation);     
       
       int state,flag,position;
       state=0;
       for(int i=0;i<customers;i++){
            position=-1;
            for(int cus=0;cus<customers;cus++){
                  if(sequence[cus])continue;
                  flag=1;
                  for(int r=0;r<resources;r++)
                      if(need[cus][r]>temp_available[r]){
                          flag=0;                          
                          break;
                      }                   
                  if(flag){
                     position=cus;
                     break; 
                  }
            } 
            
            if(position==-1){
                 state=-1;
                 break;            
            }
            
            sequence[position]=1;
            
            for(int i=0;i<resources;i++)
                temp_available[i]+=allocation[position][i];
       }
       
       if(state==0)
           for(int i=0;i<resources;i++)
                available[i]-=request[i];
       else            
           for(int i=0;i<resources;i++){
                allocation[customer][i]-=request[i];
                update(need,maximum,allocation);       
           }
       
       free(temp_available);
       free(sequence);
       return state;
}


void release_resources(int customer, int release[]){
      for(int j=0;j<resources;j++)
          if(allocation[customer][j]<release[j]){
              printf("Release command denied\n");
              return;
          }
      
      for(int j=0;j<resources;j++){
          available[j]+=release[j];
          allocation[customer][j]-=release[j];
      }
      
      update(need,maximum,allocation);
      
      printf("Release command granted\n");
      return;
}



