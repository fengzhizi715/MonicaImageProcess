//
// Created by Tony on 2025/3/25.
//

#ifndef MONICAIMAGEPROCESS_GLOBALRESOURCE_H
#define MONICAIMAGEPROCESS_GLOBALRESOURCE_H

#include <memory>
#include "../../include/sketchDrawing/SketchDrawing.h"

using namespace std;
using namespace cv;

class GlobalResource {
public:
    GlobalResource(string modelPath);
    Mat processSketchDrawing(Mat src);

private:
    string modelPath;

    // 使用智能指针管理SketchDrawing对象
    std::unique_ptr<SketchDrawing> sketchDrawing;
};

#endif //MONICAIMAGEPROCESS_GLOBALRESOURCE_H
