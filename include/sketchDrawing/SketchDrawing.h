//
// Created by Tony on 2024/8/13.
//

#ifndef MONICAIMAGEPROCESS_SKETCHRAWING_H
#define MONICAIMAGEPROCESS_SKETCHRAWING_H

#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
//#include <cuda_provider_factory.h>
#include <onnxruntime_cxx_api.h>
#include "../common/OnnxRuntimeBase.h"

using namespace cv;
using namespace std;
using namespace Ort;

class SketchDrawing: public OnnxRuntimeBase {

public:
    SketchDrawing(std::string modelPath, const char* logId, const char* provider);

    void inferImage(Mat& src, Mat& dst);

private:
    vector<float> input_image_;
    int inpWidth;
    int inpHeight;
    int outWidth;
    int outHeight;
};

#endif // MONICAIMAGEPROCESS_SKETCHRAWING_H
