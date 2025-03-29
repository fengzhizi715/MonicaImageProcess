//
// Created by Tony on 2025/3/25.
//

#ifndef MONICAIMAGEPROCESS_GLOBALRESOURCE_H
#define MONICAIMAGEPROCESS_GLOBALRESOURCE_H

#include <memory>
#include "../../include/sketchDrawing/SketchDrawing.h"
#include "../../include/faceDetect/FaceDetect.h"
#include "../../include/faceSwap/Face68Landmarks.h"
#include "../../include/faceSwap/FaceEmbedding.h"
#include "../../include/faceSwap/FaceSwap.h"
#include "../../include/faceSwap/FaceEnhance.h"
#include "../../include/faceSwap/Yolov8Face.h"

using namespace std;
using namespace cv;

class GlobalResource {
public:
    GlobalResource(string modelPath);
    Mat processSketchDrawing(Mat src);
    Mat processFaceDetect(Mat src);

private:
    string modelPath;

    std::unique_ptr<SketchDrawing>   sketchDrawing;
    std::unique_ptr<FaceDetect>      faceDetect;
    std::unique_ptr<Yolov8Face>      yolov8Face;
    std::unique_ptr<Face68Landmarks> face68Landmarks;
    std::unique_ptr<FaceEmbedding>   faceEmbedding;
    std::unique_ptr<FaceSwap>        faceSwap;
    std::unique_ptr<FaceEnhance>     faceEnhance;
};

#endif //MONICAIMAGEPROCESS_GLOBALRESOURCE_H
