//
// Created by Tony on 2024/11/5.
//

#include "../../include/colorcorrection//ColorCorrection.h"

ColorCorrection::ColorCorrection(Mat src):
        _hueOffset(0),
        _saturationOffset(0),
        _lightnessOffset(0),
        _contractScale(1.0),
        _temperatureScale(0.0),
        _highlightOffset(0.0),
        _shadowOffset(0),
        _sharpenOffset(0),
        _cornerOffset(0),
        lastHueOffset(0),
        lastSaturationOffset(0),
        lastContractScale(1.0f),
        lastLightnessOffset(0),
        lastTemperatureScale(0.0f)
{
    this->origin = src;
    width = origin.rows;
    height = origin.cols;
    middleRow = origin.rows / 2;
    middleCol = origin.cols / 2;

    // 使用std::hypot计算半径
    radius = std::hypot(middleRow, middleCol);

    cvtColor(origin, _cachedHSLImg, cv::COLOR_BGR2HSV);
    GaussianBlur(origin, blurMask, {0, 0}, 5);

    // 预计算距离映射图
    distanceMap.create(origin.size(), CV_32F);
    parallel_for_(Range(0, origin.rows), [&](const Range& range) {
        for (int r = range.start; r < range.end; ++r) {
            float* ptr = distanceMap.ptr<float>(r);
            float dy = r - middleRow;
            for (int c = 0; c < origin.cols; ++c) {
                float dx = c - middleCol;
                ptr[c] = dx * dx + dy * dy;  // 存储距离平方
            }
        }
    });

    genHighlightAndShadowMask();

    // Initialize LUTs
    updateHueLUT();
    updateSaturationLUT();
    updateLightnessLUT();
    updateTemperatureLUT();
}

ColorCorrection::~ColorCorrection() {
}

void ColorCorrection::updateHueLUT() {
    lutH.create(1, 256, CV_8U);
    for (int i = 0; i < 256; i++) {
        int h = i + _hueOffset;
        h = (h % 180 + 180) % 180;
        lutH.at<uchar>(i) = static_cast<uchar>(h);
    }
    lastHueOffset = _hueOffset;
}

void ColorCorrection::updateSaturationLUT() {
    lutS.create(1, 256, CV_8U);
    for (int i = 0; i < 256; i++) {
        int s = i + _saturationOffset;
        s = (s > 255) ? 255 : (s < 0) ? 0 : s;
        lutS.at<uchar>(i) = static_cast<uchar>(s);
    }
    lastSaturationOffset = _saturationOffset;
}

void ColorCorrection::updateLightnessLUT() {
    lutL.create(1, 256, CV_8U);
    for (int i = 0; i < 256; i++) {
        float l = _contractScale * i + _lightnessOffset;
        l = (l > 255) ? 255 : (l < 0) ? 0 : l;
        lutL.at<uchar>(i) = static_cast<uchar>(l);
    }
    lastContractScale = _contractScale;
    lastLightnessOffset = _lightnessOffset;
}

void ColorCorrection::updateTemperatureLUT() {
    lutB.create(1, 256, CV_8U);
    lutG.create(1, 256, CV_8U);
    lutR.create(1, 256, CV_8U);

    if (_temperatureScale > 0) {
        for (int i = 0; i < 256; i++) {
            lutR.at<uchar>(i) = saturate_cast<uchar>(i * (1.0f + _temperatureScale));
            lutG.at<uchar>(i) = saturate_cast<uchar>(i * (1.0f + _temperatureScale * 0.4f));
            lutB.at<uchar>(i) = i;
        }
    } else {
        float scale = -_temperatureScale;
        for (int i = 0; i < 256; i++) {
            lutB.at<uchar>(i) = saturate_cast<uchar>(i * (1.0f + scale));
            lutG.at<uchar>(i) = i;
            lutR.at<uchar>(i) = i;
        }
    }
    lastTemperatureScale = _temperatureScale;
}

Mat ColorCorrection::adjustContrast(int contrastScale) {
    _contractScale = contrastScale / 255.0 * 2.0f;
    return adjust();
}

Mat ColorCorrection::adjustHue(int hueOffset) {
    _hueOffset = hueOffset;
    return adjust();
}

Mat ColorCorrection::adjustSaturation(int saturationOffset) {
    _saturationOffset = saturationOffset;
    return adjust();
}

Mat ColorCorrection::adjustLightness(int lightnessOffset) {
    _lightnessOffset = lightnessOffset;
    return adjust();
}

Mat ColorCorrection::adjustTemperature(int temperatureScale) {
    _temperatureScale = temperatureScale / 255.0;
    return adjust();
}

Mat ColorCorrection::adjustHighlight(int highlightOffset) {
    _highlightOffset = highlightOffset;
    return adjust();
}

Mat ColorCorrection::adjustShadow(int shadowOffset) {
    _shadowOffset = shadowOffset;
    return adjust();
}

Mat ColorCorrection::adjustSharpen(int sharpenOffset) {
    _sharpenOffset = sharpenOffset / 255.0;
    return adjust();
}

Mat ColorCorrection::adjustCorner(int cornerOffset) {
    _cornerOffset = cornerOffset / 255.0;
    return adjust();
}

void ColorCorrection::doColorCorrection(ColorCorrectionSettings colorCorrectionSettings, Mat& dst) {
    switch(colorCorrectionSettings.status) {
        case 1:
            dst = adjustContrast(colorCorrectionSettings.contrast - 255);
            break;
        case 2:
            dst = adjustHue(colorCorrectionSettings.hue - 180);
            break;
        case 3:
            dst = adjustSaturation(colorCorrectionSettings.saturation - 255);
            break;
        case 4:
            dst = adjustLightness(colorCorrectionSettings.lightness - 255);
            break;
        case 5:
            dst = adjustTemperature(colorCorrectionSettings.temperature - 255);
            break;
        case 6:
            dst = adjustHighlight(colorCorrectionSettings.highlight - 255);
            break;
        case 7:
            dst = adjustShadow(colorCorrectionSettings.shadow - 255);
            break;
        case 8:
            dst = adjustSharpen(colorCorrectionSettings.sharpen);
            break;
        case 9:
            dst = adjustCorner(colorCorrectionSettings.corner);
            break;
        default:
            break;
    }
}

Mat ColorCorrection::adjust() {
    cout << "_contractScale = " << _contractScale << ", _hueOffset = " << _hueOffset
         << ", _saturationOffset = " << _saturationOffset << ", _lightnessOffset = " << _lightnessOffset
         << ", _temperatureScale = " << _temperatureScale << ", _highlightOffset = " << _highlightOffset
         << ", _shadowOffset = " << _shadowOffset << ", _sharpenOffset = " << _sharpenOffset
         << ", _cornerOffset = " << _cornerOffset << endl;

    // Update LUTs if parameters changed
    if (lastHueOffset != _hueOffset) updateHueLUT();
    if (lastSaturationOffset != _saturationOffset) updateSaturationLUT();
    if (lastContractScale != _contractScale || lastLightnessOffset != _lightnessOffset) updateLightnessLUT();
    if (lastTemperatureScale != _temperatureScale) updateTemperatureLUT();

    if (_cachedHSLImg.empty() || _cachedHSLImg.size() != origin.size()) {

        width = origin.rows;
        height = origin.cols;
        middleRow = origin.rows / 2;
        middleCol = origin.cols / 2;

        // 使用std::hypot计算半径
        radius = std::hypot(middleRow, middleCol);

        cvtColor(origin, _cachedHSLImg, cv::COLOR_BGR2HSV);
        GaussianBlur(origin, blurMask, {0, 0}, 5);

        // 预计算距离映射图
        distanceMap.create(origin.size(), CV_32F);
        parallel_for_(Range(0, origin.rows), [&](const Range& range) {
            for (int r = range.start; r < range.end; ++r) {
                float* ptr = distanceMap.ptr<float>(r);
                float dy = r - middleRow;
                for (int c = 0; c < origin.cols; ++c) {
                    float dx = c - middleCol;
                    ptr[c] = dx * dx + dy * dy;  // 存储距离平方
                }
            }
        });

        genHighlightAndShadowMask();
    }

    cv::Mat hslCopy = _cachedHSLImg.clone();
    cv::Mat dst;
    const int rows = hslCopy.rows;
    const int cols = hslCopy.cols;

    // 直接操作连续内存替代 split/merge
    parallel_for_(Range(0, rows), [&](const Range& range) {
        for (int r = range.start; r < range.end; r++) {
            Vec3b* row = hslCopy.ptr<Vec3b>(r);
            for (int c = 0; c < cols; c++) {
                row[c][0] = lutH.at<uchar>(row[c][0]);  // H通道
                row[c][1] = lutS.at<uchar>(row[c][1]);  // S通道
                row[c][2] = lutL.at<uchar>(row[c][2]);  // L通道
            }
        }
    });

    // Apply highlight, shadow and corner effects
    auto processEffects = [&](const cv::Range& range) {
        for (int i = range.start; i < range.end; i++) {
            cv::Vec3b* hslRow = hslCopy.ptr<cv::Vec3b>(i);
            const uchar* hlRow = highlightMask.ptr<uchar>(i);
            const uchar* shRow = shadowMask.ptr<uchar>(i);
            const float* distRow = distanceMap.ptr<float>(i);  // 使用预计算的距离映射

            for (int j = 0; j < cols; j++) {
                cv::Vec3b& hslVal = hslRow[j];
                uchar& v = hslVal[2];  // Lightness channel

                // Apply highlight and shadow
                float hlFactor = hlRow[j] / 255.0f;
                float shFactor = shRow[j] / 255.0f;
                int lVal = v + hlFactor * _highlightOffset + shFactor * _shadowOffset;

                // Apply corner effect - 使用预计算的距离平方
                float distance = std::sqrt(distRow[j]);  // 只需一次开方
                float cornerFactor = 1.0f - std::max(distance / radius * 2.0f - 1.0f, 0.0f) * _cornerOffset;
                lVal = static_cast<int>(lVal * cornerFactor);

                // Clamp value
                v = saturate_cast<uchar>(lVal);
            }
        }
    };
    cv::parallel_for_(cv::Range(0, rows), processEffects);

    // Convert to BGR
    cv::cvtColor(hslCopy, dst, cv::COLOR_HSV2BGR);

    // Apply temperature adjustment
    vector<Mat> bgrChannels(3);
    split(dst, bgrChannels);
    LUT(bgrChannels[0], lutB, bgrChannels[0]); // B channel
    LUT(bgrChannels[1], lutG, bgrChannels[1]); // G channel
    LUT(bgrChannels[2], lutR, bgrChannels[2]); // R channel
    merge(bgrChannels, dst);

    // Apply sharpen
    cv::addWeighted(dst, 1.0f + _sharpenOffset, blurMask, -_sharpenOffset, 0, dst);

    return dst;
}

void ColorCorrection::genHighlightAndShadowMask() {
    cvtColor(origin, highlightMask, COLOR_BGR2GRAY);
    cvtColor(origin, shadowMask, COLOR_BGR2GRAY);

    parallel_for_(Range(0, origin.rows), [&](const Range& range) {
        for (int i = range.start; i < range.end; i++) {
            uchar* hlRow = highlightMask.ptr<uchar>(i);
            uchar* shRow = shadowMask.ptr<uchar>(i);
            for (int j = 0; j < origin.cols; j++) {
                uchar g = hlRow[j];

                if (g > 150) {
                    hlRow[j] = 255;
                } else {
                    float f = g / 150.0f;
                    hlRow[j] = static_cast<uchar>(f * f * 255);
                }

                if (g < 50) {
                    shRow[j] = 255;
                } else {
                    float f = (255 - g) / 205.0f;
                    shRow[j] = static_cast<uchar>(f * f * 255);
                }
            }
        }
    });
}