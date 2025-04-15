#include <iostream>
#include <queue>
#include <algorithm>
#include <vector>
#include <climits>

using namespace std;

struct Process {
    char id;
   int arrivalTime, burstTime, priority;
    int remainingTime, waitingTime, turnaroundTime, completionTime;
    int originalArrivalTime, originalBurstTime;

    Process(char i, int at, int bt, int p) : id(i), arrivalTime(at), burstTime(bt), priority(p), remainingTime(bt), originalArrivalTime(at), originalBurstTime(bt) {}
};

bool compareArrivalTime(const Process& a, const Process& b) {
    if (a.arrivalTime == b.arrivalTime) {
        return a.originalArrivalTime < b.originalArrivalTime;
    }
    return a.arrivalTime < b.arrivalTime;
}
bool compareBurstTime(Process& a, const Process& b) {
    if (a.burstTime == b.burstTime) {
        return a.originalArrivalTime < b.originalArrivalTime;
    }
     return a.burstTime < b.burstTime;
}
struct ComparePriority {
    bool operator()(const Process& a, const Process& b) {
        if (a.priority == b.priority) {
            return a.originalArrivalTime > b.originalBurstTime;
        }
        return a.priority < b.priority;
    }
};
bool compareRemainingTime(const Process& a, const Process& b) {
    if (a.remainingTime == b.remainingTime) {
        return a.originalBurstTime < b.originalBurstTime;
    }
     return a.remainingTime < b.remainingTime;
}
bool compareId(const Process& a, const Process& b) {
    return a.id < b.id;
}

void moveToInitialQueue(Process& process, vector<queue<Process>>& queues) {
    queues[0].push(process);
}

void runPreemptivePriority(vector<queue<Process>>& queues, vector<Process>& completedJobs, int& currentTime);
void runRoundRobin(vector<queue<Process>>& queues, vector<Process>& completedJobs, int& currentTime, int& algorithm, int quantumTime3);
void runScheduler(vector<queue<Process>>& queues, vector<Process>& completedJobs, int& currentTime, int& algorithm, int quantumTime3);
void MLFQ(vector<Process>& jobs, int numberOfJobs, int quantumTime, int algorithm, int quantumTime3, vector<Process>& completedJobs);
void DisplayResults(vector<Process>& completedJobs);

int main() {
    vector<Process> completedJobs;  // Vector to store completed jobs
    int numberOfJobs, quantumTime, algorithm;

    cout << "=============================================" << endl;
    cout << "Multi-Level Feedback Queue Scheduling Algorithm" << endl;
    cout << "=============================================" << endl;

    cout << "\nEnter the number of jobs: ";
    cin >> numberOfJobs;

    cout << "Enter the time quantum: ";
    cin >> quantumTime;

    cout << "(1 - FCFS, 2 - SJF, 3 - NPP, 4 - PP, 5 - SRTF, 6 - RR)\nChoose scheduling algorithm for Tier 3 : ";
    cin >> algorithm;

    vector<Process> processes;
    char jobId;
    int arrivalTime, burstTime, priority, quantumTime3;

    for (int i = 0; i < numberOfJobs; ++i) {
        cout << "\nEnter details for Job " << (char)('A' + i) << ":" << endl;
        cout << "Arrival Time: ";
        cin >> arrivalTime;
        cout << "Burst Time: ";
        cin >> burstTime;
        if (algorithm == 3 || algorithm == 4) {
            cout << "Priority: ";
            cin >> priority;
        }
        if (algorithm == 6) {
            cout << "Quantum Time: ";
            cin >> quantumTime3;
        }

        processes.emplace_back('A' + i, arrivalTime, burstTime, priority);
    }

    cout << "\n=============================================\n" << endl;
    
    // Run MLFQ scheduling algorithm
    MLFQ(processes, numberOfJobs, quantumTime, algorithm, quantumTime3, completedJobs);

    // Display results for completed jobs
    DisplayResults(completedJobs);

    return 0;
}

void MLFQ(vector<Process>& jobs, int numberOfJobs, int quantumTime, int algorithm, int quantumTime3, vector<Process>& completedJobs) {
    vector<queue<Process>> queues(3);

    int currentTime = 0;

    sort(jobs.begin(), jobs.end(), compareArrivalTime);
    for (const auto& job : jobs) {
        queues[0].push(job);
    }

    // Process Jobs in Queue 
    while (!queues[0].empty() || !queues[1].empty() || !queues[2].empty()) {

        // Process Jobs in Queue 0 or Tier 1
        while (!queues[0].empty()) {
            Process& currentJob = queues[0].front();

            if (currentJob.arrivalTime <= currentTime) {
                
                int executionTime = min(currentJob.remainingTime, quantumTime);
                currentTime += executionTime;
                currentJob.remainingTime -= executionTime;

                cout << "Executing in Tier 1 Job " << currentJob.id << " at Time " << currentTime
                    << " | Remaining Time: " << currentJob.remainingTime << endl;
                    
                if (currentJob.remainingTime >= 0) {
                    queues[1].push(currentJob);  // If not completed, push back to the priority queue
                } else {
                    currentJob.completionTime = currentTime;
                    completedJobs.push_back(currentJob);  // Store completed job
                    cout << " | Completion Time: " << currentTime;
                }

                queues[0].pop();  // Remove the process from the priority queue
            } else {
                break;
            }
        }

        while (!queues[1].empty()) {
            Process& currentJob = queues[1].front();

            if (currentJob.arrivalTime <= currentTime) {
                int executionTime = min(currentJob.remainingTime, quantumTime);
                currentTime += executionTime;
                currentJob.remainingTime -= executionTime;

                cout << "Executing in Tier 2 Job " << currentJob.id << " at Time " << currentTime
                    << " | Remaining Time: " << currentJob.remainingTime << endl;
                   
                if (currentJob.remainingTime > 0) {
                    queues[2].push(currentJob);  // If not completed, push back to the priority queue
                } else {
                    currentJob.completionTime = currentTime;
                    completedJobs.push_back(currentJob);  // Store completed job
                    cout << " | Completion Time: " << currentTime;
                }

                queues[1].pop();  // Remove the process from the priority queue
            } else {
                break;
            }

            if (!queues[0].empty() && queues[0].front().arrivalTime <= currentTime) {
                // Move back to processing jobs in Queue 0
                break;
            }
        }

        switch (algorithm) {
            case 1 ... 4:
                runScheduler(queues, completedJobs, currentTime, algorithm, quantumTime3);
                break;
            case 5:
                runPreemptivePriority(queues, completedJobs, currentTime);
                break;
            case 6:
                runRoundRobin(queues, completedJobs, currentTime, algorithm, quantumTime3);
                break;
        
        default:
            break;
        }
    }
}

void runScheduler(vector<queue<Process>>& queues, vector<Process>& completedJobs, int& currentTime, int& algorithm, int quantumTime3) {
    vector<Process> tier3Processes;
    // Extract processes from the tier 3 queue to a vector
    while (!queues[2].empty()) {
        tier3Processes.push_back(queues[2].front());
        queues[2].pop();
    }

    switch (algorithm) {
        case 1:
            // First Come First Serve
            // Sort the vector based on Arrival Time
            sort(tier3Processes.begin(), tier3Processes.end(), compareArrivalTime);
            break;
        case 2:
            // Shortest Job First
            // Sort the vector based on Burst Time
            sort(tier3Processes.begin(), tier3Processes.end(), compareBurstTime);
            break;
        case 3:
            // Shortest Remaining Time First
            // Sort the vector based on Remaining Burst Time
            sort(tier3Processes.begin(), tier3Processes.end(), compareRemainingTime);
            break;
        case 4:
            // Non-preemptive Priority
            // Sort the vector based on Priority
            sort(tier3Processes.begin(), tier3Processes.end(), ComparePriority());
            break;
        default:
            break;
    }

    // Add sorted processes back to the tier 3 queue
    for (const auto& process : tier3Processes) {
        queues[2].push(process);
    }

    while (!queues[2].empty()) {

        Process& currentJob = queues[2].front();

        if (!queues[0].empty() && queues[0].front().arrivalTime <= currentTime) {
            // Move back to processing jobs in Queue 0
            break;
        }

        if (currentJob.arrivalTime <= currentTime) {
            int executionTime = currentJob.remainingTime;
            currentTime += executionTime;
            currentJob.remainingTime -= executionTime;

            cout << "Executing in Tier 3 Job " << currentJob.id << " at Time " << currentTime
                << " | Remaining Time: " << currentJob.remainingTime << endl;

            if (currentJob.remainingTime <= 0) {
                currentJob.completionTime = currentTime;
                completedJobs.push_back(currentJob);  // Store completed job
                cout << " | Completion Time: " << currentTime << endl;
            } else {
                moveToInitialQueue(currentJob, queues);
            }

            queues[2].pop();  // Remove the process from the priority queue
        } else {
            break;
        }
    }
}

void runRoundRobin(vector<queue<Process>>& queues, vector<Process>& completedJobs, int& currentTime, int& algorithm, int quantumTime3) {
    vector<Process> tier3Processes;
    // Extract processes from the tier 3 queue to a vector
    while (!queues[2].empty()) {
        tier3Processes.push_back(queues[2].front());
        queues[2].pop();
    }

    sort(tier3Processes.begin(), tier3Processes.end(), compareArrivalTime);

    // Add sorted processes back to the tier 3 queue
    for (const auto& process : tier3Processes) {
        queues[2].push(process);
    }

    while (!queues[2].empty()) {

        Process& currentJob = queues[2].front();

        if (!queues[0].empty() && queues[0].front().arrivalTime <= currentTime) {
            // Move back to processing jobs in Queue 0
            break;
        }

        if (currentJob.arrivalTime <= currentTime) {
            int executionTime = min(currentJob.remainingTime, quantumTime3);
            currentTime += executionTime;
            currentJob.remainingTime -= executionTime;

            cout << "Executing in RR Job " << currentJob.id << " at Time " << currentTime
                << " | Remaining Time: " << currentJob.remainingTime << endl;

            if (currentJob.remainingTime <= 0) {
                if (currentJob.remainingTime <= 0 && !queues[0].empty() && queues[0].front().arrivalTime <= currentTime){
                    queues[2].push(currentJob);
                }                
                currentJob.completionTime = currentTime;
                completedJobs.push_back(currentJob);  // Store completed job
                cout << " | Completion Time: " << currentTime << endl;
            } else {
                queues[2].push(currentJob);
            }

            queues[2].pop();  // Remove the process from the priority queue
        } else {
            break;
        }
    }
}

void runPreemptivePriority(vector<queue<Process>>& queues, vector<Process>& completedJobs, int& currentTime) {
    if (!queues[2].empty()) {
        Process& currentJob = queues[2].front();

        if (currentJob.arrivalTime <= currentTime) {
            int nextArrivalTime = INT_MAX;  // Initialize with a large value

            // Check the next arrival time of the processes in the queues
            for (const auto& q : queues) {
                if (!q.empty() && q.front().arrivalTime < nextArrivalTime) {
                    nextArrivalTime = q.front().arrivalTime;
                }
            }

            int timeSlice = nextArrivalTime - currentTime;

            int executionTime = min(currentJob.remainingTime, timeSlice);
            currentTime += executionTime;
            currentJob.remainingTime -= executionTime;

            cout << "Executing in Priority Job " << currentJob.id << " at Time " << currentTime
                << " | Remaining Time: " << currentJob.remainingTime << endl;

            if (currentJob.remainingTime <= 0) {
                currentJob.completionTime = currentTime;
                completedJobs.push_back(currentJob);  // Store completed job
                cout << " | Completion Time: " << currentTime << endl;
            } else {
                queues[2].pop();  // Remove the process from the priority queue
                queues[2].push(currentJob);  // Push back to the priority queue for preemption
            }
        } else {
            cout << "No job eligible for execution at current time " << currentTime << endl;
        }
    } else {
        cout << "Priority queue is empty at current time " << currentTime << endl;
    }
}

void DisplayResults(vector<Process>& completedJobs) {
    sort(completedJobs.begin(), completedJobs.end(), compareId);
    cout << "\n=============================================" << endl;
    cout << "ID\tAT\tBT\tCT\tTAT\tWT\n" << endl;

    for (const auto& job : completedJobs) {
        cout << job.id << "\t"
             << job.originalArrivalTime << "\t"
             << job.originalBurstTime << "\t"
             << job.completionTime << "\t"
             << (job.completionTime - job.originalArrivalTime)<< "\t" 
             << ((job.completionTime - job.originalArrivalTime) - job.originalBurstTime)
             << endl;
    }
    cout << "=============================================\n" << endl;
}