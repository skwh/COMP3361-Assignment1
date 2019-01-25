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

void Scheduler::simulateRoundRobin() {
    std::cout << "RR " << this->blockDuration << " " << this->timeSlice << std::endl;
    
    //TODO: implement
    
    //make sure the file was read in correctly
    for (std::vector<Task*>::iterator it = this->tasks.begin() ; it != tasks.end() ; ++it) {
        std::cout << (*it)->name << " " << (*it)->arrivalTime << " " << (*it)->totalTime << " " << (*it)->blockInterval << std::endl;
    }
}

void Scheduler::simulateSPN() {
    std::cout << "SPN " << this->blockDuration << std::endl;
    
    //TODO: implement
    
    std::cout << "Hey!!! I am also supposed to simulate SPN!!!" << std::endl;
}