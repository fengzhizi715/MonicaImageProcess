//
// Created by Tony on 2024/8/18.
//

#ifndef MONICAIMAGEPROCESS_UTILS_H
#define MONICAIMAGEPROCESS_UTILS_H

#include <iostream>
#include <vector>
#include "opencv2/opencv.hpp"

typedef struct
{
    float xmin;
    float ymin;
    float xmax;
    float ymax;
} Bbox;

std::wstring get_win_path(const std::string& path);

float GetIoU(const Bbox box1, const Bbox box2);

std::vector<int> nms(std::vector<Bbox> boxes, std::vector<float> confidences, const float nms_thresh);

#endif //MONICAIMAGEPROCESS_UTILS_H
