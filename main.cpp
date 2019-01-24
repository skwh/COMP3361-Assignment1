/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: evan
 *
 * Created on January 24, 2019, 11:04 AM
 */

#include <cstdlib>
#include <iostream>

/*
 * 
 */
int main(int argc, char** argv) {
    if (argc != 4) {
        std::cout << "Usage: " << argv[0] 
                << " file_name block_duration time_slice" << std::endl;
        return 1;
    }
    std::string fileName = std::string(argv[1]);
    int blockDuration = argv[2] - '0';
    int timeSlice = argv[3] - '0';
    
    
    
    return 0;
}

