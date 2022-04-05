import java.util.concurrent.*;
import java.util.Arrays;
import java.util.Scanner;

public class mergesort extends RecursiveAction{
        static final int THRESHOLD=10;

        private int begin;
        private int end;
        private Integer[] array;

        public mergesort(int begin,int end,Integer[] array){
            this.begin = begin;
            this.end = end;
            this.array = array;     
        }

        protected void compute(){
            if(end-begin<THRESHOLD){
                 int size=end-begin+1;
                 for(int i=1;i<=size-1;i++)
                     for(int j=begin;j<=end-i;j++){
                          if(array[j].compareTo(array[j+1])==1){
                                int temp=array[j];
                                array[j]=array[j+1];
                                array[j+1]=temp;
                          }
                     }
            }
            else{
                 int mid=(begin+end)/2;
                 int size=end-begin+1;
                 mergesort left=new mergesort(begin,mid,array);
                 mergesort right=new mergesort(mid+1,end,array);
                 
                 left.fork();
                 right.fork();
                 
                 left.join();
                 right.join();
                 
                 Integer []temp=new Integer[size];
                 int i=0,j=mid+1,t=0;
                 while(i<=mid&&j<=end){
                     if(array[i].compareTo(array[j])==-1)
                           temp[t++]=array[i++];
                     else temp[t++]=array[j++];
                 }
                 while(i<=mid)temp[t++]=array[i++];
                 while(j<=end)temp[t++]=array[j++];
                 
                 for(int k=0;k<size;k++)array[k+begin]=temp[k];
            }        
        }
        
        public static void main(String[] args){
                 Scanner s=new Scanner(System.in);
                 ForkJoinPool pool = new ForkJoinPool();
        
                 System.out.print("Please insert the length of the array\n");
                 int length=s.nextInt();
                 if(length<0){
                       System.out.print("Illegal input\n");
                       System.exit(1);
                 }
                 
                 Integer []array=new Integer[length];
                 
                 System.out.print("Please input the elements of the array\n");
                 for(int i=0;i<length;i++)
                      array[i]=s.nextInt();
                 
                 mergesort task=new mergesort(0,length-1,array);
                 pool.invoke(task);
                 
                 System.out.print("The array after sorting:\n");
                 for(int a:array){
                      System.out.print(a);
                      System.out.print(" ");       
                 }
                 System.out.println("");
        }
       
}

