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
#include <queue>

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
        
        this->tasks.push_back(currentTask);
    }
    if (fileStream.bad()) {
        throw std::runtime_error("Error while reading file " + this->fileName);
    }
    fileStream.close();
    this->simulateReady = true;
}

/*
 bool Scheduler::operator<(const Task& larg, const Task& rarg) {
    return larg.priority < rarg.priority;
}
*/

void Scheduler::simulateRoundRobin() {
    std::cout << "RR " << this->blockDuration << " " << this->timeSlice << std::endl;
    
    //TODO: implement
    
    int systemTime = 0;
    int sliceProg = 0;
    
    Task idle;
    idle.name = "<idle>";
    idle.intervalProg = 0;
    
    Task* head;
    Task* tail;
    Task* currTask;
    Task* pointer;
    
    std::vector<Task*>::iterator it = tasks.begin();
    for (it; it <= tasks.end(); it++) {
        if (it == tasks.begin()) {
            head = it;
        }
        else if (it == tasks.end()) {
            tail = it;
            tail->next = head;
        }
        else {
            it->next = it+1;
            it->blockProg = 0;
            it->intervalProg = 0;
            it->totalProg = 0;
        }
    }
    
    pointer = head;
    currTask = idle;
    while ((head != NULL) && (tail != NULL)) {
        
        // Checks to see if any new tasks have arrived and sets them to ready
        do {
            if (pointer->arrivalTime >= systemTime) {
                pointer->taskState = READY;
            }
            else {
                pointer->taskState = ARRIVING;
            }
            
            pointer = pointer->next;
        } while (pointer != head);
        
        
        // check if currTask should terminate
        if ((currTask->totalProg == currTask->totalTime) && currTask != idle) {
            std::cout << " " << systemTime << "    " << currTask->name << "    " << currTask->intervalProg << "    T";
            
            pointer = head;
            while (pointer->next != currTask) { // find task pointing to currTask
                pointer = pointer->next;
            }
            
            if (pointer->next == head) { // if currTask is head, set head to currTask->next and remove currTask
                pointer->next = head->next;
                head = pointer->next;
            }
            else if (pointer->next == tail) { // if currTask is tail, set tail to pointer (node before currTask) and remove currTask
                pointer->next = head;
                tail = pointer;
            }
            else if (pointer->next == pointer) { // if currTask is the only task, remove currTask, set head, tail, and pointer to null;
                pointer->next = NULL;
                pointer = NULL;
                head = NULL;
                tail = NULL;
            }
            else { // otherwise remove currTask
                pointer->next = pointer->next->next;
            }
            currTask = idle;    
        }
        
        // check if currTask should block
        if ((currTask->intervalProg == currTask->blockInterval) && (currTask != idle)) {
            std::cout << " " << systemTime << "    " << currTask->name << "    " << currTask->intervalProg << "    B";
            currTask->intervalProg = 0;
            currTask->taskState = BLOCKED;
            currTask = idle;
        }
        
        // checks blocked tasks and if they have fulfilled the block duration are set to ready
        pointer = head;  
        do { 
            if ((pointer->taskState == BLOCKED) && (pointer->blockProg == blockDuration)) {
                pointer->taskState = READY;
            }
            pointer = pointer->next;
        } while (pointer != head);
        
        
        // Check if currTask's timeslice is done 
        if (sliceProg == timeSlice) {  // if true, currTask = idle and check for a ready task. Also update progress on blocked tasks.
            std::cout << " " << systemTime << "    " << currTask->name << "    " << currTask->intervalProg << "    S";
            sliceProg = 0;
            currTask->taskState = READY;
            currTask = idle;
            pointer = head->next;
            while (pointer != head->next) {
                if (pointer->taskState == BLOCKED) {
                    pointer->blockProg++;
                }
                if (pointer->taskState == READY) {
                    currTask = pointer;
                }
                pointer = pointer->next;
            }
        }
        else if (currTask == idle) { // If idle, check for ready tasks
            pointer = head;
            do {
                if (pointer->taskState == READY)
                {
                    std::cout << " " << systemTime << "    " << currTask->name << "    " << currTask->intervalProg << "    I";
                    currTask = pointer;
                    
                }
            } while (pointer != head);
        }
        
        if (currTask != idle) {
            currTask->totalProg++;
            sliceProg++;
        }
        
        
        systemTime++;
        currTask->intervalProg++;
    }
    
    /*
    std::priority_queue<Task, std::vector<Task*>, operator<> scheduler;
    std::vector<Task*>::iterator it = tasks.begin();
    for (it; it != tasks.end(); it++) {
        scheduler.push(*it);
        if (it->arrivalTime == 0)
        {
            it->priority = 0;
            //it->taskState = READY;
        }
        else {
            it->priority = -2;
            //it->taskState = BLOCKED;
        }
        //it->taskState = BLOCKED;
        it->blockTime = 0;
        it->intervalTime = 0;
    }
    
    int systemTime = 0;
    int currTimeSlice = 0;
    
    Task* currentTask = NULL;
    
    while (!tasks.empty()) {
        std::vector<Task*> tempTasks;
        for (int i = 0; i <= scheduler.size(); i++) {
            tempTasks[i] = scheduler.top();
            scheduler.pop();
            
            if (tempTasks[i]->priority >= 0)
            {
                tempTasks[i]->priority++;
            }
            else if (tempTasks[i]->priority == -1)
            {
                tempTasks[i]->blockTime++;
            }
        }
        
        // This may need to go after the currTimeSlice if statement
        if (currentTask == NULL) {
            for (int i = 0; i <= tempTasks.size(); i++)
            {
                if (tempTasks[i]->priority >= 0)
                {
                    currentTask = tempTasks[i];
                }
            }
        }
        if (currentTask == NULL)
        {
            
        }
        
        
        if (currTimeSlice == timeSlice)
        {
            currTimeSlice = 0;
            currentTask->priority = 0;
            scheduler.push(currentTask);
            currentTask == NULL;
        }
        
        currentTask->totalTime--;
        currTimeSlice++;
        
        
        
        systemTime++;
     
    }
    
    
    //make sure the file was read in correctly
    for (std::vector<Task*>::iterator it = this->tasks.begin() ; it != tasks.end() ; ++it) {
        std::cout << (*it)->name << " " << (*it)->arrivalTime << " " << (*it)->totalTime << " " << (*it)->blockInterval << std::endl;
    }
     * */
}

void Scheduler::simulateSPN() {
    std::cout << "SPN " << this->blockDuration << std::endl;
    
    //TODO: implement
    
    std::cout << "Hey!!! I am also supposed to simulate SPN!!!" << std::endl;
}