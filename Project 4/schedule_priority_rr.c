# include <stdio.h>
# include <stdlib.h>
# include <string.h>

# include "task.h"
# include "list.h"
# include "cpu.h"
# include "schedulers.h"

# define MAX_NUMBER 10

struct node *head[MAX_NUMBER] = {}; 

int time = 0;

int count = 0,total_wait=0,total_res=0,total_turn=0;

void add(char *name, int priority, int burst) {
	Task *task;
	task = (Task *) malloc (sizeof(Task));
	
	task -> tid = __sync_fetch_and_add(&tid_value, 1);
	task -> name = (char *) malloc (sizeof(char) * strlen(name));
	strcpy(task -> name, name);
	task -> priority = priority;
	task -> burst = burst;
	
	task -> last_exec = 0;
	task -> wait = 0;
	task -> res = 0;
	task -> turn = 0;
	insert(&head[priority - MIN_PRIORITY], task);
}

void schedule() {
    struct node *current = NULL;
    for(int i=MAX_NUMBER-1;i>=0;i--){
	while (head[i]){
	current = head[i];
	while (current->next)current = current -> next;
	Task *task = current -> task;
	
	    if(task -> burst <= QUANTUM){
	       run(task, task -> burst);
	       delete(&head[i], task);
	       time += task -> burst;

	       int last_wait_time = time - task -> last_exec - task -> burst;	
	       task -> wait += last_wait_time;	
	       if (task -> last_exec == 0)
	          task -> res = time - task -> burst;	
	       task -> last_exec = time;
	       task -> turn = time;
	
               count += 1;
               total_wait += task -> wait;
	       total_res += task -> res;
      	       total_turn += task -> turn;

	       free(task -> name);
	       free(task);	
	    }
	    else{
	       run(task, QUANTUM);
	       delete(&head[i], task);
	       time += QUANTUM;
	       task -> burst -= QUANTUM;
	   
	       int last_wait_time = time - task -> last_exec - QUANTUM;	
	       task -> wait += last_wait_time;	
	       if (task -> last_exec == 0)
	            task -> res = time - QUANTUM;	
	       task -> last_exec = time;
	       task -> turn = time;
	   
	       insert(&head[i], task);	
	    }
	}
    }	
	printf("\n");
	printf("There are %d tasks\n", count);
	double avg_wait = 1.0 * total_wait / count;
	double avg_res = 1.0 * total_res / count;
	double avg_turn = 1.0 * total_turn / count;
	printf("Average Waiting Time: %f\n", avg_wait);
	printf("Average Response Time: %f\n", avg_res);
	printf("Average Turnaround Time: %f\n", avg_turn);	
}




