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
    buildPyramidAsync();  // 自动重建
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
    return pyramid[std::min(1, static_cast<int>(pyramid.size()) - 1)].clone();
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

    if (isBuilding.exchange(true)) return; // 🔒防止重复构建

    pyramidReadyPromise = std::make_shared<std::promise<void>>();
    pyramidReadyFuture = pyramidReadyPromise->get_future().share();

    cv::Mat base = originalImage.clone();
    const int levels = computeValidLevels(base, numLevels);  // 🧠防止空图

    std::thread([this, base, levels]() {
        std::vector<cv::Mat> levelsVec(levels);

        if (base.empty()) {
            isBuilding = false;
            pyramidReadyPromise->set_value();
            return;
        }

        levelsVec[0] = base;

        for (int i = 1; i < levels; ++i) {
            cv::Mat down;
            cv::pyrDown(levelsVec[i - 1], down);
            if (down.empty() || down.cols < 4 || down.rows < 4) break;
            levelsVec[i] = down;
        }

        {
            std::lock_guard<std::mutex> lock(pyramidMutex);
            pyramid = std::move(levelsVec);
        }

        isBuilding = false;
        pyramidReadyPromise->set_value();
    }).detach();
}

cv::Mat PyramidImage::downsample(const cv::Mat& input) {
    cv::Mat output;
    cv::pyrDown(input, output);
    return output;
}