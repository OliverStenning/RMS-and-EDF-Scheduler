import matplotlib.pyplot as pyplot
import numpy as np

# Constants
TASK_Y_HEIGHT = 6
TASK_Y_SPACING = 10

# Calculates the middle point of bars for input task number
def calculateTaskMid(number):
    return (number * TASK_Y_SPACING) + ((number - 1) * TASK_Y_HEIGHT) + (TASK_Y_HEIGHT / 2)

# Calculates the bottom point of bars for the input task number
def calculateTaskBot(number):
    return (number * TASK_Y_SPACING) + ((number - 1) * TASK_Y_HEIGHT)

# Plots the schedule between start and end times of the input file to the output file
def plotSchedule(fileInput, fileOutput, startTime, endTime, type):

    # Reads file into events list
    with open(fileInput, "rt") as file:
        events = file.readlines()

    # Removes dead space from events list
    events = [x.strip() for x in events]

    # Gets the number of tasks from the first line of output
    numTasks = events[0].split()
    numTasks = int(numTasks[0])

    # Removes first and last two lines which are not events
    del events[0]
    del events[(len(events) - 1)]
    del events[(len(events) - 1)]

    startPos = 0
    endPos = 0

    # Last second shown begins one second before end
    endTime -= 1

    # Finds what position in events list the start time happens
    for x in events:
        event = x.split()
        if (int(event[0]) == startTime):
            startPos = events.index(x)
            break

    # Finds what position in events list the last event at the end time happens
    for x in reversed(events):
        event = x.split()
        if (int(event[0]) == endTime):
            endPos = events.index(x)
            break

    # If the end time wasn't found in the events list then set the final position to the end of the list
    if endPos == 0:
        endPos = len(events) - 1
        endTime = events[endPos].split()
        endTime = int(endTime[0])

    # Remove events that occur outside of the start and end time
    del events[endPos:(len(events) - 1)]
    if startPos != 0:
        del events[0:startPos]

    # Create a figure object for creating the plot
    fig, gantt = pyplot.subplots(figsize=(10, numTasks * 1.5))

    # Setup axis limits and labels
    gantt.set_xlim(startTime, endTime + 1)
    gantt.set_ylim(0, ((TASK_Y_HEIGHT * numTasks) + (TASK_Y_SPACING * (numTasks + 1))))
    gantt.set_xlabel("Seconds")
    gantt.set_ylabel("Task")

    # Setup tick values in x axis (Can be removed for automatic tick values)
    #gantt.set_xticks(range(endTime + 1))

    # Calculate position of ticks on y axis to line up with center of task and create labels for each task
    yticks = [None] * numTasks
    yticklabels = [None] * numTasks

    for x in range(numTasks):
        yticks[x] = calculateTaskMid(x + 1)
        yticklabels[x] = str((x + 1))

    # Setup tick values and labels in y axis
    gantt.set_yticks(yticks)
    gantt.set_yticklabels(yticklabels)

    # Set grid to only show vertical lines
    gantt.grid(axis = 'x')

    # Iterate through events and plot event depending on type and time on the figure
    for x in events:
        event = x.split()
        # Check which task event belongs to
        for y in range(1, numTasks + 1):
            if (int(event[2]) == y):
                # Check what event type it is
                if "Executes" in x:
                    gantt.broken_barh([(int(event[0]), 1)], (calculateTaskBot(y), (TASK_Y_HEIGHT)), facecolors ='deepskyblue')
                elif "Completes" in x:
                    pyplot.plot((int(event[0]) + 1), calculateTaskMid(y), marker = 'o', color = 'lime')
                elif "Misses" in x:
                    pyplot.plot(int(event[0]), calculateTaskMid(y), marker = 'x', color = 'red')
                break


    # Reduce amount of white space in plot output images
    pyplot.tight_layout()

    # Show preview of plot (Can be disabled to automatically generate output)
    #pyplot.show()

    # Save plot to specified output file
    pyplot.savefig(fileOutput)

    # Print to console once plot has been generated
    print("Successfully saved " + type + " plot.")

# Call function to generate schedule for EDF
plotSchedule("testcases\output-edf-2.txt", "testcases\output-edf-plot-2.png", 0, 250, "EDF")

# Call function to generate schedule for RMS
plotSchedule("testcases\output-rms-2.txt", "testcases\output-rms-plot-2.png", 0, 250, "RMS")
