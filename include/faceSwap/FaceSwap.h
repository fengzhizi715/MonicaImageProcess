//
// Created by Tony on 2024/8/30.
//

#ifndef MONICAIMAGEPROCESS_FACESWAP_H
#define MONICAIMAGEPROCESS_FACESWAP_H

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <onnxruntime_cxx_api.h>
#include "../common/OnnxRuntimeBase.h"
#include "../utils/Utils.h"

class FaceSwap: public OnnxRuntimeBase
{
public:
    FaceSwap(std::string modelPath, std::string modelPath2, const char* logId, const char* provider);

    cv::Mat process(cv::Mat target, const std::vector<float> source_face_embedding, const std::vector<cv::Point2f> target_landmark_5);

    ~FaceSwap();  // 析构函数, 释放内存

private:
    void preprocess(cv::Mat target, const std::vector<cv::Point2f> face_landmark_5, const std::vector<float> source_face_embedding, cv::Mat& affine_matrix, cv::Mat& box_mask);

    std::vector<float> input_image;
    std::vector<float> input_embedding;
    int input_height;
    int input_width;
    const int len_feature = 512;
    float* model_matrix;
    std::vector<cv::Point2f> normed_template;
    const float FACE_MASK_BLUR = 0.3;
    const int FACE_MASK_PADDING[4] = {0, 0, 0, 0};
    const float INSWAPPER_128_MODEL_MEAN[3] = {0.0, 0.0, 0.0};
    const float INSWAPPER_128_MODEL_STD[3] = {1.0, 1.0, 1.0};
};

#endif //MONICAIMAGEPROCESS_FACESWAP_H
