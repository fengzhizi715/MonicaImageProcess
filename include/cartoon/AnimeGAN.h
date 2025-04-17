//
// Created by Tony on 2025/4/15.
//

#ifndef MONICAIMAGEPROCESS_ANIMEGAN_H
#define MONICAIMAGEPROCESS_ANIMEGAN_H

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

class AnimeGAN: public OnnxRuntimeBase
{
public:
    AnimeGAN(std::string modelPath, const char* logId, const char* provider);

    void inferImage(Mat& src, Mat& dst);
private:
    const int inpWidth = 512;
    const int inpHeight = 512;
    const int outWidth = 512;
    const int outHeight = 512;
    vector<float> input_image_;
};

#endif //MONICAIMAGEPROCESS_ANIMEGAN_H
