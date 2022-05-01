# include <stdio.h>
# include <stdlib.h>
# include <string.h>

# define MAX_LINE 80

typedef struct node{
    int begin,end; 
      
    char *id;  

    struct node *next;
     
}mem_node;

mem_node *head=NULL;

int memoryspace;


void simplify(char *string);

void clear(char *string);

int request(char *process_id, int size, char type);

int release(char *process_id);

void compact();

void status_display();


int main(int argc, char *argv[]){
   if(argc!=2){
       printf("Invalid input\n");
       exit(1);
   }    
   memoryspace=atoi(argv[1]);
   char op[MAX_LINE], process_id[MAX_LINE], sub_op[MAX_LINE];
   
   while(1){
       clear(op);
       clear(process_id);
       clear(sub_op);

       printf("allocator>");
       fgets(op,MAX_LINE,stdin);
       simplify(op);
       
       if(strcmp(op,"X")==0)break;
       
       if(strcmp(op,"STAT")==0){
           status_display();
           continue;
       }
 
       if(strcmp(op,"C")==0){
           compact();
           continue;
       }
       
       int i=0;
       for(;op[i];i++) 
             if(op[i]==' ')break;
       for(int j=0;j<i;j++)
             sub_op[j]=op[j];
       sub_op[i]=0;
       
       if(strcmp(sub_op,"RQ")==0){
           int position=3;
           for(;op[position];position++){
                if(op[position]==' ')break;
                process_id[position-3]=op[position];
           }
         
           if(op[position]==0){
                printf("Invalid input\n");
                continue;
           }
           
           int error=0;
           int size=0;
           position++;  
           for(int i=position;op[i];i++){
                if(op[i]==' ')break;
                position++;
                if(op[i]<'0'||op[i]>'9'){
                      error=1;
                      break;
                }
                size=size*10+op[i]-'0';                
           }
           
           if(error==1||op[position]==0){
                printf("Invalid input\n");
                continue;   
           }
           
           position++;
           if((op[position]!='W')&(op[position]!='B')&(op[position]!='F')){
                printf("Invalid input\n");
                continue;   
           }
           
           if(op[position+1]!=0){
               printf("Invalid input\n");
               continue;     
           }
                 
           if(request(process_id,size,op[position])==0)printf("Request accepted\n");           
           else printf("Request denied\n");
       }
       else if(strcmp(sub_op,"RL")==0){
                int position=3;
                int error=0;
                for(;op[position];position++){
                     if(op[position]==' '){
                          error=1;
                          break;
                     }
                     process_id[position-3]=op[position];
                }
      
                if(error){
                     printf("Invalid input\n");
                     continue;   
                } 
                else{
                     if(release(process_id)==0)printf("Release accepted\n");
                     else printf("Release denied\n");
                }
            }
            else     
                printf("Invalid input\n");
   
   }

   return 0; 
}


void simplify(char *string){
   int length=strlen(string);
   char *temp=(char*)malloc(length * sizeof(char));
   for(int i=0;i<length;i++){
      temp[i]=string[i];   
   }
   clear(string);
   
   int newlen=0,flag=1;
   for(int i=0;i<length;i++){
      if(temp[i]==' '||temp[i]=='\n'||temp[i]=='\t'){
         if(!flag){  
            flag=1;
            string[newlen++]=' ';
         }
      }
      else{ 
          string[newlen++]=temp[i];
          flag=0;
      } 
   }
   if(string[newlen-1]==' ')string[newlen-1]=0;  
   
   free(temp);
}




void clear(char *string){
   memset(string,0,sizeof(string));
}



int request(char *process_id, int size, char type){
   int name_length=strlen(process_id);
   if(head==NULL){
         if(size<=memoryspace){
                head=(mem_node *)malloc(sizeof(mem_node));
                head->id=(char *)malloc(sizeof(char)*(name_length));
                strcpy(head->id,process_id);
                head->begin=0;
                head->end=size-1;
                head->next=NULL;
                return 0;
         }
         else return 1;
   }
   
   mem_node *h=head;
   while(h!=NULL){
         if(strcmp(h->id,process_id)==0){
               printf("The process already exists!\n");
               return 1;
         }
         h=h->next;
   }

   if(type=='F'){
         mem_node *n=head;
         int hole_length;
         
         hole_length=n->begin;
         if(size<=hole_length){
                mem_node *temp;
                temp=(mem_node *)malloc(sizeof(mem_node));
                temp->id=(char *)malloc(sizeof(char)*(name_length));
                strcpy(temp->id,process_id);
                temp->begin=0;
                temp->end=size-1;
                temp->next=head;
                head=temp;
                return 0;         
         }
         
         while(n->next!=NULL){
                hole_length=n->next->begin-n->end-1;
                if(size<=hole_length){
                       mem_node *temp;
                       temp=(mem_node *)malloc(sizeof(mem_node));
                       temp->id=(char *)malloc(sizeof(char)*(name_length));
                       strcpy(temp->id,process_id);
                       temp->begin=n->end+1;
                       temp->end=n->end+size;
                       temp->next=n->next;
                       n->next=temp;                   
                       return 0;  
                } 
                n=n->next;  
         }  
   
         hole_length=memoryspace-n->end-1;
         if(size<=hole_length){
                mem_node *temp;
                temp=(mem_node *)malloc(sizeof(mem_node));
                temp->id=(char *)malloc(sizeof(char)*(name_length));
                strcpy(temp->id,process_id);
                temp->begin=n->end+1;
                temp->end=n->end+size;
                n->next=temp;
                temp->next=NULL;
                return 0;         
         } 
       
   }
   
   if(type=='B'){
         mem_node *n=head;
         int hole_length;
         int cur_min=memoryspace;
         int flag=0;
         mem_node *min;
         
         hole_length=n->begin;
         if(size<=hole_length&&hole_length<cur_min){
                cur_min=hole_length;
                flag=1;         
         }
         
         while(n->next!=NULL){
                hole_length=n->next->begin-n->end-1;
                if(size<=hole_length&&hole_length<cur_min){
                        cur_min=hole_length;
                        flag=2;
                        min=n;
                }
                n=n->next;        
         }
           
         hole_length=memoryspace-n->end-1;
         if(size<=hole_length&&hole_length<cur_min){
                cur_min=hole_length;
                flag=3;   
         }
         
         if(flag==0)return 1;
         
         if(flag==1){
                mem_node *temp;
                temp=(mem_node *)malloc(sizeof(mem_node));
                temp->id=(char *)malloc(sizeof(char)*(name_length));
                strcpy(temp->id,process_id);
                temp->begin=0;
                temp->end=size-1;
                temp->next=head;
                head=temp;
                return 0;   
         }       
           
         if(flag==2){
                mem_node *temp;
                temp=(mem_node *)malloc(sizeof(mem_node));
                temp->id=(char *)malloc(sizeof(char)*(name_length));
                strcpy(temp->id,process_id);
                temp->begin=min->end+1;
                temp->end=min->end+size;
                temp->next=min->next;
                min->next=temp;                   
                return 0;  
         }
         
         if(flag==3){
                mem_node *temp;
                temp=(mem_node *)malloc(sizeof(mem_node));
                temp->id=(char *)malloc(sizeof(char)*(name_length));
                strcpy(temp->id,process_id);
                temp->begin=n->end+1;
                temp->end=n->end+size;
                n->next=temp;
                temp->next=NULL;
                return 0;   
         }
        
   }
   
   if(type=='W'){
         mem_node *n=head;
         int hole_length;
         int cur_max=0;
         int flag=0;
         mem_node *max;
         
         hole_length=n->begin;
         if(size<=hole_length&&hole_length>cur_max){
                cur_max=hole_length;
                flag=1;         
         }
         
         while(n->next!=NULL){
                hole_length=n->next->begin-n->end-1;
                if(size<=hole_length&&hole_length>cur_max){
                        cur_max=hole_length;
                        flag=2;
                        max=n;
                }
                n=n->next;        
         }
           
         hole_length=memoryspace-n->end-1;
         if(size<=hole_length&&hole_length>cur_max){
                cur_max=hole_length;
                flag=3;   
         }
         
         if(flag==0)return 1;
         
         if(flag==1){
                mem_node *temp;
                temp=(mem_node *)malloc(sizeof(mem_node));
                temp->id=(char *)malloc(sizeof(char)*(name_length));
                strcpy(temp->id,process_id);
                temp->begin=0;
                temp->end=size-1;
                temp->next=head;
                head=temp;
                return 0;   
         }       
           
         if(flag==2){
                mem_node *temp;
                temp=(mem_node *)malloc(sizeof(mem_node));
                temp->id=(char *)malloc(sizeof(char)*(name_length));
                strcpy(temp->id,process_id);
                temp->begin=max->end+1;
                temp->end=max->end+size;
                temp->next=max->next;
                max->next=temp;                   
                return 0;  
         }
         
         if(flag==3){
                mem_node *temp;
                temp=(mem_node *)malloc(sizeof(mem_node));
                temp->id=(char *)malloc(sizeof(char)*(name_length));
                strcpy(temp->id,process_id);
                temp->begin=n->end+1;
                temp->end=n->end+size;
                n->next=temp;
                temp->next=NULL;
                return 0;   
         }
   
   
   }
   
   
   
   return 1;
}



int release(char *process_id){
   if(head==NULL)return 1;
   if(strcmp(head->id,process_id)==0){
         mem_node *temp=head; 
         head=head->next;
         free(temp->id);
         free(temp);
         return 0;
   }  
   
   mem_node *n=head;
   while(n->next!=NULL){
         if(strcmp(n->next->id,process_id)==0){
               mem_node *temp=n->next;
               n->next=n->next->next;
               free(temp->id);
               free(temp);
               return 0;
         }  
         n=n->next;
   }
    
   return 1;
}



void compact(){
   int current=0;
   mem_node *n=head;
   while(n!=NULL){
       int size=n->end-n->begin+1;
       n->begin=current;
       n->end=current+size-1;
       current+=size;
       n=n->next;
   }
   printf("Compaction completed\n");
   return;   
}



void status_display(){
   if(head==NULL){
         printf("Address [0 : %d] Unused\n", memoryspace-1);
	 return;
   }else if(head->begin!=0)
	     printf("Address [0 : %d] Unused\n", head->begin-1);
    
   mem_node *n=head;
   int hole_length;
   while(n->next!=NULL){
         printf("Address [%d : %d] Process %s\n",n->begin,n->end,n->id);  
         hole_length=n->next->begin-n->end-1;
         if(hole_length>0)
             printf("Address [%d : %d] Unused\n", n->end+1,n->next->begin-1);
         n=n->next;
   }
   
   printf("Address [%d : %d] Process %s\n",n->begin,n->end,n->id);  
   hole_length=memoryspace-1-n->end;
   if(hole_length>0)
         printf("Address [%d : %d] Unused\n", n->end+1,memoryspace-1);

}




