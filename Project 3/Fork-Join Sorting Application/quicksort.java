import java.util.concurrent.*;
import java.util.Arrays;
import java.util.Scanner;

public class quicksort extends RecursiveAction{
        static final int THRESHOLD=10;

        private int begin;
        private int end;
        private Integer[] array;

        public quicksort (int begin,int end,Integer[] array){
            this.begin = begin;
            this.end = end;
            this.array = array;     
        }

        protected void compute(){
            if(end-begin<THRESHOLD){
                 int size=end-begin+1;
                 for(int i=1;i<=size-1;i++)
                     for(int j=begin;j<=end-i;j++)
                         if(array[j].compareTo(array[j+1])==1){
                                int temp=array[j];
                                array[j]=array[j+1];
                                array[j+1]=temp;
                         }
            }
            else{
                 int pivot=array[end];
                 int mid=begin;
                 for(int j=mid;j<=end-1;j++){
                       if(array[j].compareTo(pivot)==-1){
                          int temp=array[j];
                          array[j]=array[mid];
                          array[mid]=temp;
                          mid++;
                       }
                 }
                 array[end]=array[mid];
                 array[mid]=pivot;
                    
                 quicksort left=new quicksort(begin,mid-1,array);
                 quicksort right=new quicksort(mid+1,end,array);
                 
                 left.fork();
                 right.fork();
                 
                 left.join();
                 right.join();
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
                 
                 Integer []array = new Integer[length];
                 
                 System.out.print("Please input the elements of the array\n");
                 for(int i=0;i<length;i++)
                      array[i]=s.nextInt();
                 
                 quicksort task=new quicksort(0,length-1,array);
                 pool.invoke(task);
                 
                 System.out.print("The array after sorting:\n");
                 for(int a:array){
                      System.out.print(a);
                      System.out.print(" ");       
                 }
                 System.out.println("");
        }
       
}

