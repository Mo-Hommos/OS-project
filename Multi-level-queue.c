#include <stdio.h>
#include <stdlib.h>

#define MAX_PROCESSES 50
#define QUANTUM1 8   // Time quantum for the first Round Robin queue
#define QUANTUM2 16  // Time quantum for the second Round Robin queue

typedef struct {
    int pid;
    int arrivalTime;
    int burstTime;
    int waitingTime;
    int turnaroundTime;
    int remainingTime;
    int completionTime;
} Process;

// Function to calculate average times
void calculateAverageTimes(Process processes[], int n, float *avgWaitingTime, float *avgTurnaroundTime) {
    int totalWaitingTime = 0, totalTurnaroundTime = 0;
    for (int i = 0; i < n; i++) {
        totalWaitingTime += processes[i].waitingTime;
        totalTurnaroundTime += processes[i].turnaroundTime;
    }
    *avgWaitingTime = (float)totalWaitingTime / n;
    *avgTurnaroundTime = (float)totalTurnaroundTime / n;
}

// Function to sort processes by arrival time
void sortByArrival(Process processes[], int n) {
    for(int i=0; i<n-1; i++) {
        for(int j=0; j<n-i-1; j++) {
            if(processes[j].arrivalTime > processes[j+1].arrivalTime) {
                Process temp = processes[j];
                processes[j] = processes[j+1];
                processes[j+1] = temp;
            }
        }
    }
}

// Round Robin Scheduling Function
void roundRobin(Process processes[], int n, int quantum, int *time, int queueLevel, Process rrQueue[], int *rrCount) {
    for(int i=0; i<n; i++) {
        if(queueLevel == 1) { // RR1
            if(processes[i].arrivalTime <= *time && processes[i].remainingTime > 0) {
                int execTime = (processes[i].remainingTime > quantum) ? quantum : processes[i].remainingTime;
                // If current time is less than arrival, advance time
                if(*time < processes[i].arrivalTime) {
                    *time = processes[i].arrivalTime;
                }
                // Execute the process
                if(processes[i].remainingTime == processes[i].burstTime) {
                    processes[i].waitingTime = *time - processes[i].arrivalTime;
                }
                *time += execTime;
                processes[i].remainingTime -= execTime;
                if(processes[i].remainingTime > 0) {
                    rrQueue[*rrCount] = processes[i];
                    (*rrCount)++;
                }
                else {
                    processes[i].completionTime = *time;
                }
            }
        }
        else if(queueLevel == 2) { // RR2
            if(processes[i].arrivalTime <= *time && processes[i].remainingTime > 0) {
                int execTime = (processes[i].remainingTime > quantum) ? quantum : processes[i].remainingTime;
                if(*time < processes[i].arrivalTime) {
                    *time = processes[i].arrivalTime;
                }
                if(processes[i].waitingTime == 0 && processes[i].completionTime == 0) {
                    processes[i].waitingTime = *time - processes[i].arrivalTime - (processes[i].burstTime - processes[i].remainingTime);
                }
                *time += execTime;
                processes[i].remainingTime -= execTime;
                if(processes[i].remainingTime > 0) {
                    // Move to FCFS queue
                    // FCFS will be handled separately
                }
                else {
                    processes[i].completionTime = *time;
                }
            }
        }
    }
}

// FCFS Scheduling Function
void fcfs(Process processes[], int n, int *time) {
    for(int i=0; i<n; i++) {
        if(processes[i].remainingTime > 0) {
            if(*time < processes[i].arrivalTime) {
                *time = processes[i].arrivalTime;
            }
            processes[i].waitingTime = *time - processes[i].arrivalTime - (processes[i].burstTime - processes[i].remainingTime);
            *time += processes[i].remainingTime;
            processes[i].remainingTime = 0;
            processes[i].completionTime = *time;
            processes[i].turnaroundTime = processes[i].completionTime - processes[i].arrivalTime;
            processes[i].waitingTime = processes[i].turnaroundTime - processes[i].burstTime;
        }
    }
}

// Multilevel Queue Scheduling Function
void multilevelQueueScheduling(Process processes[], int n) {
    int time = 0;
    int completed = 0;

    // Sort processes by arrival time
    sortByArrival(processes, n);

    // RR1 Queue
    Process rr1Queue[MAX_PROCESSES];
    int rr1Count = 0;
    printf("\nProcessing First Round Robin Queue (Quantum = %d)...\n", QUANTUM1);
    roundRobin(processes, n, QUANTUM1, &time, 1, rr1Queue, &rr1Count);

    // RR2 Queue
    Process rr2Queue[MAX_PROCESSES];
    int rr2Count = 0;
    printf("\nProcessing Second Round Robin Queue (Quantum = %d)...\n", QUANTUM2);
    // Populate RR2 queue from RR1 remaining processes
    for(int i=0; i<rr1Count; i++) {
        rr2Queue[i] = rr1Queue[i];
    }
    rr2Count = rr1Count;
    rr1Count = 0; // Reset RR1 count
    // Execute RR2
    roundRobin(processes, n, QUANTUM2, &time, 2, rr2Queue, &rr2Count);

    // FCFS Queue
    printf("\nProcessing FCFS Queue...\n");
    fcfs(processes, n, &time);
}

// Function to print the results
void printResults(Process processes[], int n) {
    float avgWaitingTime, avgTurnaroundTime;
    calculateAverageTimes(processes, n, &avgWaitingTime, &avgTurnaroundTime);

    printf("\nPID\tArrival\tBurst\tWaiting\tTurnaround\n");
    for(int i=0; i<n; i++) {
        printf("%d\t%d\t%d\t%d\t%d\n",
               processes[i].pid,
               processes[i].arrivalTime,
               processes[i].burstTime,
               processes[i].waitingTime,
               processes[i].turnaroundTime);
    }

    printf("\nAverage Waiting Time: %.2f\n", avgWaitingTime);
    printf("Average Turnaround Time: %.2f\n", avgTurnaroundTime);
}

// Main Function
int main() {
    int n;
    Process processes[MAX_PROCESSES];

    printf("Enter the number of processes: ");
    scanf("%d", &n);

    for(int i=0; i<n; i++) {
        printf("\nEnter details for Process %d:\n", i+1);
        processes[i].pid = i+1;
        printf("Arrival Time: ");
        scanf("%d", &processes[i].arrivalTime);
        printf("Burst Time: ");
        scanf("%d", &processes[i].burstTime);
        processes[i].remainingTime = processes[i].burstTime;
        processes[i].waitingTime = 0;
        processes[i].turnaroundTime = 0;
        processes[i].completionTime = 0;
    }

    // Perform Multilevel Queue Scheduling
    multilevelQueueScheduling(processes, n);

    // Calculate Turnaround and Waiting Times
    for(int i=0; i<n; i++) {
        if(processes[i].completionTime !=0) {
            processes[i].turnaroundTime = processes[i].completionTime - processes[i].arrivalTime;
            processes[i].waitingTime = processes[i].turnaroundTime - processes[i].burstTime;
        }
    }

    // Print the results
    printResults(processes, n);

    return 0;
}