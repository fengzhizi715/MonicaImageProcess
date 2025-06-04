//
// Created by Tony on 2025/4/18.
//
#include "../../include/utils/Utils.h"
#include <iostream>
#include <string>

bool ascendSort(vector<Point> a, vector<Point> b) {
    return contourArea(a) > contourArea(b);
}