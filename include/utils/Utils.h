//
// Created by Tony on 2025/4/18.
//

#ifndef MONICAIMAGEPROCESS_UTILS_H
#define MONICAIMAGEPROCESS_UTILS_H

#include <vector>
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;

bool ascendSort(vector<Point> a, vector<Point> b);

std::string detectRawFormat(const std::string& path);

#endif //MONICAIMAGEPROCESS_UTILS_H
