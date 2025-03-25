//
// Created by Tony on 2024/8/12.
//

#include <iostream>
#include "../../include/common/OnnxRuntimeBase.h"
#include "../../include/Constants.h"

using namespace cv;
using namespace std;
using namespace Ort;


OnnxRuntimeBase::OnnxRuntimeBase(string modelPath, const char* logId, const char* provider)
{
    sessionOptions.SetGraphOptimizationLevel(ORT_ENABLE_BASIC);

    env = Ort::Env(ORT_LOGGING_LEVEL_WARNING, logId);

    std::vector<std::string> availableProviders = Ort::GetAvailableProviders();
    auto cudaAvailable = std::find(availableProviders.begin(), availableProviders.end(), "CUDAExecutionProvider");
    OrtCUDAProviderOptions cudaOption;
    if (provider == OnnxProviders::CUDA.c_str()) {
        if (cudaAvailable == availableProviders.end()) {
            std::cout << "CUDA is not supported by your ONNXRuntime build. Fallback to CPU." << std::endl;
            std::cout << "Inference device: CPU" << std::endl;
        }
        else {
            std::cout << "Inference device: GPU" << std::endl;
            sessionOptions.AppendExecutionProvider_CUDA(cudaOption);
        }
    }
    else if (provider == OnnxProviders::CPU.c_str()) {
        // "cpu" by default
        std::cout << "Inference device: CPU" << std::endl;
    }
    else
    {
        throw std::runtime_error("NotImplemented provider=" + std::string(provider));
    }

    #ifdef _WIN32
        auto modelPathW = get_win_path(modelPath);  // For Windows (wstring)
        ort_session = Ort::Session(env, modelPathW.c_str(), sessionOptions);
    #else
        ort_session = Ort::Session(env, modelPath.c_str(), sessionOptions);  // For Linux、MacOS (string)
    #endif

    size_t numInputNodes = ort_session.GetInputCount();
    size_t numOutputNodes = ort_session.GetOutputCount();
    AllocatorWithDefaultOptions allocator;

    for (int i = 0; i < numInputNodes; i++)
    {
        input_names.push_back(ort_session.GetInputName(i, allocator)); // 低版本onnxruntime的接口函数
        // auto input_name = ort_session->GetInputNameAllocated(i, allocator);  /// 高版本onnxruntime的接口函数
        // input_names.push_back(input_name.get()); /// 高版本onnxruntime的接口函数
        Ort::TypeInfo input_type_info = ort_session.GetInputTypeInfo(i);
        auto input_tensor_info = input_type_info.GetTensorTypeAndShapeInfo();
        auto input_dims = input_tensor_info.GetShape();
        input_node_dims.push_back(input_dims);
    }
    for (int i = 0; i < numOutputNodes; i++)
    {
        output_names.push_back(ort_session.GetOutputName(i, allocator)); // 低版本onnxruntime的接口函数
        // auto output_name = ort_session->GetOutputNameAllocated(i, allocator);
        // output_names.push_back(output_name.get()); /// 高版本onnxruntime的接口函数
        Ort::TypeInfo output_type_info = ort_session.GetOutputTypeInfo(i);
        auto output_tensor_info = output_type_info.GetTensorTypeAndShapeInfo();
        auto output_dims = output_tensor_info.GetShape();
        output_node_dims.push_back(output_dims);
    }
}

OnnxRuntimeBase::~OnnxRuntimeBase() {
    sessionOptions.release();
    ort_session.release();
}

std::vector<Ort::Value> OnnxRuntimeBase::forward(Ort::Value& inputTensors)
{
    return ort_session.Run(RunOptions{ nullptr }, input_names.data(), &inputTensors, 1, output_names.data(), output_names.size());
}