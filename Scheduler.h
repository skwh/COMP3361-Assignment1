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

class Scheduler {
public:
    
    Scheduler(std::string fileName, int blockDuration, int timeSlice);
    
    Scheduler(const Scheduler& orig) = delete;
    Scheduler(Scheduler && orig) = delete;
    Scheduler operator=(const Scheduler & orig) = delete;
    Scheduler operator=(Scheduler && orig) = delete;
    
    virtual ~Scheduler();
    
private:
    
    std::string fileName;
    int blockDuration;
    int timeSlice;

};

#endif /* SCHEDULER_H */

