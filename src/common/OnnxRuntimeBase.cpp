//
// Created by Tony on 2024/8/12.
//

#include "../../include/common/OnnxRuntimeBase.h"

using namespace cv;
using namespace std;
using namespace Ort;


OnnxRuntimeBase::OnnxRuntimeBase(string modelPath, const char* logId)
{
    /// OrtStatus* status = OrtSessionOptionsAppendExecutionProvider_CUDA(sessionOptions, 0);   ///如果使用cuda加速，需要取消注释

    sessionOptions.SetGraphOptimizationLevel(ORT_ENABLE_BASIC);

    env = Ort::Env(ORT_LOGGING_LEVEL_WARNING, logId);

    const char* model_path = "";
    #ifdef _WIN32
        auto modelPathW = get_win_path(modelPath);  // For Windows (wstring)
        model_path = modelPathW.c_str();
    #else
        model_path = modelPath.c_str();             // For Linux、MacOS (string)
    #endif

    ort_session = new Session(env, model_path, sessionOptions);

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

OnnxRuntimeBase::~OnnxRuntimeBase() {
    sessionOptions.release();
    ort_session -> release();
}
