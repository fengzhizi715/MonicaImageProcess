//
// Created by Tony on 2024/8/22.
//

#ifndef MONICAIMAGEPROCESS_FACE68LANDMARKS_H
#define MONICAIMAGEPROCESS_FACE68LANDMARKS_H

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <onnxruntime_cxx_api.h>
#include "../common/OnnxRuntimeBase.h"
#include "../utils/Utils.h"

using namespace cv;
using namespace std;
using namespace Ort;

class Face68Landmarks: public OnnxRuntimeBase
{
public:
    Face68Landmarks(std::string modelPath, const char* logId, const char* provider);

    std::vector<cv::Point2f> detect(cv::Mat src, const Bbox bounding_box, std::vector<cv::Point2f> &face_landmark_5of68);

private:
    void preprocess(cv::Mat src, const Bbox bounding_box);

    std::vector<float> input_image;
    int input_height;
    int input_width;
    cv::Mat inv_affine_matrix;
};

#endif //MONICAIMAGEPROCESS_FACE68LANDMARKS_H
