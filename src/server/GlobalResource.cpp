//
// Created by Tony on 2025/3/25.
//
#include <iostream>
#include "GlobalResource.h"
#include "../../include/Constants.h"

GlobalResource::GlobalResource(string modelPath): modelPath(modelPath) {

    string sketchDrawingMode = modelPath + "/opensketch_style_512x512.onnx";
    const string& onnx_logid = "Sketch Drawing";
    const string& onnx_provider = OnnxProviders::CPU;
    sketchDrawing = std::make_unique<SketchDrawing>(sketchDrawingMode, onnx_logid.c_str(), onnx_provider.c_str());

    // 初始化资源，比如加载模型、配置文件等
    cout << "GlobalResource initialized." << endl;
}

Mat GlobalResource::processSketchDrawing(Mat src) {
    cout << "process SketchDrawing..." << endl;

    Mat dst;
    sketchDrawing.get()->inferImage(src, dst);
    return dst;
}