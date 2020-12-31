#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

const int MAX_CHAR = 1000;

struct Task {
    char name[7];
    int executionTime;
    int period;
};

// Variables for storing tasks
int numTasks = 0;
struct Task *tasks;

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

    int index = 0;
    while(fgets(str, MAX_CHAR, fp) != NULL) {

        if (index == 0) {

            // Reads number of tasks from first line
            char *token = strtok(str, " ");
            token = strtok(NULL, " ");

            numTasks = strtol(token, NULL, 10);
            tasks = malloc(sizeof(struct Task) * numTasks);

        } else if (index > 0) {

            // Reads each subsequent line and splits data into structs
            char *token = strtok(str, " ");
            strcpy(tasks[index - 1].name, token);

            token = strtok(NULL, " ");
            tasks[index - 1].executionTime = strtol(token, NULL, 10);

            token = strtok(NULL, " ");
            tasks[index - 1].period = strtol(token, NULL, 10);
        }
        index++;
    }
    fclose(fp);
    return 0;
}

// Print list of tasks that have been read in
void printTasks() {
    for (int i = 0; i < numTasks; ++i) {
        printf("\n%s\nExecution time: %d Period: %d\n", tasks[i].name, tasks[i].executionTime, tasks[i].period);
    }
}

int main() {

    readFile("C:\\Users\\Oliver\\Programming\\CLion\\Scheduler\\testcases\\testcase4.txt");
    printTasks();

    return 0;
}


