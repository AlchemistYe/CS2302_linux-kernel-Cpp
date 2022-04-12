#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#define MAX_LINE 80 
#define READ_END 0
#define WRITE_END 1

void clear(char *string);

int check(char *string);

void simplify(char *string);

int transmit(char *instru,char **args);

int main(void)
{
  char *args[MAX_LINE/2 + 1],*instru,*last_instru;
  char *in_file,*out_file;
  int should_run = 1,concurrency=0,last=0; 

  instru=(char*)malloc(MAX_LINE * sizeof(char));
  last_instru=(char*)malloc(MAX_LINE * sizeof(char));
  in_file=(char*)malloc(MAX_LINE * sizeof(char));
  out_file=(char*)malloc(MAX_LINE * sizeof(char));
  
  clear(last_instru);
  pid_t pid;
  
  while (should_run) {
      printf("osh>");
      fflush(stdout);
      if(concurrency)wait(NULL);
      
      concurrency=0;
      clear(instru);
      fgets(instru,MAX_LINE,stdin);
      
      simplify(instru);
      concurrency=check(instru);
      
      if(strcmp(instru,"exit")==0){
          should_run=0;
          continue;
      }
      if(strcmp(instru,"!!")==0){
          if(!last){
             fprintf(stderr,"No Commands in History\n");
             continue;
          }
          else{
             printf("%s\n", last_instru);
             strcpy(instru, last_instru);
          }
      }
      
        
      pid = fork();
      if(pid<0){
          fprintf(stderr,"Fork Failed \n");
          exit(1);
      }    
      else if(pid==0){
          for(int i=0;i<=MAX_LINE/2;i++){
             args[i]=(char*)malloc(MAX_LINE * sizeof(char));
          }
          int argn=transmit(instru,args);
          for(int i=argn;i<=MAX_LINE/2;i++){
             free(args[i]);
             args[i]=NULL;
          }
          if(concurrency==1){
             argn--;
             free(args[argn]);
             args[argn]=NULL;
          }
          int position=-1;
          for(int i=0;i<argn;i++){
              if(strcmp(args[i], "|") == 0){
                 position=i;
                 break;
              }
          }
          if(position>0){
               if(position==0||position==argn-1){
                    fprintf(stderr, "'|' has a wrong position \n");
                    exit(1);
               }
               int fd[2];
               if (pipe(fd) == -1) {
                 fprintf(stderr,"Pipe failed \n");
                 exit(1);
               }
               pid=fork();
               if(pid < 0){
                  fprintf(stderr, "Fork Failed \n");
                  exit(1);
               }
               else if(pid==0){
                   close(fd[WRITE_END]);
                   for(int i=0;i<=position;i++)free(args[i]);
                   for(int i=position+1;i<argn;i++)args[i-position-1]=args[i];
                   for(int i=argn-position-1;i<argn;i++)args[i]=NULL;
                   argn=argn-position-1;
                   if(dup2(fd[READ_END],STDIN_FILENO)<0){ 
                       fprintf(stderr, "Duplication Failed \n");
                       exit(1);
                   }
                   execvp(args[0],args);
                   close(fd[READ_END]);
                   for(int i=0;i<argn;i++)free(args[i]);
                   free(instru);
                   free(in_file);
                   free(out_file);
               }
               else{
                   close(fd[READ_END]);             
                   for(int i=position;i<argn;i++){
                       free(args[i]);
                       args[i]=NULL;
                   }
                   argn=position;
                   if(dup2(fd[WRITE_END],STDOUT_FILENO)<0){
                       fprintf(stderr, "Duplication Failed \n");
                       exit(1);
                   }   
                   execvp(args[0],args);
                   close(fd[WRITE_END]); 
               }
         }
         else{
               int indirect=0,outdirect=0,infd=-1,outfd=-1;
               if(argn>=3&&(strcmp(args[argn - 2], "<") == 0 || strcmp(args[argn - 2], ">") == 0)){
                   if(strcmp(args[argn-2], "<") == 0 ){
                       indirect=1;
                       strcpy(in_file,args[argn-1]);
                   }
                   else if(strcmp(args[argn-2], ">") == 0){
                       outdirect=1;
                       strcpy(out_file,args[argn-1]);
                   }
                   argn-=2;
                   free(args[argn]);args[argn]=NULL;
                   free(args[argn+1]);args[argn+1]=NULL;
               }           
               if(indirect){
                   infd=open(in_file,O_RDONLY,0644);            
                  if(infd<0){
                      fprintf(stderr, "No such files\n");
                      exit(1);
                  }
                  if(dup2(infd,STDIN_FILENO)<0){
                      fprintf(stderr, "Duplication Failed\n");
                      exit(1);
                  }                
               }
               if(outdirect){
                   outfd=open(out_file,O_WRONLY|O_TRUNC|O_CREAT,0644);            
                   if(outfd<0){
                       fprintf(stderr, "No such files\n");
                       exit(1);
                   }
                   if(dup2(outfd,STDOUT_FILENO)<0){
                       fprintf(stderr, "Duplication Failed\n");
                       exit(1);
                   }    
               }
            execvp(args[0],args);
            if(indirect && infd>0)close(infd);
            if(outdirect && outfd>0)close(outfd);
         }
         for(int i=0;i<argn;i++)free(args[i]);
         free(instru);
         free(last_instru);
         free(in_file);
         free(out_file);
         exit(0);
      }
      else {
          if(concurrency==0){
            wait(NULL);
            printf("Child Process Terminates\n");
          }
      }
      if(!last)last=1;
      strcpy(last_instru,instru);
  }
  free(instru);
  free(in_file);
  free(out_file);
  free(last_instru);
  return 0;
}


void clear(char *string){
   memset(string,0,sizeof(string));
}

int check(char *string){
   int length=strlen(string);
   if(length>0 && string[length-1]=='&')return 1;
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

int transmit(char *instru,char **args){
    int argn=0;
    int length=strlen(instru);
    
    for(int i=0;i<length;i++){
       clear(args[argn]);
       int j=i;
       while(j<length&&instru[j]!=' '){
          args[argn][j-i]=instru[j];  
          j++;
       }
       if((args[argn][0]=='<'||args[argn][0]=='>'||args[argn][0]=='|')&&j>=i+2){
          strcpy(args[argn+1],args[argn]+1);
          for(int k=1;k<j-i;k++)args[argn][k]=0;
          ++argn;
       }
       i=j;
       argn++;    
    }
    
    return argn;
}


