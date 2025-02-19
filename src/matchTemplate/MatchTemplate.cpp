//
// Created by Tony on 2025/1/12.
//
#include "../../include/matchTemplate/MatchTemplate.h"

using namespace cv::dnn;

// 使用 Canny 边缘检测
cv::Mat MatchTemplate::computeCanny(const cv::Mat& image, double threshold1 = 50, double threshold2 = 150) {

    cv::Mat gray, edges;
    if (image.channels() == 3) {
        cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
    } else {
        gray = image.clone();
    }

    cv::Canny(gray, edges, threshold1, threshold2);
    CV_Assert(edges.type() == CV_8U); // 确保输出为单通道图像
    return edges;
}

// 处理旋转 + 缩放
void MatchTemplate::processAngleScale(const cv::Mat& inputEdges, const cv::Mat& templateEdges, double angle, double scale,
                              double threshold, std::mutex& resultMutex, std::vector<cv::Rect>& results, std::vector<float>& scores) {

    // 旋转模板
    cv::Mat rotatedTemplate;
    cv::Point2f center(templateEdges.cols / 2.0f, templateEdges.rows / 2.0f);
    cv::Mat rotationMatrix = cv::getRotationMatrix2D(center, angle, 1.0);

    // 计算旋转后外接矩形尺寸
    cv::Rect bbox = cv::RotatedRect(center, templateEdges.size(), angle).boundingRect();
    rotationMatrix.at<double>(0, 2) += bbox.width / 2.0 - center.x;
    rotationMatrix.at<double>(1, 2) += bbox.height / 2.0 - center.y;

    cv::warpAffine(templateEdges, rotatedTemplate, rotationMatrix, bbox.size(), cv::INTER_LINEAR, BORDER_CONSTANT, Scalar(0));

    // 缩放模板
    cv::Mat scaledTemplate;
    cv::resize(rotatedTemplate, scaledTemplate, cv::Size(), scale, scale, scale < 1 ? cv::INTER_AREA : cv::INTER_LINEAR);

    // 边界检查
    if (scaledTemplate.empty() || scaledTemplate.cols < 1 || scaledTemplate.rows < 1 ||
        scaledTemplate.cols > inputEdges.cols || scaledTemplate.rows > inputEdges.rows) {
        return;
    }

    // 模板匹配
    cv::Mat result;
    try {
        cv::matchTemplate(inputEdges, scaledTemplate, result, cv::TM_CCOEFF_NORMED);
    } catch (const cv::Exception& e) {
        std::cerr << "Error in matchTemplate: " << e.what() << std::endl;
        return;
    }

    if (result.empty()) return;

    // 记录满足阈值的匹配结果
    for (int y = 0; y < result.rows; ++y) {
        for (int x = 0; x < result.cols; ++x) {
            float matchScore = result.at<float>(y, x);
            if (matchScore >= threshold) {
                std::lock_guard<std::mutex> lock(resultMutex);
                results.emplace_back(cv::Rect(x, y, scaledTemplate.cols, scaledTemplate.rows));
                scores.push_back(matchScore);
            }
        }
    }
}

// 并行化的模板匹配
void MatchTemplate::parallelTemplateMatching(const cv::Mat& inputEdges, const cv::Mat& templateEdges,
                              double angleStart, double angleEnd, double angleStep,
                              double scaleStart, double scaleEnd, double scaleStep,
                              double threshold, std::vector<cv::Rect>& matches, std::vector<float>& scores) {
    std::mutex resultMutex;
    std::vector<std::thread> threads;
    std::condition_variable cv;
    std::mutex cvMutex;
    int activeThreads = 0;
    const int maxThreads = std::thread::hardware_concurrency();

    for (double angle = angleStart; angle <= angleEnd; angle += angleStep) {
        for (double scale = scaleStart; scale <= scaleEnd; scale += scaleStep) {
            {
                std::unique_lock<std::mutex> lock(cvMutex);
                cv.wait(lock, [&] { return activeThreads < maxThreads; });
                activeThreads++;
            }

            threads.emplace_back([&, angle, scale] {
                processAngleScale(inputEdges, templateEdges, angle, scale, threshold, resultMutex, matches, scores);
                {
                    std::lock_guard<std::mutex> lock(cvMutex);
                    activeThreads--;
                }
                cv.notify_one();
            });
        }
    }

    for (auto& thread : threads) {
        thread.join();
    }
}

// 使用 OpenCV 的 NMS
void MatchTemplate::applyNMS(const std::vector<cv::Rect>& boxes, const std::vector<float>& scores,
                             std::vector<cv::Rect>& finalBoxes,
                             float scoreThreshold, float nmsThreshold) {
    if (boxes.empty() || scores.empty() || boxes.size() != scores.size()) {
        return;
    }

    std::vector<int> indices;
    NMSBoxes(boxes, scores, scoreThreshold, nmsThreshold, indices);
    for (int idx : indices) {
        finalBoxes.push_back(boxes[idx]);
    }
}

Mat MatchTemplate::templateMatching(Mat image, Mat templateImage, int matchType,
                                    double angleStart, double angleEnd, double angleStep,
                                    double scaleStart, double scaleEnd, double scaleStep,
                                    double matchTemplateThreshold,  float scoreThreshold, float nmsThreshold, Scalar scalar) {

    // 绘制最终结果
    cv::Mat resultImage = image.clone();

    if (matchType == 1) { // 灰度匹配
        cvtColor(image, image, COLOR_BGR2GRAY);
        cvtColor(templateImage, templateImage, COLOR_BGR2GRAY);
    } else if (matchType == 2) { // 边缘匹配
        // 计算图像和模板的 Canny 边缘
        image = computeCanny(image, 50, 150);
        templateImage = computeCanny(templateImage, 50, 150);
    }

    vector<Rect> matches;
    vector<float> scores;

    // 并行模板匹配
    parallelTemplateMatching(image, templateImage, angleStart, angleEnd, angleStep, scaleStart, scaleEnd, scaleStep, matchTemplateThreshold, matches, scores);

    // 使用 OpenCV 的 NMS 过滤结果
    vector<Rect> finalMatches;
    applyNMS(matches, scores, finalMatches, scoreThreshold, nmsThreshold);

    for (const auto& match : finalMatches) {
        rectangle(resultImage, match, scalar, 2);
    }

    return resultImage;
}