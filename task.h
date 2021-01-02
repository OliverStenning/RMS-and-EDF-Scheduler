#ifndef SCHEDULER_TASK_H
#define SCHEDULER_TASK_H

/* Holds all data of each task */
struct Task {
    int name;
    int duration;
    int period;
    int priority;
    int progress;
    int completions;
};

/* Prints value of all tasks to console */
void printTasks(struct Task *tasks, int n);

/* Calculates the super period of current tasks */
int superPeriod(struct Task *tasks, int n);

#endif //SCHEDULER_TASK_H
