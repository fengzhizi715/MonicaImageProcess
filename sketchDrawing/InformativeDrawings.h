//
// Created by Tony on 2024/8/13.
//

#ifndef MONICAIMAGEPROCESS_INFORMATIVEDRAWINGS_H
#define MONICAIMAGEPROCESS_INFORMATIVEDRAWINGS_H

#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
//#include <cuda_provider_factory.h>
#include <onnxruntime_cxx_api.h>
#include "../BaseOnnxRuntime.h"

using namespace cv;
using namespace std;
using namespace Ort;

class InformativeDrawings: public BaseOnnxRuntime {

public:
    InformativeDrawings(string modelpath);
    Mat detect(Mat& cv_image);

private:
    vector<float> input_image_;
    int inpWidth;
    int inpHeight;
    int outWidth;
    int outHeight;
};

#endif //MONICAIMAGEPROCESS_INFORMATIVEDRAWINGS_H
