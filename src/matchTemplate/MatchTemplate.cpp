//
// Created by Tony on 2025/1/12.
//
#include "../../include/matchTemplate/MatchTemplate.h"

// 使用 Canny 边缘检测
cv::Mat MatchTemplate::computeCanny(const cv::Mat& image, double threshold1 = 50, double threshold2 = 150) {
    cv::Mat edges;
    cv::Canny(image, edges, threshold1, threshold2);
    CV_Assert(edges.type() == CV_8U); // 确保输出为单通道图像
    return edges;
}

// 处理单个角度和尺度
void MatchTemplate::processAngleScale(const cv::Mat& inputEdges, const cv::Mat& templateEdges, double angle, double scale,
                              double threshold, std::mutex& resultMutex, std::vector<cv::Rect>& results, std::vector<float>& scores) {
    // 旋转模板
    cv::Point2f center(templateEdges.cols / 2.0f, templateEdges.rows / 2.0f);
    cv::Mat rotationMatrix = cv::getRotationMatrix2D(center, angle, 1.0);
    cv::Mat rotatedTemplate;
    cv::warpAffine(templateEdges, rotatedTemplate, rotationMatrix, templateEdges.size(), cv::INTER_LINEAR);

    // 缩放模板
    cv::Mat scaledTemplate;
    cv::resize(rotatedTemplate, scaledTemplate, cv::Size(), scale, scale);

    // 检查模板有效性
    if (scaledTemplate.empty() || scaledTemplate.cols < 1 || scaledTemplate.rows < 1) {
        return; // 跳过无效模板
    }

    // 检查模板与输入图像尺寸
    if (scaledTemplate.cols > inputEdges.cols || scaledTemplate.rows > inputEdges.rows) {
        return; // 跳过尺寸不匹配的模板
    }

    // 边缘模板匹配
    cv::Mat result;
    try {
        cv::matchTemplate(inputEdges, scaledTemplate, result, cv::TM_CCOEFF_NORMED);
    } catch (const cv::Exception& e) {
        std::cerr << "Error in matchTemplate: " << e.what() << std::endl;
        return;
    }

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
    std::vector<std::future<void>> futures;

    for (double angle = angleStart; angle <= angleEnd; angle += angleStep) {
        for (double scale = scaleStart; scale <= scaleEnd; scale += scaleStep) {
            futures.emplace_back(std::async(std::launch::async, &MatchTemplate::processAngleScale,
                                            std::ref(inputEdges), std::ref(templateEdges),
                                            angle, scale, threshold, std::ref(resultMutex),
                                            std::ref(matches), std::ref(scores)));
        }
    }

    // 等待所有线程完成
    for (auto& future : futures) {
        future.get();
    }
}

// 使用 OpenCV 的 NMS
void MatchTemplate::applyNMS(const std::vector<cv::Rect>& boxes, const std::vector<float>& scores,
                             std::vector<cv::Rect>& finalBoxes,
                             float scoreThreshold, float nmsThreshold) {
    if (boxes.empty() || scores.empty()) {
        return; // 避免空输入导致的崩溃
    }

    std::vector<int> indices;
    cv::dnn::NMSBoxes(boxes, scores, scoreThreshold, nmsThreshold, indices);

    for (int idx : indices) {
        finalBoxes.push_back(boxes[idx]);
    }
}

Mat MatchTemplate::templateMatching(Mat& image, Mat& templateImage,
                                    double angleStart, double angleEnd, double angleStep,
                                    double scaleStart, double scaleEnd, double scaleStep,
                                    double matchTemplateThreshold,  float scoreThreshold,float nmsThreshold) {

    // 绘制最终结果
    cv::Mat resultImage = image.clone();

    // 计算图像和模板的 Canny 边缘
//    cv::Mat imageEdges = computeCanny(image, 50, 150);
//    cv::Mat templateEdges = computeCanny(templateImage, 50, 150);

    std::vector<cv::Rect> matches;
    std::vector<float> scores;

    // 并行模板匹配
    parallelTemplateMatching(image, templateImage, angleStart, angleEnd, angleStep, scaleStart, scaleEnd, scaleStep, matchTemplateThreshold, matches, scores);

    // 使用 OpenCV 的 NMS 过滤结果
    vector<Rect> finalMatches;
    applyNMS(matches, scores, finalMatches, scoreThreshold, nmsThreshold);

    for (const auto& match : finalMatches) {
        rectangle(resultImage, match, cv::Scalar(0, 0, 255), 2);
    }

    return resultImage;
}