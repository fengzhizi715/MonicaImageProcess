//
// Created by Tony on 2025/7/20.
//
#include "../include/PyramidImage.h"

PyramidImage::PyramidImage(const cv::Mat& decodedImage)
        : originalImage(decodedImage.clone()) {
    buildPyramidIfNeeded();
}

PyramidImage::PyramidImage(const cv::Mat& decodedImage, const std::string& path, const std::string& fmt)
        : originalImage(decodedImage.clone()), sourcePath(path), format(fmt) {
    buildPyramidIfNeeded();
}

const cv::Mat& PyramidImage::getOriginal() const {
    return originalImage;
}

const cv::Mat& PyramidImage::getPyramidLevel(int level) {
    buildPyramidIfNeeded();
    if (level < 0 || level >= static_cast<int>(pyramidImages.size())) {
        throw std::out_of_range("Invalid pyramid level");
    }
    return pyramidImages[level];
}

const cv::Mat& PyramidImage::getPreviewImage() {
    buildPyramidIfNeeded();
    int previewLevel = std::min(2, static_cast<int>(pyramidImages.size() - 1));
    return pyramidImages[previewLevel];
}

int PyramidImage::getPyramidLevelCount() const {
    return static_cast<int>(pyramidImages.size());
}

void PyramidImage::rebuildPyramid(int levels) {
    maxLevels = levels;
    pyramidImages.clear();
    buildPyramidIfNeeded();
}

const std::string& PyramidImage::getSourcePath() const {
    return sourcePath;
}

const std::string& PyramidImage::getFormat() const {
    return format;
}

void PyramidImage::buildPyramidIfNeeded() {
    if (!pyramidImages.empty()) return;

    cv::Mat current = originalImage;
    pyramidImages.push_back(current); // Level 0
    for (int i = 1; i < maxLevels; ++i) {
        cv::Mat down;
        cv::pyrDown(current, down);
        pyramidImages.push_back(down);
        current = down;
    }
}