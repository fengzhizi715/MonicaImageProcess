//
// Created by Tony on 2024/8/17.
//

#include "../../include/utils/Timer.h"

Timer::Timer(double& accumulator, bool isEnabled)
        : accumulator(accumulator), isEnabled(isEnabled) {
    if (isEnabled) {
        start = std::chrono::high_resolution_clock::now();
    }
}

// Stop the timer and update the accumulator
void Timer::stop() {
    if (isEnabled) {
        auto end = std::chrono::high_resolution_clock::now();
        double duration = std::chrono::duration<double>(end - start).count();
        accumulator += duration;
    }
}