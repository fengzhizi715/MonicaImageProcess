//
// Created by Tony on 2025/7/20.
//
#include "../include/PyramidImage.h"
#include <thread>
#include <future>
#include <algorithm>

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
    std::lock_guard<std::mutex> lock(pyramidMutex);
    if (level < 0 || level >= pyramid.size()) return cv::Mat();
    return pyramid[level].clone();
}

int PyramidImage::getLevelCount() const {
    std::lock_guard<std::mutex> lock(pyramidMutex);
    return static_cast<int>(pyramid.size());
}

cv::Mat PyramidImage::downsample(const cv::Mat& input) {
    cv::Mat output;
    cv::pyrDown(input, output);
    return output;
}

void PyramidImage::buildPyramidAsync() {
    std::vector<std::future<cv::Mat>> futures;
    std::vector<cv::Mat> levels(numLevels);

    levels[0] = originalImage.clone();

    for (int i = 1; i < numLevels; ++i) {
        futures.push_back(std::async(std::launch::async, [prev = levels[i - 1]]() {
            cv::Mat result;
            cv::pyrDown(prev, result);
            return result;
        }));
    }

    for (int i = 1; i < numLevels; ++i) {
        levels[i] = futures[i - 1].get();
    }

    std::lock_guard<std::mutex> lock(pyramidMutex);
    pyramid = std::move(levels);
}