//
// Created by Tony on 2025/1/11.
//

#ifndef MONICAIMAGEPROCESS_MATCHTEMPLATE_H
#define MONICAIMAGEPROCESS_MATCHTEMPLATE_H

#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <vector>
#include <future>
#include <mutex>
#include <iostream>

using namespace std;
using namespace cv;

class MatchTemplate {

public:
    Mat templateMatching(Mat& image, Mat& templateImage,
                             double angleStart, double angleEnd, double angleStep,
                             double scaleStart, double scaleEnd, double scaleStep,
                             double matchTemplateThreshold,  float scoreThreshold,float nmsThreshold);

private:
    // 使用 Canny 边缘检测
    Mat computeCanny(const cv::Mat& image, double threshold1, double threshold2);

    // 处理单个角度和尺度
    static void processAngleScale(const cv::Mat& inputEdges, const cv::Mat& templateEdges, double angle, double scale,
                                  double threshold, std::mutex& resultMutex, std::vector<cv::Rect>& results, std::vector<float>& scores);

    // 并行化的模板匹配
    void parallelTemplateMatching(const cv::Mat& inputEdges, const cv::Mat& templateEdges,
                                  double angleStart, double angleEnd, double angleStep,
                                  double scaleStart, double scaleEnd, double scaleStep,
                                  double threshold, std::vector<cv::Rect>& matches, std::vector<float>& scores);

    // 使用 OpenCV 的 NMS
    void applyNMS(const std::vector<cv::Rect>& boxes, const std::vector<float>& scores, std::vector<cv::Rect>& finalBoxes,
                  float scoreThreshold, float nmsThreshold);
};

#endif //MONICAIMAGEPROCESS_MATCHTEMPLATE_H
