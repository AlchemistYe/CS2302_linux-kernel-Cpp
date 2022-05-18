# include <stdio.h>
# include <string.h>

int main(){
   FILE *output=fopen("output.txt","r");
   FILE *correct=fopen("correct.txt","r");

   int flag=1;
   int cor_value, value, line=0;
   while(~fscanf(correct, "Virtual address: %*d Physical address: %*d Value: %d\n", &cor_value)){
      line++;
      if(fscanf(output, "Virtual address: %*d Physical address: %*d Value: %d\n", &value) == EOF){
         printf("The number of lines mismatched\n");
         break;
      }
      if(cor_value!=value){
         printf("Error on line %d\n",++line);
         flag=0;
      }
   }

   if(flag==1)printf("The answer is correct\n");

   fclose(output);
   fclose(correct);
   return 0;
}
