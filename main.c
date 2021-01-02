#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>

#include "util.h"
#include "task.h"
#include "scheduler.h"

const int MAX_CHAR = 1000;

// Variables for storing tasks
int numTasks = 0;
struct Task *tasks;

// Variables for storing schedule
int numScheduleEvents = 0;
struct ScheduleEvents *schedule;

// Reads data from input file and stores them in tasks array
int readFile(char* filename) {
    FILE *fp;
    char str[MAX_CHAR];

    // Opens testcase file from filename provided
    fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Could not open file %s", filename);
        return 1;
    }

    int index = -1;
    while(fgets(str, MAX_CHAR, fp) != NULL) {

        if (index == -1) {


            // Reads number of tasks from first line
            strtok(str, " ");
            char *token;
            token = strtok(NULL, " ");

            numTasks = strtol(token, NULL, 10);
            tasks = malloc(sizeof(struct Task) * numTasks);

        } else if (index >= 0) {

            // Reads each subsequent line and splits data into structs

            // Splits string after "Task" to retrieve task number
            strtok(str, "k");
            char *token = strtok(NULL, " ");
            tasks[index].name = strtol(token, NULL, 10);

            token = strtok(NULL, " ");
            tasks[index].duration = strtol(token, NULL, 10);

            token = strtok(NULL, " ");
            tasks[index].period = strtol(token, NULL, 10);

            // Give a placeholder priority and progress
            tasks[index].priority = - 1;
            tasks[index].progress = 0;

        }
        index++;
    }
    fclose(fp);
    return 0;
}

// Get next deadline of task at a certain time
int nextDeadline(int time, int n) {

    int completions;
    completions = (int) floor((double) time / (double) tasks[n].period);
    return tasks[n].period * (completions + 1);
}

int main() {

    readFile("C:\\Users\\Oliver\\Programming\\CLion\\Scheduler\\testcases\\testcase1.txt");

    switch (schedulabilityCheck(tasks, numTasks)) {
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
    };

    numScheduleEvents = superPeriod(tasks, numTasks);

    prioritiseRMS(tasks, numTasks);

    printTasks(tasks, numTasks);

    schedule = scheduleRMS(tasks, numTasks, numScheduleEvents);
    printSchedule(schedule, numScheduleEvents);


    return 0;
}


