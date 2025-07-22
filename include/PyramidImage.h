//
// Created by Tony on 2025/7/19.
//

#ifndef MONICAIMAGEPROCESS_PYRAMIDIMAGE_H
#define MONICAIMAGEPROCESS_PYRAMIDIMAGE_H

#pragma once

#include <opencv2/opencv.hpp>
#include <vector>
#include <memory>
#include <mutex>
#include <future>
#include <atomic>

class PyramidImage {
public:
    // 从解码后的图像构造（可用原图或预览图）
    explicit PyramidImage(const cv::Mat& image, int levels = 4);

    void waitForPyramid() const;

    bool isPyramidReady() const;

    // 更新原图（如解码完成后替换预览）
    void updateImage(const cv::Mat& newImage);

    // 获取原图
    cv::Mat getOriginal() const;

    // 获取指定层级（0 表示原图，levels-1 为最小图）
    cv::Mat getLevel(int level) const;

    // 获取预览图（默认返回第一层，非最后一层）
    cv::Mat getPreview() const;

    // 获取 pyramid 层级总数
    int getLevelCount() const;

private:
    void buildPyramidAsync();
    int computeValidLevels(const cv::Mat& image, int maxLevel) const;
    cv::Mat downsample(const cv::Mat& input);

    cv::Mat originalImage;
    std::vector<cv::Mat> pyramid;
    int numLevels;

    mutable std::mutex pyramidMutex;
    mutable std::shared_future<void> pyramidReadyFuture;
    mutable std::shared_ptr<std::promise<void>> pyramidReadyPromise;

    std::atomic<bool> isBuilding{false};
};

#endif //MONICAIMAGEPROCESS_PYRAMIDIMAGE_H
