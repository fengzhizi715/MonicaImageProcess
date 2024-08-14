//
// Created by Tony on 2024/8/12.
//

#include "BaseOnnxRuntime.h"

using namespace cv;
using namespace std;
using namespace Ort;


BaseOnnxRuntime::BaseOnnxRuntime(string model_path)
{
    /// OrtStatus* status = OrtSessionOptionsAppendExecutionProvider_CUDA(sessionOptions, 0);   ///如果使用cuda加速，需要取消注释

    sessionOptions.SetGraphOptimizationLevel(ORT_ENABLE_BASIC);
    /// std::wstring widestr = std::wstring(model_path.begin(), model_path.end());  ////windows写法
    /// ort_session = new Session(env, widestr.c_str(), sessionOptions); ////windows写法
    ort_session = new Session(env, model_path.c_str(), sessionOptions); ////linux写法

    size_t numInputNodes = ort_session->GetInputCount();
    size_t numOutputNodes = ort_session->GetOutputCount();
    AllocatorWithDefaultOptions allocator;

    for (int i = 0; i < numInputNodes; i++)
    {
        input_names.push_back(ort_session->GetInputName(i, allocator)); // 低版本onnxruntime的接口函数
        // auto input_name = ort_session->GetInputNameAllocated(i, allocator);  /// 高版本onnxruntime的接口函数
        // input_names.push_back(input_name.get()); /// 高版本onnxruntime的接口函数
        Ort::TypeInfo input_type_info = ort_session->GetInputTypeInfo(i);
        auto input_tensor_info = input_type_info.GetTensorTypeAndShapeInfo();
        auto input_dims = input_tensor_info.GetShape();
        input_node_dims.push_back(input_dims);
    }
    for (int i = 0; i < numOutputNodes; i++)
    {
        output_names.push_back(ort_session->GetOutputName(i, allocator)); // 低版本onnxruntime的接口函数
        // auto output_name = ort_session->GetOutputNameAllocated(i, allocator);
        // output_names.push_back(output_name.get()); /// 高版本onnxruntime的接口函数
        Ort::TypeInfo output_type_info = ort_session->GetOutputTypeInfo(i);
        auto output_tensor_info = output_type_info.GetTensorTypeAndShapeInfo();
        auto output_dims = output_tensor_info.GetShape();
        output_node_dims.push_back(output_dims);
    }
}

BaseOnnxRuntime::~BaseOnnxRuntime() {
    sessionOptions.release();
    ort_session -> release();
}
