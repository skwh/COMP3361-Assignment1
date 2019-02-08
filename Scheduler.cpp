/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Scheduler.cpp
 * Author: evan
 * 
 * Created on January 24, 2019, 11:22 AM
 */

#include "Scheduler.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>

Scheduler::Scheduler(std::string fileName, int blockDuration, int timeSlice) {
    this->fileName = fileName;
    this->blockDuration = blockDuration;
    this->timeSlice = timeSlice;
    
    this->tasks = std::vector<Task*>();
    this->simulateReady = false;
    
    this->readTasksFromFile();
    
}

Scheduler::~Scheduler() {
    for (std::vector<Task*>::iterator it = this->tasks.begin(); it != this->tasks.end(); ++it) {
        delete *it;
    }
}

void Scheduler::Simulate(Scheduler::ALGORITHM a) {
    if (!this->simulateReady) {
        std::cerr << "Simulation Halted! Scheduler was not ready!" << std::endl;
        return;
    }
    if (a == Scheduler::ALGORITHM::ROUND_ROBIN) {
        this->simulateRoundRobin();
    } else {
        this->simulateSPN();
    }
    this->resetTaskStats();
}

void Scheduler::readTasksFromFile() {
    std::ifstream fileStream(this->fileName, std::ios::binary);
    if (!fileStream.is_open()) {
        throw std::runtime_error("Couldn't open file " + this->fileName);
    }
    std::string line;
    while (std::getline(fileStream, line)) {
        std::istringstream currentLineStream(line);
        
        Task* currentTask = new Task;
        currentLineStream >> currentTask->name;
        std::string intValues;
        
        //read in arrival_time
        currentLineStream >> intValues;
        currentTask->arrivalTime = std::stoi(intValues, nullptr, 10);
        
        //read in total_time
        currentLineStream >> intValues;
        currentTask->totalTime = std::stoi(intValues, nullptr, 10);
        
        //read in block_interval
        currentLineStream >> intValues;
        currentTask->blockInterval = std::stoi(intValues, nullptr, 10);
        
        currentTask->next = new Task;
        
        this->tasks.push_back(currentTask);
    }
    if (fileStream.bad()) {
        throw std::runtime_error("Error while reading file " + this->fileName);
    }
    fileStream.close();
    this->simulateReady = true;
}

void Scheduler::resetTaskStats() {
    for (std::vector<Task*>::iterator it = tasks.begin(); it != tasks.end(); ++it) {
        (*it)->blockProg = (*it)->arrivalTime;
        (*it)->taskState = Scheduler::STATE::BLOCKED;
        (*it)->totalProg = (*it)->totalTime;
        (*it)->intervalProg = 0;
        (*it)->turnAround = 0;
    }
}


void Scheduler::simulateRoundRobin() {
    std::cout << "RR " << this->blockDuration << " " << this->timeSlice << std::endl;
    
    // create systemTime and sliceProgress variables
    int systemTime = 0;
    int sliceProg = 0;
    
    // Create, allocate memory and initialize pointers for linked list
    Task idleTask;
    idleTask.name = "<idle>";
    idleTask.intervalProg = 0;
    idleTask.taskState = BLOCKED;
    Task* idle = &idleTask;
    
    Task* head = new Task;
    head->next = new Task;
    
    Task* tail = new Task;
    tail->next = new Task;
    
    Task* currTask = new Task;
    currTask->next = new Task;
    
    Task* pointer = new Task;
    pointer->next = new Task;
    
    
    // Create linked list from task vector and initialize remaining helper variables in each task
    constructList(head, tail);
    
    // Initialize idle helper bool and set pointer and currentTask in preparation for the while loop
    bool stayIdle = false;
    pointer = head;
    currTask = idle;
    
    
    // Main loop in which round robin scheduling takes place
    while ((head != nullptr) && (tail != nullptr)) {
        
        // Checks to see if any new tasks have arrived and sets them to ready
        checkArrivals(pointer, head, &systemTime);
        
        
        // check if currTask should terminate
        if ((currTask != idle) && (currTask->totalProg == currTask->totalTime)) {
            checkTermination(currTask, pointer, head, tail, idle, &sliceProg, &systemTime);
        }
        
        
        if(head != 0) {
            // check if currTask should block
            if ((currTask != idle) && (currTask->intervalProg == currTask->blockInterval)) {
                checkBlock(currTask, idle, pointer, &sliceProg);
            }
            
            // checks blocked tasks and if they have fulfilled the block duration are set to ready, if not increment their block progress
            updateBlocks(pointer, head);
        
            // Check if a new Task needs to be loaded
            switchTask(currTask, idle, pointer, &sliceProg, &systemTime, &stayIdle);
        
            // checks blocked tasks and if they have fulfilled the block duration are set to ready, if not increment their block progress
            //updateBlocks(pointer, head);
        
            // if currTask is not idle increment currTask's total progress and the slice progress
            if (currTask != idle) {
                currTask->totalProg++;
            }
        
            sliceProg++;
            systemTime++;
           currTask->intervalProg++;
        }
    }
    std::cout << " " << systemTime << "\t" << "<done>" << "\t" << calcTurnAround(&systemTime) << std::endl; 
}


void Scheduler::constructList(Task*& head, Task*& tail) {
    // Create linked list from task vector and initialize remaining helper variables in each task
    std::vector<Task*>::iterator it = tasks.begin();
    for (it = tasks.begin(); it < tasks.end(); it++) {
        if (it == tasks.begin()) {
            head = *it;
            (*it)->next = *(it + 1);
        }
        else if (it == tasks.end()-1) {
            tail = *it;
            tail->next = head;
        }
        else {
            (*it)->next = *(it + 1);
        }
        (*it)->blockProg = 0;
        (*it)->intervalProg = 0;
        (*it)->totalProg = 0;
        (*it)->turnAround = 0;
    }
}

void Scheduler::checkArrivals(Task*& pointer, Task*& head, int* systemTime) {
    // Checks to see if any new tasks have arrived and sets them to ready
    pointer = head;
    do {
        if ((pointer->arrivalTime) == (*systemTime)) {
            pointer->taskState = READY;
        }
        else if (pointer->arrivalTime >= (*systemTime)) {
            pointer->taskState = ARRIVING;
        }
            
        pointer = pointer->next;
    } while (pointer != head);
}

void Scheduler::checkTermination(Task*& currTask, Task*& pointer, Task*& head, Task*& tail, Task*& idle, int* sliceProg, int* systemTime) {
    // check if currTask should terminate
    std::cout << (*sliceProg) << "\tT" << std::endl;
            
    pointer = head;
    while (pointer->next != currTask) { // find task pointing to currTask
        pointer = pointer->next;
    }
            
    if (pointer->next == pointer) { // if currTask is the only task, remove currTask, set head, tail, and pointer to null;
        pointer->next = nullptr;
        pointer = nullptr;
        head = nullptr;
        tail = nullptr; 
    }
    else if (pointer->next == tail) { // if currTask is tail, set tail to pointer (node before currTask) and remove currTask
        pointer->next = head;
        tail = pointer;
    }
    else if (pointer->next == head) { // if currTask is head, set head to currTask->next and remove currTask
        pointer->next = head->next;
        head = pointer->next;
    }
    else { // otherwise remove currTask
        pointer->next = pointer->next->next;
    }
    currTask->turnAround = (*systemTime);
    (*sliceProg) = 0;
    currTask = idle;    
}

void Scheduler::checkBlock(Task*& currTask, Task*& idle, Task*& pointer, int* sliceProg) {
    // check if currTask should block
    std::cout << (*sliceProg) << "\tB" << std::endl;
        
    currTask->intervalProg = 0;
    (*sliceProg) = 0;
    currTask->taskState = BLOCKED;
    pointer = currTask;
    currTask = idle;
}
 
void Scheduler::switchTask(Task*& currTask, Task*& idle, Task*& pointer, int* sliceProg, int* systemTime, bool* stayIdle) {
    // Check if a new Task needs to be loaded
    // Check if currTask's timeslice is done 
    if (((*sliceProg) == timeSlice) && (currTask != idle)) {  // if true, currTask = idle and check for a ready task. Also update progress on blocked tasks.
        std::cout << (*sliceProg) << "\tS" << std::endl;
        (*sliceProg) = 0;
        Task* temp = currTask->next;
        pointer = temp;
        currTask = idle;
            
        do { // find next task to switch to
            if (pointer->taskState == READY) {
                currTask = pointer;
                std::cout << " " << (*systemTime) << "\t" << currTask->name << "\t"; 
                break;
            }
            pointer = pointer->next;
        } while (pointer != temp);
    }
    else if (currTask == idle) { // If idle, check for ready tasks
        Task* temp = pointer;
        do {
            if ((pointer->taskState == READY)) {
                if ((*sliceProg) > 0) {
                    std::cout << (*sliceProg) <<  "\tI" << std::endl;
                    (*sliceProg) = 0;
                    (*stayIdle) = false;
                }
                        
                currTask = pointer;
                std::cout << " " << (*systemTime) << "\t" << currTask->name << "\t";
                break;
            }
            pointer = pointer->next;
        } while (pointer != temp);
                
        if ((currTask == idle) && (!(*stayIdle))) {
            std::cout << " " << (*systemTime) << "\t" << currTask->name << "\t";
            (*stayIdle) = true;
        }
    }
 }

void Scheduler::updateBlocks(Task*& pointer, Task*& head) {
   // checks blocked tasks and if they have fulfilled the block duration are set to ready, if not increment their block progress
            pointer = head;  
            do { 
                if ((pointer->taskState == BLOCKED) && (pointer->blockProg == blockDuration)) {
                    pointer->taskState = READY;
                    pointer->blockProg = 0;
               }
                else if (pointer->taskState == BLOCKED) {
                    pointer->blockProg++;
                }
                pointer = pointer->next;
            } while (pointer != head);  
}

float Scheduler::calcTurnAround(int* systemTime) {
    std::vector<Task*>::iterator it = tasks.begin();
    float turnAroundSum = 0;
    float i = 0;
    for (it = tasks.begin(); it < tasks.end(); it++, i++) {
        turnAroundSum += (*it)->turnAround;
    }
    return (turnAroundSum / i); 
}




void Scheduler::simulateSPN() {
    std::cout << "SPN " << this->blockDuration << std::endl;
    Task* runningTask = nullptr;
    std::unordered_set<Task*> readyTasks;
    int systemTime = 0;
    int terminatedTasks = 0;
    int globalIntervalTime = 0;
    bool idling = false;
    std::vector<Task*>::iterator it;
    while (terminatedTasks < tasks.size()) {
        for (it = tasks.begin(); it != tasks.end(); ++it) {
            Task* currentTask = (*it);
            switch (currentTask->taskState) {
                case Scheduler::STATE::TERMINATED:
                    // Ignore terminated tasks
                    continue;
                case Scheduler::STATE::BLOCKED:
                    // If the task is not done blocking, break
                    if (!spnHandleBlocked(currentTask)) {
                        break;
                    }
                    // If the task is done blocking, the program falls through
                case Scheduler::STATE::READY:
                    // Add the ready task to the list of ready tasks
                    readyTasks.insert(currentTask);
                    break;
                case Scheduler::STATE::RUNNING:
                    spnHandleRunning(runningTask, currentTask, systemTime, globalIntervalTime, terminatedTasks);
                    break;
                default:
                    break;
            }
        }
        // Choose another task to run, if the current task has finished and other tasks are ready
        if (runningTask == nullptr && !readyTasks.empty()) {
            chooseSPN(runningTask, readyTasks, systemTime, globalIntervalTime, idling);
        }
        // No ready process is avaliable, the system is idling
        if (runningTask == nullptr && terminatedTasks != tasks.size()) {
            if (!idling) {
                idling = true;
                std::cout << " " << systemTime << "\t" << "<idle>" << "\t";
            }
        }
        // The task list has been exhausted
        if (terminatedTasks == tasks.size()) {
            float turnaroundTime = calcTurnAround(&systemTime);
            std::cout << " " << systemTime << "\t" << "<done>" << "\t" << turnaroundTime << std::endl;
            return;
        } else {
            globalIntervalTime++;
            systemTime++;
        }
    }
}

void Scheduler::chooseSPN(Task*& runningTask, std::unordered_set<Task*>& readyTasks, int& systemTime, int& globalIntervalTime, bool& idling) {
    Task* shortestTask = nullptr;
    unsigned int shortestTaskTime = -1;
    // Find the shortest task
    for (std::unordered_set<Task*>::iterator sit = readyTasks.begin(); sit != readyTasks.end(); ++sit) {
        Task* currentReadyTask = (*sit);
        if (currentReadyTask->blockInterval < shortestTaskTime) {
            shortestTask = currentReadyTask;
            shortestTaskTime = shortestTask->blockInterval;
        }
    }
    if (shortestTask != nullptr) {
        // If the scheduler was previously idling, show the end of the idle
        if (idling) {
            std::cout << globalIntervalTime << "\t" << "I" << std::endl;
            idling = false;
        }
        // Set the new task as the running task
        runningTask = shortestTask;
        readyTasks.clear();
        runningTask->taskState = Scheduler::STATE::RUNNING;
        runningTask->totalProg--;
        runningTask->intervalProg++;
        globalIntervalTime = 0;
        std::cout << " " << systemTime << "\t" << runningTask->name << "\t";
    }
}

void Scheduler::spnHandleRunning(Task*& runningTask, Task*& currentTask, int& systemTime, int& globalIntervalTime, int& terminatedTasks) {
    if (currentTask->totalProg > 0) {
        if (currentTask->intervalProg < currentTask->blockInterval) {
            // The process does not block yet
            currentTask->intervalProg++;
            currentTask->totalProg--;
        } else {
            // The process blocks
            currentTask->intervalProg = 0;
            std::cout << globalIntervalTime << "\t" << "B" << std::endl;
            currentTask->taskState = Scheduler::STATE::BLOCKED;
            runningTask = nullptr;
        }
    } else {
        // The process is finished, terminate it
        std::cout << globalIntervalTime << "\t" << "T" << std::endl;
        currentTask->taskState = Scheduler::STATE::TERMINATED;
        currentTask->turnAround = systemTime;
        terminatedTasks++;
        runningTask = nullptr;
        globalIntervalTime = 0;
    }
}

bool Scheduler::spnHandleBlocked(Task*& currentTask) {
    // Count down on the block progress
    currentTask->blockProg--;
    if (currentTask->blockProg <= 0) {
        // Task is unblocked, move to ready
        currentTask->taskState = Scheduler::STATE::READY;
        currentTask->blockProg = this->blockDuration;
        return true;
    } else {
        return false;
    }
}