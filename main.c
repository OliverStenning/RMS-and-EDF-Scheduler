#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "task.h"
#include "scheduler.h"

const int MAX_CHAR = 1000;

// Variables for storing tasks
int numTasks = 0;
struct Task *tasks;

// Variables for storing schedule
int numEvents = 0;
int numMisses = 0;
struct ScheduleEvent *scheduleList;

// Reads data from input file and stores them in tasks array
int readFile(char *filename) {
    FILE *fp;
    char str[MAX_CHAR];

    // Opens testcase file from filename provided
    fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Could not open file %s\n", filename);
        return 1;
    }

    // Set index at -1 to detect first iteration
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
            tasks[index].completions = 0;

        }
        index++;
    }

    // Close file
    fclose(fp);
    return 0;
}

// Saves data from schedules array into output file
int saveFile(char *filename) {

    FILE *f = fopen(filename, "w");
    if (f ==NULL) {
        printf("Could not open file %s", filename);
        return 1;
    }

    for (int i = 0; i < numEvents; ++i) {

        // Dont output event if undefined type or idle
        if (scheduleList[i].type != 0 && scheduleList[i].type != 4) {

            // Start line with time and task name
            fprintf(f, "%d Task %d ", scheduleList[i].time, scheduleList[i].name);

            // Add correct event type to end
            switch (scheduleList[i].type) {
                case 1:
                    fprintf(f, "Executes\n");
                    break;
                case 2:
                    fprintf(f, "Completes\n");
                    break;
                case 3:
                    fprintf(f, "Misses\n");
                    break;
                default:
                    fclose(f);
                    printf("Error saving schedule Type: %d Line: %d Events: %d", scheduleList[i].type, i, numEvents);
                    return 2;
            }
        }
    }

    fprintf(f, "%d Deadline Misses", numMisses);

    // Close file
    fclose(f);
    return 0;
}

int main() {

    // Take user input for input file location and read input tasks
    char filename[MAX_CHAR];
    int reading = 1;
    while (reading) {
        printf("Enter input file location:\n");
        scanf("%s", filename);
        if (strcmp(filename, "default") == 0) {
            strcpy(filename, "C:\\Users\\Oliver\\Programming\\CLion\\Scheduler\\testcases\\testcase1.txt");
        }
        reading = readFile(filename);
    }

    // Schedule tasks using RMS
    scheduleList = schedule(&numEvents, &numMisses, tasks, numTasks, 0);

    // Take user input for RMS output file location and save schedule
    reading = 1;
    while (reading) {
        printf("Enter RMS output file location:\n");
        scanf("%s", filename);
        if (strcmp(filename, "default") == 0) {
            strcpy(filename, "C:\\Users\\Oliver\\Programming\\CLion\\Scheduler\\testcases\\output-rms.txt");
        }
        reading = saveFile(filename);
    }

    // Free memory used for storing schedule
    free(scheduleList);

    // Reset tasks to default state
    resetTasks(tasks, numTasks);

    // Schedule tasks using EDF
    scheduleList = schedule(&numEvents, &numMisses, tasks, numTasks, 1);

    // Take user input for EDF output file location and save schedule
    reading = 1;
    while (reading) {
        printf("Enter RMS output file location:\n");
        scanf("%s", filename);
        if (strcmp(filename, "default") == 0) {
            strcpy(filename, "C:\\Users\\Oliver\\Programming\\CLion\\Scheduler\\testcases\\output-edf.txt");
        }
        reading = saveFile(filename);
    }



    // Free memory used for storing schedule and tasks
    free(scheduleList);
    free(tasks);

    return 0;
}