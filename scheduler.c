#include "scheduler.h"
#include <stdio.h>
#include <math.h>

void printSchedule(struct ScheduleEvent *schedule, int n, int m) {

    printf("\nSCHEDULE\n");

    for (int i = 0; i < n - 1; ++i) {

        // Only print if the schedules are defined
        if (schedule[i].type != Undefined) {
            // Start line with time
            printf("%d ", schedule[i].time);

            // Finish line if idle
            if (schedule[i].type == Idle) {
                printf("Idle\n");
            } else {

                // If not idle add task name
                printf("Task %d ", schedule[i].name);

                // Add operation and finish line
                switch (schedule[i].type) {
                    case Executes:
                        printf("Executes\n");
                        break;
                    case Completes:
                        printf("Completes\n");
                        break;
                    case Misses:
                        printf("Misses\n");
                        break;
                    default:
                        printf("\nError\n");
                        break;
                }
            }
        }
    }

    printf("%d Deadline Misses\n", m);
}

void printSchedulabilityCheck(struct Task *tasks, int n) {
    switch (schedulabilityCheck(tasks, n)) {
        case 0:
            printf("Can be scheduled with RMS and EDF\n");
            break;
        case 1:
            printf("Can be scheduled with RMS only\n");
            break;
        case 2:
            printf("Can be scheduled with EDF only\n");
            break;
        default:
            printf("Cannot be scheduled\n");
            break;
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

    // Set all priorities to unassigned
    for (int i = 0; i < n; ++i) {
        tasks[i].priority = -1;
    }

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

void prioritiseEDF(struct Task *tasks, int n) {

    // Set all priorities to unassigned
    for (int i = 0; i < n; ++i) {
        tasks[i].priority = -1;
    }

    // Longest possible deadline of a given task is the super period
    int maxPeriod = superPeriod(tasks, n);

    for (int i = 0; i < n; ++i) {
        int smallest = maxPeriod;
        int smallestPos = 0;

        for (int j = 0; j < n; ++j) {

            // If priority has not been set and period is smaller than currently smallest period found
            if (tasks[j].priority == -1 && (tasks[j].period * (tasks[j].completions + 1)) <= smallest) {
                smallest = tasks[j].period * (tasks[j].completions + 1);
                smallestPos = j;
            }
        }

        // Set priority of smallest period unassigned task to iteration of loop
        tasks[smallestPos].priority = i;
    }
}

struct ScheduleEvent* schedule(int *numEvents, int *numMisses, struct Task *tasks, int n, int isEDF) {

    // Prioritise tasks using RMS if EDF not selected
    if (!isEDF) {
        prioritiseRMS(tasks, n);
    }

    // Reset number of misses if previously scheduled
    (*numMisses) = 0;

    // Get super period of tasks
    int sPeriod = superPeriod(tasks, n);

    // Allocate minimum amount of memory used for execute/idle events
    // Complete and miss event quantity is unknown before scheduling
    *numEvents = sPeriod;
    struct ScheduleEvent *schedule = malloc(sizeof(struct ScheduleEvent) * (*numEvents));

    // Position of last event in schedule array
    int eventPos = 0;

    // Loop through all time values in super period
    for (int time = 0; time < sPeriod; ++time) {

        // Reprioritise tasks every iteration depending on nearest deadlines if EDF
        if (isEDF) {
            prioritiseEDF(tasks, n);
        }

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
            schedule[eventPos].type = Idle;
            schedule[eventPos].time = time; // Current time
            schedule[eventPos].completions = 0; // Default value
            eventPos++;

            // End this iteration of time loop early
            continue;
        }

        // Check to see if any tasks have missed their deadlines
        for (int j = 0; j < n; ++j) {
            if (tasks[j].completions < floor(((double) time) / ((double) tasks[j].period))) {

                // Allocate more space for miss event
                (*numEvents)++;
                schedule = realloc(schedule, sizeof(struct ScheduleEvent) * (*numEvents));

                // Add miss event to schedule
                schedule[eventPos].time = time;
                schedule[eventPos].name = tasks[j].name;
                schedule[eventPos].type = Misses;
                schedule[eventPos].completions = tasks[j].completions;
                (*numMisses)++;
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
        schedule[eventPos].type = Executes;
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

            // Allocate more space for completion event
            (*numEvents)++;
            schedule = realloc(schedule, sizeof(struct ScheduleEvent) * (*numEvents));

            // Increment completions counter
            tasks[nextTask].completions++;

            // Add completion event to schedule
            schedule[eventPos].time = time;
            schedule[eventPos].name = tasks[nextTask].name;
            schedule[eventPos].type = Completes;
            schedule[eventPos].completions = tasks[nextTask].completions;
            eventPos++;
        }

    }
    return schedule;
}