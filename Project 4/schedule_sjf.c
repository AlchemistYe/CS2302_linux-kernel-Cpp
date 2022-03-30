# include <stdio.h>
# include <stdlib.h>
# include <string.h>

# include "task.h"
# include "list.h"
# include "cpu.h"
# include "schedulers.h"

struct node *head = NULL; 


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
	insert(&head, task);
}

void schedule() {
  	struct node *current = NULL;
  	struct node *shortjob = NULL;
	while (head){
	current = head -> next;
	shortjob = head;
	while (current){
	     if(shortjob-> task -> burst > current -> task -> burst)shortjob = current;
	     current = current -> next;
	}
	Task *task = shortjob -> task;
	run(task, task -> burst);
	delete(&head, task);
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
	
	printf("\n");
	printf("There are %d tasks\n", count);
	double avg_wait = 1.0 * total_wait / count;
	double avg_res = 1.0 * total_res / count;
	double avg_turn = 1.0 * total_turn / count;
	printf("Average Waiting Time: %f\n", avg_wait);
	printf("Average Response Time: %f\n", avg_res);
	printf("Average Turnaround Time: %f\n", avg_turn);	
}




