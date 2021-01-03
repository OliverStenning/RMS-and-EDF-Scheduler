#ifndef SCHEDULER_SCHEDULER_H
#define SCHEDULER_SCHEDULER_H

#include "task.h"
#include <stdlib.h>


/* Holds all data of each event in schedule */
struct ScheduleEvent {
    int time;
    int name;
    int type; // 0 = Undefined | 1 = Executes | 2 = Completes | 3 = Misses | 4 = Idle
    int completions;
};


/* Prints value of all schedule events to console */
void printSchedule(struct ScheduleEvent *schedule, int n);


/* Checks to see if its possible to schedule tasks without missing deadlines
 * Output:
 * 0 = possible with RMS and EDF
 * 1 = possible with RMS only
 * 2 = possible with EDF only
 * 3 = possible with both
 * */
int schedulabilityCheck(struct Task *tasks, int n);


/* Prioritises all tasks in order of smallest period
 * Lower priority value happens first
 * */
void prioritiseRMS(struct Task *tasks, int n);


/* Generates a schedule using RMS */
struct ScheduleEvent* scheduleRMS(int *numEvents, struct Task *tasks, int n);


/* Prioritises all tasks in order of smallest deadline */
void prioritiseEDF(struct Task *tasks, int n);


/* Generates a schedule using EDF */
struct ScheduleEvent* scheduleEDF(int *numEvents, struct Task *tasks, int n);

#endif //SCHEDULER_SCHEDULER_H