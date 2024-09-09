//
// Created by Tony on 2024/8/30.
//

#ifndef MONICAIMAGEPROCESS_FACEEMBEDDING_H
#define MONICAIMAGEPROCESS_FACEEMBEDDING_H

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <onnxruntime_cxx_api.h>
#include "../common/OnnxRuntimeBase.h"
#include "../utils/Utils.h"

class FaceEmbedding: public OnnxRuntimeBase
{
public:
    FaceEmbedding(std::string modelPath, const char* logId, const char* provider);

    std::vector<float> detect(cv::Mat src, const std::vector<cv::Point2f> face_landmark_5);

private:
    void preprocess(cv::Mat src, const std::vector<cv::Point2f> face_landmark_5);
    std::vector<float> input_image;
    int input_height;
    int input_width;
    std::vector<cv::Point2f> normed_template;
};

#endif //MONICAIMAGEPROCESS_FACEEMBEDDING_H
