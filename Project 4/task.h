/**
 * Representation of a task in the system.
 */

#ifndef TASK_H
#define TASK_H
extern int tid_value;

// representation of a task
typedef struct task {
    char *name;
    int tid;
    int priority;
    int burst;
    
    int last_exec;
    int wait;
    int res;
    int turn;
} Task;

#endif
