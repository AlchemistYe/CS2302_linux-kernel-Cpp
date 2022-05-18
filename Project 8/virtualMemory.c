# include <stdio.h>
# include <stdlib.h>
# include <string.h>

# define pageNumber 256
# define pageSize 256
# define frameNumber 128
# define frameSize 256
# define TLBNumber 16

//free_frame_list
struct free_frame_list {
	int frame_id;
	struct free_frame_list *next;
};

struct free_frame_list *head = NULL;
struct free_frame_list *tail = NULL;

void add_free_frame(int frame_id);

int get_free_frame();



//TLB
int TLB_page[TLBNumber],TLB_frame[TLBNumber];
int TLB_LRU[TLBNumber];
int hit_count;

int get_TLB_frame(int page_id);

void update_TLB(int page_id, int frame_id);

void delete_TLB(int page_id, int frame_id);



//page table
int page_table[pageNumber], valid_table[pageNumber];
int page_fault;

int get_frame(int page_id);

void delete_frame(int frame_id);



//memory
char memory[frameNumber*frameSize];
int frame_LRU[frameNumber];
char buffer[frameSize];
FILE *backing_store;

int add_into_memory(int page_id);

char access_memory(int frame_id, int offset);



//MAIN
int main(int argc, int *argv[]){
   if(argc!=2){
      printf("Invalid input\n");
      exit(1);   
   }
   
   FILE *input=fopen(argv[1],"r");
   if(input==NULL){
      printf("No such file\n");
      exit(1);
   }
   
   FILE *output=fopen("output.txt","w");
   
   page_fault=0;
   for(int i=0;i<pageNumber;i++){
      page_table[i] = 0;
      valid_table[i]=0;
   }

   hit_count=0;
   for(int i=0;i<TLBNumber;i++){
      TLB_page[i]=0;
      TLB_frame[i]=0;
      TLB_LRU[i]=0;  
   }
      
   backing_store=fopen("BACKING_STORE.bin", "rb");
   if(backing_store==NULL){
      printf("No BACKING_STORE.bin\n");
      exit(1);
   }
   for(int i=0;i<frameNumber;i++)
      add_free_frame(i);
      
   for(int i=0;i<frameNumber;i++)
      frame_LRU[i]=0;
   
   
   int address, page_id, frame_id, offset, value, count=0;
   while(~fscanf(input,"%d",&address)){
      count++;
      address=address&0x0000ffff;
      offset=address&0x000000ff;
      page_id=(address>>8)&0x000000ff;
      frame_id=get_frame(page_id);   
      value=(int)access_memory(frame_id,offset);
      fprintf(output, "Virtual address: %d Physical address: %d Value: %d\n", address, (frame_id << 8) + offset, value);
   }
   
   printf("  TLB hit rate: %.4f %%\n  Page fault rate: %.4f %%\n", 100.0 * hit_count / count, 100.0 * page_fault / count);
   
   if(head!=NULL){
      struct free_frame_list *temp;
      while (head != tail) {
	temp = head;
	head = head -> next;
	free(temp);
      }
      free(head);
      head = tail = NULL;   
   }
   
   
   fclose(backing_store);
   fclose(input);
   fclose(output);
   return 0;
}



void add_free_frame(int frame_id){
   if(head==NULL&&tail==NULL){
      tail=(struct free_frame_list *)malloc(sizeof(struct free_frame_list));
      tail->next=NULL;
      tail->frame_id=frame_id;
      head=tail;
   }
   else{
      tail->next=(struct free_frame_list *)malloc(sizeof(struct free_frame_list));
      tail->next->frame_id=frame_id;
      tail->next->next=NULL;
      tail=tail->next;
  }
}



int get_free_frame(){
   if(head==NULL&&tail==NULL)return -1;

   int frame_id;
   if(head==tail){
      frame_id=head->frame_id;
      free(head);
      head=NULL;
      tail=NULL;
      return frame_id;  
   }
   
   struct free_frame_list *temp;	
   frame_id = head -> frame_id;
   temp = head;
   head = head -> next;
   free(temp);
   
   return frame_id;
}




int get_TLB_frame(int page_id){
   int index=-1;
   for(int i=0;i<TLBNumber;i++){
      if(TLB_LRU[i]>0&&TLB_page[i]==page_id){
         index=i;
         break;
      }   
   }

   if(index==-1)return -1;
   
   hit_count++;
   for(int i=0;i<TLBNumber;i++){
      if(TLB_LRU[i]>0&&TLB_LRU[i]<TLB_LRU[index])
         TLB_LRU[i]++;   
   }
   TLB_LRU[index]=1;
   
   return TLB_frame[index];
  
}



void update_TLB(int page_id, int frame_id){
   int index=-1;
   for(int i=0;i<TLBNumber;i++)
      if(TLB_LRU[i]==0){
         index=i;
         break;
      }  
   
   
   if(index==-1){
      for(int i=0;i<TLBNumber;i++)
         if(TLB_LRU[i]==TLBNumber){   
            index=i;
            break;
         }
   }
   
  
   TLB_page[index]=page_id;
   TLB_frame[index]=frame_id;
   
   for(int i=0;i<TLBNumber;i++)
      if(TLB_LRU[i]>0)
         TLB_LRU[i]++;   
         
   TLB_LRU[index]=1;
}



void delete_TLB(int page_id, int frame_id){
   int index=-1;
   for(int i=0;i<TLBNumber;i++)
      if(TLB_LRU[i]>0&&TLB_page[i]==page_id&&TLB_page[i]==frame_id){
         index=i;
         break;
      }

   if(index==-1)return;

   for(int i=0;i<TLBNumber;i++)
      if(TLB_LRU[i]>TLB_LRU[index])
         TLB_LRU[i]--;

   TLB_LRU[index]=0;  
}



int get_frame(int page_id){
   if(page_id<0||page_id>=pageNumber)return -1;
   
   int TLB_frame=get_TLB_frame(page_id);
   if(TLB_frame!=-1)return TLB_frame;
   
   if(valid_table[page_id]==0){
      page_fault++;
      page_table[page_id] = add_into_memory(page_id);
      valid_table[page_id]=1;
      update_TLB(page_id,page_table[page_id]);
      return page_table[page_id];
   }
   else{
      update_TLB(page_id,page_table[page_id]);
      return page_table[page_id];
   }

}




void delete_frame(int frame_id){
   int page_id=-1;
   for(int i=0;i<pageNumber;i++)
      if(valid_table[i]==1&&page_table[i]==frame_id){
         page_id=i;
         break;
      }   
   
 
   if(page_id==-1){
      printf("No this frame_id in memory\n");
      exit(1);
   }
   
   valid_table[page_id]=0;
   
   delete_TLB(page_id,frame_id);
}



int add_into_memory(int page_id){
   fseek(backing_store, page_id * frameSize, SEEK_SET);
   fread(buffer, sizeof(char), frameSize, backing_store);
   
   int frame_id=get_free_frame();
   if(frame_id==-1){
       for(int i=0;i<frameNumber;i++){
          if(frame_LRU[i]==frameNumber){
             frame_id=i;
             break;
          }       
       }
       
       delete_frame(frame_id);
   }

   for(int i=0;i<frameSize;i++)
      memory[frame_id*frameSize+i]=buffer[i];
   for(int i=0;i<frameNumber;i++)
      if(frame_LRU[i]>0)frame_LRU[i]++;
   
   frame_LRU[frame_id]=1;
   
   return frame_id;
}




char access_memory(int frame_id, int offset){
   char value=memory[frame_id*frameSize+offset];
   for(int i=0;i<frameNumber;i++)
      if(frame_LRU[i]>0&&frame_LRU[i]<frame_LRU[frame_id])
         frame_LRU[i]++;
   
   frame_LRU[frame_id]=1;    
        
   return value;
}







