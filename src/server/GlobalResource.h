//
// Created by Tony on 2025/3/25.
//

#ifndef MONICAIMAGEPROCESS_GLOBALRESOURCE_H
#define MONICAIMAGEPROCESS_GLOBALRESOURCE_H

#include <memory>
#include "../../include/sketchDrawing/SketchDrawing.h"
#include "../../include/faceDetect/FaceDetect.h"

using namespace std;
using namespace cv;

class GlobalResource {
public:
    GlobalResource(string modelPath);
    Mat processSketchDrawing(Mat src);
    Mat processFaceDetect(Mat src);

private:
    string modelPath;

    std::unique_ptr<SketchDrawing> sketchDrawing;
    std::unique_ptr<FaceDetect> faceDetect;
};

#endif //MONICAIMAGEPROCESS_GLOBALRESOURCE_H
