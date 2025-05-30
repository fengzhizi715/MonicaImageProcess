//
// Created by Tony on 2025/4/18.
//
#include "../../include/utils/Utils.h"
#include <libraw/libraw.h>
#include <iostream>
#include <string>

bool ascendSort(vector<Point> a, vector<Point> b) {
    return contourArea(a) > contourArea(b);
}

// 返回 RAW 文件格式字符串（如 CR2、CR3、ARW）
std::string detectRawFormat(const std::string& path) {
    LibRaw rawProcessor;

    int ret = rawProcessor.open_file(path.c_str());
    if (ret != LIBRAW_SUCCESS) {
        return "Unknown";
    }

    // 图像设备信息结构体
    libraw_iparams_t& idata = rawProcessor.imgdata.idata;

    // Canon 文件标志：idata.make 中含 "Canon"
    // CR3 是基于 HEIF，使用不同识别法
    if (std::string(idata.make).find("Canon") != std::string::npos) {
        // CR3 有特殊标记（idata.raw_count > 0 且 idata.dng_version == 0）
        if (idata.dng_version == 0 && idata.raw_count == 1 && idata.cdesc[0] == 'R') {
            return "CR3";
        } else {
            return "CR2";
        }
    }

    if (std::string(idata.make).find("Sony") != std::string::npos) {
        return "ARW";
    }

    if (std::string(idata.make).find("Nikon") != std::string::npos) {
        return "NEF";
    }

    if (std::string(idata.make).find("Fujifilm") != std::string::npos) {
        return "RAF";
    }

    if (idata.dng_version != 0) {
        return "DNG";
    }

    return "UnknownRAW";
}