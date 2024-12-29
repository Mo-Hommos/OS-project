#include <stdio.h>
#include <stdlib.h>

#define MAX_PROCESSES 50

typedef struct {
    int pid;
    int arrivalTime;
    int burstTime;
    int priority;
    int waitingTime;
    int turnaroundTime;
    int completionTime;
    int remainingTime;
} Process;

void calculateAverageTimes(Process processes[], int n, float *avgWaitingTime, float *avgTurnaroundTime) {
    int totalWaitingTime = 0, totalTurnaroundTime = 0;
    for (int i = 0; i < n; i++) {
        totalWaitingTime += processes[i].waitingTime;
        totalTurnaroundTime += processes[i].turnaroundTime;
    }
    *avgWaitingTime = (float)totalWaitingTime / n;
    *avgTurnaroundTime = (float)totalTurnaroundTime / n;
}

// Sort processes by arrival time
void sortByArrival(Process processes[], int n) {
    for(int i = 0; i < n-1; i++) {
        for(int j = 0; j < n-i-1; j++) {
            if(processes[j].arrivalTime > processes[j+1].arrivalTime) {
                Process temp = processes[j];
                processes[j] = processes[j+1];
                processes[j+1] = temp;
            }
        }
    }
}

// First-Come-First-Served (FCFS)
void fcfs(Process processes[], int n) {
    int time = 0;
    for (int i = 0; i < n; i++) {
        if (time < processes[i].arrivalTime) {
            time = processes[i].arrivalTime;
        }
        processes[i].waitingTime = time - processes[i].arrivalTime;
        time += processes[i].burstTime;
        processes[i].turnaroundTime = processes[i].waitingTime + processes[i].burstTime;
        processes[i].completionTime = time;
    }
}

// Shortest Job First (SJF) Non-Preemptive
void sjfNonPreemptive(Process processes[], int n) {
    int completed = 0, time = 0, shortest = -1;
    int visited[MAX_PROCESSES] = {0};

    while (completed < n) {
        shortest = -1;
        for (int i = 0; i < n; i++) {
            if (!visited[i] && processes[i].arrivalTime <= time &&
                (shortest == -1 || processes[i].burstTime < processes[shortest].burstTime)) {
                shortest = i;
            }
        }

        if (shortest == -1) {
            time++;
            continue;
        }

        visited[shortest] = 1;
        processes[shortest].waitingTime = time - processes[shortest].arrivalTime;
        time += processes[shortest].burstTime;
        processes[shortest].turnaroundTime = processes[shortest].waitingTime + processes[shortest].burstTime;
        processes[shortest].completionTime = time;
        completed++;
    }
}

// Shortest Job First (SJF) Preemptive
void sjfPreemptive(Process processes[], int n) {
    int time = 0, completed = 0, shortest = -1;
    for (int i = 0; i < n; i++) {
        processes[i].remainingTime = processes[i].burstTime;
    }

    while (completed < n) {
        shortest = -1;
        for (int i = 0; i < n; i++) {
            if (processes[i].arrivalTime <= time && processes[i].remainingTime > 0 &&
                (shortest == -1 || processes[i].remainingTime < processes[shortest].remainingTime)) {
                shortest = i;
            }
        }

        if (shortest == -1) {
            time++;
            continue;
        }

        processes[shortest].remainingTime--;
        if (processes[shortest].remainingTime == 0) {
            completed++;
            processes[shortest].completionTime = time + 1;
            processes[shortest].turnaroundTime = processes[shortest].completionTime - processes[shortest].arrivalTime;
            processes[shortest].waitingTime = processes[shortest].turnaroundTime - processes[shortest].burstTime;
        }
        time++;
    }
}

// Round Robin
void roundRobin(Process processes[], int n, int quantum) {
    int time = 0, completed = 0;
    for (int i = 0; i < n; i++) {
        processes[i].remainingTime = processes[i].burstTime;
    }

    while (completed < n) {
        for (int i = 0; i < n; i++) {
            if (processes[i].arrivalTime <= time && processes[i].remainingTime > 0) {
                int executed = (processes[i].remainingTime > quantum) ? quantum : processes[i].remainingTime;
                processes[i].remainingTime -= executed;
                time += executed;

                if (processes[i].remainingTime == 0) {
                    completed++;
                    processes[i].completionTime = time;
                    processes[i].turnaroundTime = processes[i].completionTime - processes[i].arrivalTime;
                    processes[i].waitingTime = processes[i].turnaroundTime - processes[i].burstTime;
                }
            }
        }
    }
}

// Priority Scheduling
void priorityScheduling(Process processes[], int n) {
    int completed = 0, time = 0, highestPriority = -1;
    int visited[MAX_PROCESSES] = {0};

    while (completed < n) {
        highestPriority = -1;
        for (int i = 0; i < n; i++) {
            if (!visited[i] && processes[i].arrivalTime <= time &&
                (highestPriority == -1 || processes[i].priority < processes[highestPriority].priority)) {
                highestPriority = i;
            }
        }

        if (highestPriority == -1) {
            time++;
            continue;
        }

        visited[highestPriority] = 1;
        processes[highestPriority].waitingTime = time - processes[highestPriority].arrivalTime;
        time += processes[highestPriority].burstTime;
        processes[highestPriority].turnaroundTime = processes[highestPriority].waitingTime + processes[highestPriority].burstTime;
        processes[highestPriority].completionTime = time;
        completed++;
    }
}

// Print results
void printResults(Process processes[], int n) {
    float avgWaitingTime, avgTurnaroundTime;
    calculateAverageTimes(processes, n, &avgWaitingTime, &avgTurnaroundTime);

    printf("\n%-5s %-7s %-6s %-8s %-8s %-11s %-10s\n", "PID", "Arrival", "Burst", "Priority", "Waiting", "Turnaround", "Completion");
    for (int i = 0; i < n; i++) {
        printf("%-5d %-7d %-6d %-8d %-8d %-11d %-10d\n",
               processes[i].pid, processes[i].arrivalTime, processes[i].burstTime,
               processes[i].priority, processes[i].waitingTime,
               processes[i].turnaroundTime, processes[i].completionTime);
    }

    printf("\nAverage Waiting Time: %.2f\n", avgWaitingTime);
    printf("Average Turnaround Time: %.2f\n", avgTurnaroundTime);
}

// Main function
int main() {
    int n, choice, quantum;
    Process processes[MAX_PROCESSES];

    printf("Enter the number of processes: ");
    scanf("%d", &n);

    for (int i = 0; i < n; i++) {
        printf("\nEnter details for Process %d:\n", i + 1);
        processes[i].pid = i + 1;
        printf("Arrival Time: ");
        scanf("%d", &processes[i].arrivalTime);
        printf("Burst Time: ");
        scanf("%d", &processes[i].burstTime);
        printf("Priority: ");
        scanf("%d", &processes[i].priority);
        processes[i].remainingTime = processes[i].burstTime;
        processes[i].waitingTime = 0;
        processes[i].turnaroundTime = 0;
        processes[i].completionTime = 0;
    }

    do {
        printf("\nSelect Scheduling Algorithm:\n");
        printf("1. FCFS\n2. SJF (Non-Preemptive)\n3. SJF (Preemptive)\n4. Round Robin\n");
        printf("5. Priority Scheduling\n6. Exit\n");
        printf("Choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                fcfs(processes, n);
                printResults(processes, n);
                break;
            case 2:
                sjfNonPreemptive(processes, n);
                printResults(processes, n);
                break;
            case 3:
                sjfPreemptive(processes, n);
                printResults(processes, n);
                break;
            case 4:
                printf("Enter Time Quantum: ");
                scanf("%d", &quantum);
                roundRobin(processes, n, quantum);
                printResults(processes, n);
                break;
            case 5:
                priorityScheduling(processes, n);
                printResults(processes, n);
                break;
            case 6:
                printf("Exiting...\n");
                break;
            default:
                printf("Invalid choice! Try again.\n");
        }
    } while (choice != 6);

    return 0;
