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
    void simulateRoundRobin();
    void simulateSPN();
    
    void readTasksFromFile();
    
    enum STATE {
        BLOCKED, READY, ARRIVING
    };
    
    struct Task {
        char name;
        int arrivalTime;
        int totalTime;
        int blockInterval;
        
        STATE taskState;
        //int priority;
        int blockProg;
        int intervalProg;
        int totalProg;
        Task* next;
    };
    
    bool operator<(const Task& larg, const Task& rarg);
    
    //the tasks will need to be duplicated before they are used in the simulations,
    //see comments above & below
    Task* copyTask(const Task* orig) {
        return new Task {
            orig->name,
            orig->arrivalTime,
            orig->totalTime,
            orig->blockInterval
        };
    }
    
    std::string fileName;
    int blockDuration;
    int timeSlice;
    
    //consider switching this to a priority queue for use with SPN
    //also tasks are stored on the heap in case we want to extensively
    //copy them and modify their data- perhaps this makes the simulation easier?
    //preserving task state from read-in helps keep simulation times down
    //(no extra reading from file)
    std::vector<Task*> tasks; 
    bool simulateReady;

};

#endif /* SCHEDULER_H */

