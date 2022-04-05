#include "buffer.h"

int head,tail;
buffer_item buffer[BUFFER_SIZE+1];

int insert_item(buffer_item item){
     if((tail+1)%(BUFFER_SIZE+1)==head)return -1;
     tail=(tail+1)%(BUFFER_SIZE+1);
     buffer[tail]=item;
     return 0;
}

int remove_item(buffer_item *item){
     if(tail==head)return -1;
     head=(head+1)%(BUFFER_SIZE+1);
     *item=buffer[head];
     return 0;
}

void buffer_initialization(){
     head=0;
     tail=0;
}
