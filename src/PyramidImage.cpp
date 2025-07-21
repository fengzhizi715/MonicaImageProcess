//
// Created by Tony on 2025/7/20.
//
#include "../include/PyramidImage.h"
#include <thread>
#include <future>
#include <algorithm>
#include <stdexcept>
#include <condition_variable>

PyramidImage::PyramidImage(const cv::Mat& image, int levels)
        : originalImage(image.clone()), numLevels(std::max(1, levels)) {
    buildPyramidAsync();
}

void PyramidImage::updateImage(const cv::Mat& newImage) {
    std::lock_guard<std::mutex> lock(pyramidMutex);
    originalImage = newImage.clone();
    buildPyramidAsync();
}

cv::Mat PyramidImage::getOriginal() const {
    std::lock_guard<std::mutex> lock(pyramidMutex);
    return originalImage.clone();
}

cv::Mat PyramidImage::getLevel(int level) const {
    waitForPyramid();  // 等待构建完成

    std::lock_guard<std::mutex> lock(pyramidMutex);
    if (level < 0 || level >= static_cast<int>(pyramid.size()))
        return cv::Mat();
    return pyramid[level].clone();
}

int PyramidImage::getLevelCount() const {
    waitForPyramid();
    std::lock_guard<std::mutex> lock(pyramidMutex);
    return static_cast<int>(pyramid.size());
}

cv::Mat PyramidImage::getPreview() const {
    waitForPyramid();

    std::lock_guard<std::mutex> lock(pyramidMutex);
    if (pyramid.empty()) return cv::Mat();
    return pyramid[std::min(1, static_cast<int>(pyramid.size() - 1))].clone();
}

void PyramidImage::buildPyramidAsync() {
    std::lock_guard<std::mutex> lock(pyramidMutex);

    pyramidReadyPromise = std::make_shared<std::promise<void>>();
    pyramidReadyFuture = pyramidReadyPromise->get_future().share();

    cv::Mat base = originalImage.clone();
    int levels = numLevels;

    std::thread([this, base, levels]() {
        std::vector<cv::Mat> levelsVec(levels);
        std::vector<std::future<cv::Mat>> futures;

        if (base.empty()) {
            // 防止原图为空导致异常
            pyramidReadyPromise->set_value();
            return;
        }

        levelsVec[0] = base;

        for (int i = 1; i < levels; ++i) {
            cv::Mat prev = levelsVec[i - 1];
            futures.push_back(std::async(std::launch::async, [prev]() {
                if (prev.empty()) return cv::Mat();
                cv::Mat down;
                cv::pyrDown(prev, down);
                return down;
            }));
        }

        for (int i = 1; i < levels; ++i) {
            levelsVec[i] = futures[i - 1].get();
            if (levelsVec[i].empty()) break;  // 停止构建更低层
        }

        {
            std::lock_guard<std::mutex> lock(pyramidMutex);
            pyramid = std::move(levelsVec);
        }

        pyramidReadyPromise->set_value();
    }).detach();
}

void PyramidImage::waitForPyramid() const {
    if (pyramidReadyFuture.valid()) {
        pyramidReadyFuture.wait();
    }
}

cv::Mat PyramidImage::downsample(const cv::Mat& input) {
    cv::Mat output;
    cv::pyrDown(input, output);
    return output;
}