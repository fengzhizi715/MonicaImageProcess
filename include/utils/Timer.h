//
// Created by Tony on 2024/8/17.
//

#ifndef MONICAIMAGEPROCESS_TIMER_H
#define MONICAIMAGEPROCESS_TIMER_H

#include <chrono>

class Timer {
public:
    Timer(double& accumulator, bool isEnabled = true);
    void stop();

private:
    double& accumulator;
    bool isEnabled;
    std::chrono::time_point<std::chrono::high_resolution_clock> start;
};


#endif //MONICAIMAGEPROCESS_TIMER_H
