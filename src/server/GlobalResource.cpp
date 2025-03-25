//
// Created by Tony on 2025/3/25.
//
#include <iostream>
#include "GlobalResource.h"

GlobalResource::GlobalResource(string modelPath): modelPath(modelPath) {
    // 初始化资源，比如加载模型、配置文件等
    std::cout << "GlobalResource initialized." << std::endl;
}

// 一个示例函数，实际中可以实现图片处理逻辑
void GlobalResource::processImage() {
    std::cout << "Processing image..." << std::endl;
}