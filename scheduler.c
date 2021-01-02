#include "scheduler.h"
#include <stdio.h>
#include <math.h>

void printSchedule(struct ScheduleEvent* schedule, int n) {

    printf("\nSCHEDULE\n");

    for (int i = 0; i < n; ++i) {

        // Start line with time
        printf("Time: %2d | ", i);

        // Finish line if idle
        if (schedule[i].type == 3) {
            printf("Idle\n");
        } else {

            // If not idle add task name
            printf("Task %d ", schedule[i].name);

            // Add operation and finish line
            switch (schedule[i].type) {
                case 0:
                    printf("Executes\n");
                    break;
                case 1:
                    printf("Completes\n");
                    break;
                case 2:
                    printf("Misses\n");
                    break;
                default:
                    printf("\nError\n");
                    break;
            }
        }
    }
}

int schedulabilityCheck(struct Task* tasks, int n) {

    // Sum of all (duration / period)
    float sum = 0;
    for (int i = 0; i < n; ++i) {
        sum += ((float) tasks[i].duration / (float) tasks[i].period);
    }

    // Max value for RMS
    double max = n * (exp(log(2) / n) - 1);

    // Possible combinations of results and schedulability
    if (sum <= max && sum <= 1) {
        return 0;
    } else if (sum <= max && sum > 1) {
        return 1;
    } else if (sum > max && sum <= 1) {
        return 2;
    } else {
        return 3;
    }

}

void prioritiseRMS(struct Task* tasks, int n) {

    // Longest possible period of a given task is the super period
    int maxPeriod = superPeriod(tasks, n);

    for (int i = 0; i < n; ++i) {
        int smallest = maxPeriod;
        int smallestPos = 0;

        for (int j = 0; j < n; ++j) {

            // If priority has not been set and period is smaller than currently smallest period found
            if (tasks[j].priority == -1 && tasks[j].period <= smallest) {
                smallest = tasks[j].period;
                smallestPos = j;
            }
        }

        // Set priority of smallest period unassigned task to iteration of loop
        tasks[smallestPos].priority = i;
    }

}

struct ScheduleEvent* scheduleRMS(struct Task* tasks, int n, int superPeriod) {

    // Allocate minimum amount of memory used
    struct ScheduleEvent* schedule = malloc(sizeof(struct ScheduleEvent) * superPeriod);

    for (int i = 0; i < superPeriod; ++i) {

        // Reset task progress when period is reached
        for (int j = 0; j < n; ++j) {
            if (i % tasks[j].period == 0 && i != 0) {
                tasks[j].progress = 0;
            }
        }

        // Initialise at impossible to reach value
        int nextTask = -1;

        // Search tasks for first uncompleted task and update value
        for (int j = 0; j < n; ++j) {
            if (tasks[j].progress < tasks[j].duration) {
                nextTask = j;
                break;
            }
        }

        // If no uncompleted tasks are found then idle at this time
        if (nextTask == -1) {
            schedule[i].name = 0; // Default value
            schedule[i].type = 3; // Idle type
            schedule[i].time = i; // Default value

            // End this iteration of time loop early
            continue;
        }

        // Set next task to highest priority uncompleted task
        for (int j = 0; j < n; ++j) {
            if (tasks[j].progress < tasks[j].duration && tasks[j].priority < tasks[nextTask].priority) {
                nextTask = j;
            }
        }

        // Add new execution event to schedule
        schedule[i].time = i;
        schedule[i].name = tasks[nextTask].name;
        schedule[i].type = 0; // Executes type

        // Increment the progress of the task
        tasks[nextTask].progress += 1;

    }

    return schedule;
}