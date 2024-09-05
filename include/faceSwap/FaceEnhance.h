//
// Created by Tony on 2024/8/30.
//

#ifndef MONICAIMAGEPROCESS_FACEENHANCE_H
#define MONICAIMAGEPROCESS_FACEENHANCE_H

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <onnxruntime_cxx_api.h>
#include "../common/OnnxRuntimeBase.h"
#include "../utils/Utils.h"

class FaceEnhance: public OnnxRuntimeBase
{
public:
    FaceEnhance(std::string modelPath, const char* logId, const char* provider);

    cv::Mat process(cv::Mat target, const std::vector<cv::Point2f> target_landmark_5);

private:
    void preprocess(cv::Mat target, const std::vector<cv::Point2f> face_landmark_5, cv::Mat& affine_matrix, cv::Mat& box_mask);
    std::vector<float> input_image;
    int input_height;
    int input_width;
    std::vector<cv::Point2f> normed_template;
    const float FACE_MASK_BLUR = 0.3;
    const int FACE_MASK_PADDING[4] = {0, 0, 0, 0};
};

#endif //MONICAIMAGEPROCESS_FACEENHANCE_H