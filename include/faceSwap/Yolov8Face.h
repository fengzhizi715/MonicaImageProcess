//
// Created by Tony on 2024/8/22.
//

#ifndef MONICAIMAGEPROCESS_YOLOV8FACE_H
#define MONICAIMAGEPROCESS_YOLOV8FACE_H

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <onnxruntime_cxx_api.h>
#include "../common/OnnxRuntimeBase.h"
#include "../utils/Utils.h"

using namespace cv;
using namespace std;
using namespace Ort;

class Yolov8Face: public OnnxRuntimeBase
{
public:
    Yolov8Face(std::string modelPath, const char* logId, const char* provider, const float conf_thresh=0.5, const float iou_thresh=0.4);

    void detect(cv::Mat src, std::vector<Bbox> &boxes);   // 只返回检测框,置信度和5个关键点这两个信息在后续的模块里没有用到

private:
    void preprocess(cv::Mat img);
    std::vector<float> input_image;
    int input_height;
    int input_width;
    float ratio_height;
    float ratio_width;
    float conf_threshold;
    float iou_threshold;
};

#endif //MONICAIMAGEPROCESS_YOLOV8FACE_H
