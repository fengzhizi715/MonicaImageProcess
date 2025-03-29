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

    string yolov8FaceModelPath = modelPath + "/yoloface_8n.onnx";
    string face68LandmarksModePath = modelPath + "/2dfan4.onnx";
    string faceEmbeddingModePath = modelPath + "/arcface_w600k_r50.onnx";
    string faceSwapModePath = modelPath + "/inswapper_128.onnx";
    string faceSwapModePath2 = modelPath + "/model_matrix.bin";
    string faceEnhanceModePath = modelPath + "/gfpgan_1.4.onnx";

    const std::string& yolov8FaceLogId = "yolov8Face";
    const std::string& face68LandmarksLogId = "face68Landmarks";
    const std::string& faceEmbeddingLogId = "faceEmbedding";
    const std::string& faceSwapLogId = "faceSwap";
    const std::string& faceEnhanceLogId = "faceEnhance";

    yolov8Face      = std::make_unique<Yolov8Face>(yolov8FaceModelPath, yolov8FaceLogId.c_str(), onnx_provider.c_str());
    face68Landmarks = std::make_unique<Face68Landmarks>(face68LandmarksModePath, face68LandmarksLogId.c_str(), onnx_provider.c_str());
    faceEmbedding   = std::make_unique<FaceEmbedding>(faceEmbeddingModePath, faceEmbeddingLogId.c_str(), onnx_provider.c_str());
    faceSwap        = std::make_unique<FaceSwap>(faceSwapModePath, faceSwapModePath2, faceSwapLogId.c_str(), onnx_provider.c_str());
    faceEnhance     = std::make_unique<FaceEnhance>(faceEnhanceModePath, faceEnhanceLogId.c_str(), onnx_provider.c_str());

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

Mat GlobalResource::processFaceLandMark(Mat src) {
    cout << "process FaceDetect..." << endl;

    Mat dst;

    cv::Scalar scalar(0, 0, 255);
    try {
        vector<Bbox> boxes;
        yolov8Face.get()->detect(src, boxes);
        dst = src.clone();
        for (auto box: boxes) {rectangle(dst, cv::Point(box.xmin,box.ymin), cv::Point(box.xmax,box.ymax), scalar, 4, 8, 0);

           vector<Point2f> face_landmark_5of68;
           face68Landmarks.get()->detect(src, box, face_landmark_5of68);
           for (auto point : face_landmark_5of68)
           {
               circle(dst, cv::Point(point.x, point.y), 4, scalar, -1);
           }
        }
    } catch(...) {
    }
    return dst;
}