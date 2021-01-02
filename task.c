#include "task.h"
#include <stdio.h>
#include "util.h"

void printTasks(struct Task* tasks, int n) {

    printf("\nTASKS\n");

    for (int i = 0; i < n; ++i) {
        printf("Task %d - Execution time: %d Period: %d Priority: %d Progress: %d\n",
               tasks[i].name, tasks[i].duration, tasks[i].period, tasks[i].priority, tasks[i].progress);
    }
}

int superPeriod(struct Task* tasks, int n) {
    int periods[n];

    for (int i = 0; i < n; ++i) {
        periods[i] = tasks[i].period;
    }

    return lcm(periods, n);
}
