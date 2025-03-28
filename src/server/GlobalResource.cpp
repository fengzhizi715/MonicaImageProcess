//
// Created by Tony on 2025/3/25.
//

#include <vector>
#include <string>
#include <iostream>
#include "GlobalResource.h"
#include "../../include/Constants.h"

GlobalResource::GlobalResource(string modelPath): modelPath(modelPath) {

    string sketchDrawingMode = modelPath + "/opensketch_style_512x512.onnx";
    const string& onnx_logid = "Sketch Drawing";
    const string& onnx_provider = OnnxProviders::CPU;
    sketchDrawing = std::make_unique<SketchDrawing>(sketchDrawingMode, onnx_logid.c_str(), onnx_provider.c_str());

    string faceProto = modelPath + "/opencv_face_detector.pbtxt";
    string faceModel = modelPath + "/opencv_face_detector_uint8.pb";
    string ageProto  = modelPath + "/age_deploy.prototxt";
    string ageModel  = modelPath + "/age_net.caffemodel";
    string genderProto = modelPath + "/gender_deploy.prototxt";
    string genderModel = modelPath + "/gender_net.caffemodel";

    faceDetect = std::make_unique<FaceDetect>(faceProto,faceModel,ageProto,ageModel,genderProto,genderModel);

    // 初始化资源，加载模型文件
    cout << "GlobalResource initialized." << endl;
}

Mat GlobalResource::processSketchDrawing(Mat src) {
    cout << "process SketchDrawing..." << endl;

    Mat dst;
    sketchDrawing.get()->inferImage(src, dst);
    cvtColor(dst, dst, cv::COLOR_GRAY2BGR);
    return dst;
}

Mat GlobalResource::processFaceDetect(Mat src) {
    cout << "process FaceDetect..." << endl;

    Mat dst;
    faceDetect.get()->inferImage(src, dst);
    cvtColor(dst, dst, cv::COLOR_GRAY2BGR);
    return dst;
}