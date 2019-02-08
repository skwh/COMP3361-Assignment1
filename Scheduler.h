/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Scheduler.h
 * Author: evan
 *
 * Created on January 24, 2019, 11:22 AM
 */

#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <string>
#include <vector>
#include <unordered_set>

class Scheduler {
public:
    
    Scheduler(std::string fileName, int blockDuration, int timeSlice);
    
    Scheduler(const Scheduler& orig) = delete;
    Scheduler(Scheduler && orig) = delete;
    Scheduler operator=(const Scheduler & orig) = delete;
    Scheduler operator=(Scheduler && orig) = delete;
    
    virtual ~Scheduler();
    
    enum ALGORITHM {
        ROUND_ROBIN, SPN
    };
    
    void Simulate(ALGORITHM a);
    
private:
    
    //separate methods for simulation for logic encapsulation. we have to 
    //duplicate the task data if we want the simulations to be independent
    
    /*
     * Initializes a number of variables for use in the Round Robin scheduling
     * algorithm and contains the overarching while loop that contains the main
     * while loop in which the algorithm runs.
     */
    void simulateRoundRobin();
    
    void simulateSPN();
    
    void readTasksFromFile();
    
    void resetTaskStats();
    
    enum STATE {
        BLOCKED, READY, TERMINATED, RUNNING, ARRIVING
    };
    
    struct Task {
        std::string name;
        int arrivalTime;
        int totalTime;
        int blockInterval;
        
        STATE taskState;
        int blockProg;
        int intervalProg;
        int totalProg;
        Task* next;
        float turnAround;
    };
    
    
    
    //consider switching this to a priority queue for use with SPN
    //also tasks are stored on the heap in case we want to extensively
    //copy them and modify their data- perhaps this makes the simulation easier?
    //preserving task state from read-in helps keep simulation times down
    //(no extra reading from file)
    std::vector<Task*> tasks; 
    bool simulateReady;
    
    /**
     * Constructs a singly linked list using the elements of the tasks vector
     * and assigns the head and tail of the list
     * @param head: Pointer to be set to the head of the list
     * @param tail: Pointer to be set to the tail of the list
     */
    void constructList(Task*& head, Task*& tail);
    /**
     * Checks to see if any Tasks have arrived at the current Time interval
     * @param pointer: Pointer for searching the linked list
     * @param head: Pointer to the head of the list
     * @param systemTime: Pointer to system time variable
     */
    void checkArrivals(Task*& pointer, Task*& head, int* systemTime);
    /**
     * Checks to see if the current task should terminate
     * @param currTask: Pointer to the current task
     * @param pointer: Pointer for searching the linked list
     * @param head: Pointer to the head of the list
     * @param tail: Pointer to the tail of the list
     * @param idle: Pointer to the idle task
     * @param sliceProg: Pointer to the slice progress variable
     * @param systemTime: Pointer to the systemTime variabel
     */
    void checkTermination(Task*& currTask, Task*& pointer, Task*& head, Task*& tail, Task*& idle, int* sliceProg, int* systemTime);
    /**
     * Checks to see if the current task should block
     * @param currTask: Pointer to the Current task
     * @param idle: Pointer to the idle task
     * @param pointer: Pointer for searching the linked list
     * @param sliceProg: Pointer to the slice progress variable
     */
    void checkBlock(Task*& currTask, Task*& idle, Task*& pointer, int* sliceProg);
    /**
     * If the current task needs to be switched due to it being idle or reaching
     * the end of its time slice, switches the current task to the next ready
     * task. If no ready tasks, the current task is made idle
     * @param currTask: Pointer to the current task
     * @param idle: Pointer to the idle task
     * @param pointer: Pointer for searching the linked list
     * @param sliceProg: Pointer to the slice progress variable
     * @param systemTime: Pointer to the system time variable
     * @param stayIdle: Pointer to a bool meant to signal if this idle task is 
     * meant to persist due to a lack of ready tasks. 
     */
    void switchTask(Task*& currTask, Task*& idle, Task*& pointer, int* sliceProg, int* systemTime, bool* stayIdle);
    /**
     * Checks all blocked tasks and either increments their block progress or
     * unblocks them if they have completed their block progress
     * @param pointer: Pointer for searching the linked list
     * @param head: Pointer to the head of the list
     */
    void updateBlocks(Task*& pointer, Task*& head);
    /**
     * Calculates the average turn around time of all the tasks
     * @param systemTime: Pointer to the system time variables
     * @return: Returns the average turn around time  
     */
    float calcTurnAround(int* systemTime);
    
    /**
     * Choose the next ready process to run based on which task will take the shortest amount of time. 
     * @param runningTask: pointer to the current running task
     * @param readyTasks: the unordered set of tasks which are ready to run
     * @param systemTime: integer scheduler system time
     * @param globalIntervalTime: the amount of time that the scheduler was in the previous state
     * @param idling: boolean indicating if the scheduler was idling
     */
    void chooseSPN(Task*& runningTask, std::unordered_set<Task*>& readyTasks, int& systemTime, int& globalIntervalTime, bool& idling);
    
    /**
     * Handle the logic for when a process is running in SPN.
     * @param runningTask: pointer to the current running task
     * @param currentTask: pointer to the task which is currently being evaluated
     * @param systemTime: integer scheduler system time
     * @param globalIntervalTime: the amount of time the scheduler was in the previous state
     * @param terminatedTasks: integer representing how many tasks have terminated
     */
    void spnHandleRunning(Task*& runningTask, Task*& currentTask, int& systemTime, int& globalIntervalTime, int& terminatedTasks);
    
    /**
     * Handle the logic for when a process is blocked in SPN.
     * @param currentTask: pointer to the task which is currently being evaluated
     * @return if the task is done blocking
     */
    bool spnHandleBlocked(Task*& currentTask);
    
    std::string fileName;
    int blockDuration;
    int timeSlice;
};

#endif /* SCHEDULER_H */

