//
// Created by Tony on 2025/7/19.
//

#ifndef MONICAIMAGEPROCESS_PYRAMIDIMAGE_H
#define MONICAIMAGEPROCESS_PYRAMIDIMAGE_H

#pragma once

#include <opencv2/opencv.hpp>
#include <vector>
#include <string>

class PyramidImage {
public:
    // 构造函数：直接用解码得到的图像初始化
    explicit PyramidImage(const cv::Mat& decodedImage);

    // 构造函数：带有原始文件信息
    PyramidImage(const cv::Mat& decodedImage, const std::string& sourcePath, const std::string& format);

    // 获取原始图像（用于最终保存等高质量用途）
    const cv::Mat& getOriginal() const;

    // 获取金字塔层数
    int getPyramidLevelCount() const;

    // 获取指定层级的图像（0 是原图，1 是 1/2，2 是 1/4，以此类推）
    const cv::Mat& getPyramidLevel(int level);

    // 获取推荐用于预览的图像（自动选择一个低分辨率层）
    const cv::Mat& getPreviewImage();

    // 重置金字塔（例如图像变更后）
    void rebuildPyramid(int maxLevels = 4);

    // 获取原始路径、格式信息
    const std::string& getSourcePath() const;
    const std::string& getFormat() const;

private:
    cv::Mat originalImage;                 // 原始图像
    std::vector<cv::Mat> pyramidImages;    // 图像金字塔
    std::string sourcePath;                // 原始路径
    std::string format;                    // "raw", "heic", "jpeg", etc.
    int maxLevels = 4;                     // 默认最多构建几层金字塔

    void buildPyramidIfNeeded();           // 懒加载构建
};

#endif //MONICAIMAGEPROCESS_PYRAMIDIMAGE_H
