//
// Created by Tony on 2024/8/13.
//
#include <iomanip>
#include "../../include/sketchDrawing/SketchDrawing.h"
#include "../../include/utils/Timer.h"

SketchDrawing::SketchDrawing(string modelPath, const char* logId, const char* provider): OnnxRuntimeBase(modelPath, logId, provider)
{
    this->inpHeight = input_node_dims[0][2];
    this->inpWidth = input_node_dims[0][3];
    this->outHeight = output_node_dims[0][2];
    this->outWidth = output_node_dims[0][3];
}

void SketchDrawing::inferImage(Mat& src, Mat& dst)
{
    double preprocessTime = 0.0;
    double inferenceTime = 0.0;
    double postprocessTime = 0.0;

    Timer preprocessTimer = Timer(preprocessTime, true);
    Mat temp;
    resize(src, temp, Size(this->inpWidth, this->inpHeight));
    this->input_image_.resize(this->inpWidth * this->inpHeight * temp.channels());
    for (int c = 0; c < 3; c++)
    {
        for (int i = 0; i < this->inpHeight; i++)
        {
            for (int j = 0; j < this->inpWidth; j++)
            {
                float pix = temp.ptr<uchar>(i)[j * 3 + 2 - c];
                this->input_image_[c * this->inpHeight * this->inpWidth + i * this->inpWidth + j] = pix;
            }
        }
    }
    array<int64_t, 4> input_shape_{ 1, 3, this->inpHeight, this->inpWidth };

    auto allocator_info = MemoryInfo::CreateCpu(OrtDeviceAllocator, OrtMemTypeCPU);
    Value input_tensor_ = Value::CreateTensor<float>(allocator_info, input_image_.data(), input_image_.size(), input_shape_.data(), input_shape_.size());
    preprocessTimer.stop();

    Timer inferenceTimer = Timer(inferenceTime, true);
    vector<Value> ort_outputs = this -> forward(input_tensor_);
    inferenceTimer.stop();

    Timer postprocessTimer = Timer(postprocessTime, true);

    float* pred = ort_outputs[0].GetTensorMutableData<float>();
    dst = Mat(outHeight, outWidth, CV_32FC1, pred);
    dst *= 255;
    dst.convertTo(dst, CV_8UC1);
    resize(dst, dst, Size(src.cols, src.rows));
    postprocessTimer.stop();

    std::cout << std::fixed << std::setprecision(1);
    std::cout << "image: " << src.rows << "x" << src.cols <<" "<< (preprocessTime + inferenceTime + postprocessTime) * 1000.0 << "ms" << std::endl;
    std::cout << "speed: " << (preprocessTime * 1000.0) << "ms preprocess, ";
    std::cout << (inferenceTime * 1000.0) << "ms inference, ";
    std::cout << (postprocessTime * 1000.0) << "ms postprocess per image " << std::endl;
}