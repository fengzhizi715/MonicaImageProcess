//
// Created by Tony on 2024/8/12.
//

#ifndef MONICAIMAGEPROCESS_ONNXRUNTIMEBASE_H
#define MONICAIMAGEPROCESS_ONNXRUNTIMEBASE_H

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <vector>
#include <onnxruntime_cxx_api.h>
#include <onnxruntime_c_api.h>

class OnnxRuntimeBase
{

public:
    OnnxRuntimeBase(std::string modelPath, const char* logId, const char* provider);
    ~OnnxRuntimeBase();

    virtual std::vector<Ort::Value> forward(Ort::Value& inputTensors);

protected:
    Ort::Env env;
    Ort::Session ort_session{ nullptr };
    Ort::SessionOptions sessionOptions = Ort::SessionOptions();
    std::vector<char*> input_names;
    std::vector<char*> output_names;
    std::vector<std::vector<int64_t>> input_node_dims;  // >=1 outputs
    std::vector<std::vector<int64_t>> output_node_dims; // >=1 outputs

    Ort::MemoryInfo memory_info_handler = Ort::MemoryInfo::CreateCpu(OrtDeviceAllocator, OrtMemTypeCPU);
};


#endif //MONICAIMAGEPROCESS_ONNXRUNTIMEBASE_H
