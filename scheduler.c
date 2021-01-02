#include "scheduler.h"
#include <stdio.h>
#include <math.h>

void printSchedule(struct ScheduleEvent *schedule, int n) {

    printf("\nSCHEDULE\n");

    for (int i = 0; i < n - 1; ++i) {

        // Only print if the schedules
        if (schedule[i].type != 0) {
            // Start line with time
            printf("%d ", schedule[i].time);

            // Finish line if idle
            if (schedule[i].type == 4) {
                printf("Idle\n");
            } else {

                // If not idle add task name
                printf("Task %d ", schedule[i].name);

                // Add operation and finish line
                switch (schedule[i].type) {
                    case 1:
                        printf("Executes\n");
                        break;
                    case 2:
                        printf("Completes\n");
                        break;
                    case 3:
                        printf("Misses\n");
                        break;
                    default:
                        printf("\nError\n");
                        break;
                }
            }
        }
    }
}

int schedulabilityCheck(struct Task *tasks, int n) {

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

void prioritiseRMS(struct Task *tasks, int n) {

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

struct ScheduleEvent* scheduleRMS(int *numEvents, struct Task *tasks, int n) {

    // Calculate number of events without misses
    int sPeriod = superPeriod(tasks, n);
    *numEvents = sPeriod;
    for (int i = 0; i < n; ++i) {
        // Add number of complete periods in super period per task as maximum completions
        *numEvents += sPeriod / tasks[i].period;
    }

    // Allocate minimum amount of memory used
    struct ScheduleEvent *schedule = malloc(sizeof(struct ScheduleEvent) * (*numEvents));

    int eventPos = 0;

    for (int time = 0; time < sPeriod; ++time) {

        // Reset task progress when period is reached and task is completed
        for (int j = 0; j < n; ++j) {
            if (time % tasks[j].period == 0 && time != 0 && tasks[j].progress == tasks[j].duration) {
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
            schedule[eventPos].name = 0; // Default value
            schedule[eventPos].type = 4; // Idle type
            schedule[eventPos].time = time; // Default value
            schedule[eventPos].completions = 0; // Default value
            eventPos++;

            // End this iteration of time loop early
            continue;
        }

        // Check to see if any tasks have missed their deadlines
        for (int j = 0; j < n; ++j) {
            if (tasks[j].completions < floor(((double) time) / ((double) tasks[j].period))) {
                (*numEvents)++;
                schedule = realloc(schedule, sizeof(struct ScheduleEvent) * (*numEvents));
                schedule[eventPos].time = time;
                schedule[eventPos].name = tasks[j].name;
                schedule[eventPos].type = 3; // Misses type
                schedule[eventPos].completions = tasks[j].completions;
                eventPos++;
            }
        }

        // Set next task to highest priority uncompleted task
        for (int j = 0; j < n; ++j) {
            if (tasks[j].progress < tasks[j].duration && tasks[j].priority < tasks[nextTask].priority) {
                nextTask = j;
            }
        }

        // Add new execution event to schedule
        schedule[eventPos].time = time;
        schedule[eventPos].name = tasks[nextTask].name;
        schedule[eventPos].type = 1; // Executes type
        schedule[eventPos].completions = tasks[nextTask].completions;
        eventPos++;

        // Increment the progress of the task
        tasks[nextTask].progress++;

        // Add event if task completes
        if (tasks[nextTask].progress >= tasks[nextTask].duration) {

            // Check to see if task was behind schedule
            if (tasks[nextTask].completions < floor(((double) time) / ((double) tasks[nextTask].period))) {
                tasks[nextTask].progress = 0;
            }

            tasks[nextTask].completions++;

            schedule[eventPos].time = time;
            schedule[eventPos].name = tasks[nextTask].name;
            schedule[eventPos].type = 2; // Completes type
            schedule[eventPos].completions = tasks[nextTask].completions;
            eventPos++;
        }

    }
    return schedule;
}