//
// Created by Tony on 2024/7/26.
//

#ifndef MONICAIMAGEPROCESS_FACEDETECT_H
#define MONICAIMAGEPROCESS_FACEDETECT_H

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/dnn.hpp>
#include <tuple>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <iterator>

using namespace cv;
using namespace cv::dnn;
using namespace std;

class FaceDetect {

public:
    FaceDetect(string faceProto,string faceModel,string ageProto,string ageModel,string genderProto,string genderModel);

    void inferImage(Mat& src, Mat& dst);

private:
    tuple<Mat, vector<vector<int>>> getFaceBox(Net net, Mat &frame, double conf_threshold);

private:
    Net ageNet;
    Net genderNet;
    Net faceNet;

    vector<string> ageList;
    vector<string> genderList;
    Scalar MODEL_MEAN_VALUES;
};


#endif //MONICAIMAGEPROCESS_FACEDETECT_H
