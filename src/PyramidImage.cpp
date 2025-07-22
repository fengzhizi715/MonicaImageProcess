//
// Created by Tony on 2025/7/20.
//
#include "../include/PyramidImage.h"
#include <thread>
#include <algorithm>
#include <iostream>  // 可用于调试日志

PyramidImage::PyramidImage(const cv::Mat& image, int levels)
        : originalImage(image.clone()), numLevels(std::max(1, levels)) {
    buildPyramidAsync();
}

void PyramidImage::updateImage(const cv::Mat& newImage) {
    {
        std::lock_guard<std::mutex> lock(pyramidMutex);
        originalImage = newImage.clone();
    }
    buildPyramidAsync();
}

void PyramidImage::waitForPyramid() const {
    if (pyramidReadyFuture.valid()) {
        pyramidReadyFuture.wait();
    }
}

bool PyramidImage::isPyramidReady() const {
    return pyramidReadyFuture.valid() &&
           pyramidReadyFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
}

cv::Mat PyramidImage::getOriginal() const {
    std::lock_guard<std::mutex> lock(pyramidMutex);
    return originalImage.clone();
}

cv::Mat PyramidImage::getLevel(int level) const {
    waitForPyramid();
    std::lock_guard<std::mutex> lock(pyramidMutex);
    if (level < 0 || level >= static_cast<int>(pyramid.size()))
        return cv::Mat();
    return pyramid[level].clone();
}

cv::Mat PyramidImage::getPreview() const {
    waitForPyramid();
    std::lock_guard<std::mutex> lock(pyramidMutex);
    if (pyramid.empty()) return cv::Mat();
    return pyramid[std::min(1, static_cast<int>(pyramid.size() - 1))].clone();
}

int PyramidImage::getLevelCount() const {
    waitForPyramid();
    std::lock_guard<std::mutex> lock(pyramidMutex);
    return static_cast<int>(pyramid.size());
}

int PyramidImage::computeValidLevels(const cv::Mat& image, int maxLevel) const {
    int w = image.cols;
    int h = image.rows;
    int levels = 1;
    while (w > 64 && h > 64 && levels < maxLevel) {
        w /= 2;
        h /= 2;
        levels++;
    }
    return levels;
}

void PyramidImage::buildPyramidAsync() {
    std::lock_guard<std::mutex> lock(pyramidMutex);

    if (isBuilding.exchange(true)) return;

    pyramidReadyPromise = std::make_shared<std::promise<void>>();
    pyramidReadyFuture = pyramidReadyPromise->get_future().share();

    cv::Mat base = originalImage.clone();
    const int levels = computeValidLevels(base, numLevels);

    std::thread([this, base, levels]() {
        std::vector<cv::Mat> levelsVec(levels);

        try {
            if (base.empty()) {
                std::cerr << "[Pyramid] base image empty\n";
                pyramidReadyPromise->set_value();
                isBuilding = false;
                return;
            }

            levelsVec[0] = base;

            for (int i = 1; i < levels; ++i) {
                cv::Mat down;
                cv::pyrDown(levelsVec[i - 1], down);

                if (down.empty() || down.cols < 4 || down.rows < 4) {
                    break;
                }

                levelsVec[i] = down;
            }

            {
                std::lock_guard<std::mutex> lock(pyramidMutex);
                pyramid = std::move(levelsVec);
            }
        } catch (const std::exception& e) {
            std::cerr << "[Pyramid] Exception: " << e.what() << std::endl;
        } catch (...) {
            std::cerr << "[Pyramid] Unknown exception\n";
        }

        try {
            pyramidReadyPromise->set_value();
        } catch (...) {
            // 防止重复 set_value 抛异常
        }

        isBuilding = false;
    }).detach();
}

cv::Mat PyramidImage::downsample(const cv::Mat& input) {
    cv::Mat output;
    cv::pyrDown(input, output);
    return output;
}