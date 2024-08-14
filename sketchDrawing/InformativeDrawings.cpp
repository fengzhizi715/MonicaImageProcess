//
// Created by Tony on 2024/8/13.
//

#include "InformativeDrawings.h"

InformativeDrawings::InformativeDrawings(string model_path): BaseOnnxRuntime(model_path)
{
    this->inpHeight = input_node_dims[0][2];
    this->inpWidth = input_node_dims[0][3];
    this->outHeight = output_node_dims[0][2];
    this->outWidth = output_node_dims[0][3];
}

Mat InformativeDrawings::detect(Mat& srcimg)
{
    Mat dstimg;
    resize(srcimg, dstimg, Size(this->inpWidth, this->inpHeight));
    this->input_image_.resize(this->inpWidth * this->inpHeight * dstimg.channels());
    for (int c = 0; c < 3; c++)
    {
        for (int i = 0; i < this->inpHeight; i++)
        {
            for (int j = 0; j < this->inpWidth; j++)
            {
                float pix = dstimg.ptr<uchar>(i)[j * 3 + 2 - c];
                this->input_image_[c * this->inpHeight * this->inpWidth + i * this->inpWidth + j] = pix;
            }
        }
    }
    array<int64_t, 4> input_shape_{ 1, 3, this->inpHeight, this->inpWidth };

    auto allocator_info = MemoryInfo::CreateCpu(OrtDeviceAllocator, OrtMemTypeCPU);
    Value input_tensor_ = Value::CreateTensor<float>(allocator_info, input_image_.data(), input_image_.size(), input_shape_.data(), input_shape_.size());

    vector<Value> ort_outputs = ort_session->Run(RunOptions{ nullptr }, &input_names[0], &input_tensor_, 1, output_names.data(), output_names.size());
    float* pred = ort_outputs[0].GetTensorMutableData<float>();
    Mat result(outHeight, outWidth, CV_32FC1, pred);
    result *= 255;
    result.convertTo(result, CV_8UC1);
    resize(result, result, Size(srcimg.cols, srcimg.rows));
    return result;
}