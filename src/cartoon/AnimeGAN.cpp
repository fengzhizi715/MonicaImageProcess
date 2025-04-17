//
// Created by Tony on 2025/4/15.
//

#include "../../include/cartoon/AnimeGAN.h"

AnimeGAN::AnimeGAN(string modelPath, const char* logId, const char* provider): OnnxRuntimeBase(modelPath, logId, provider)
{
}

// 工具函数：将 OpenCV 的 Mat 转为 float tensor（NCHW）
std::vector<float> prepare_input_nhwc(const cv::Mat& img) {
    std::vector<float> input_tensor_values;
    for (int y = 0; y < img.rows; ++y) {
        for (int x = 0; x < img.cols; ++x) {
            for (int c = 0; c < 3; ++c) {
                input_tensor_values.push_back(img.at<cv::Vec3f>(y, x)[c]);
            }
        }
    }
    return input_tensor_values;
}


// 工具函数：将输出 tensor 转为 Mat
cv::Mat tensor_to_mat_nhwc(const float* data, int h, int w) {
    cv::Mat output(h, w, CV_32FC3);
    int idx = 0;
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            for (int c = 0; c < 3; ++c) {
                output.at<cv::Vec3f>(y, x)[c] = data[idx++];
            }
        }
    }
    return output;
}

void AnimeGAN::inferImage(Mat& src, Mat& dst)
{
    int w = src.cols;
    int h = src.rows;

    Mat temp;
    resize(src, temp, Size(this->inpWidth, this->inpHeight));
    temp.convertTo(temp, CV_32F, 1.0 / 255.0); // 归一化 [0,1]

    // 创建输入 tensor 数据
    std::vector<float> input_tensor_values = prepare_input_nhwc(temp);
    std::array<int64_t, 4> input_shape = {1, temp.rows, temp.cols, 3};

    Ort::MemoryInfo allocator_info = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
    Ort::Value input_tensor_ = Ort::Value::CreateTensor<float>(allocator_info, input_tensor_values.data(), input_tensor_values.size(), input_shape.data(), input_shape.size());

    vector<Value> ort_outputs = this -> forward(input_tensor_);

    // 取出输出数据
    float* output_data = ort_outputs.front().GetTensorMutableData<float>();
    dst = tensor_to_mat_nhwc(output_data, outHeight, outWidth);

    // 后处理输出图像
    dst = cv::min(cv::max(dst, 0.0f), 1.0f); // clamp 到 [0,1]
    dst.convertTo(dst, CV_8UC3, 255.0);      // 转回 [0,255]

    cv::resize(dst, dst, cv::Size(w, h));
}
