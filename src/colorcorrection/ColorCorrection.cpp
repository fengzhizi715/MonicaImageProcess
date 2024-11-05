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
        _cornerOffset(0)
{
    this->originImg = src;
    cvtColor(originImg, _cachedHSLImg, cv::COLOR_BGR2HSV);
    cvtColor(originImg, grayImg, cv::COLOR_BGR2GRAY);
    GaussianBlur(originImg, blurMask, {0, 0}, 5);
    genHighlightAndShadowMask();
}

ColorCorrection::~ColorCorrection() {
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

Mat ColorCorrection::adjust() {
    cv::Mat hslCopy = _cachedHSLImg.clone();
    int middleRow = hslCopy.rows / 2;
    int middleCol = hslCopy.cols / 2;
    float radius = sqrt(pow(hslCopy.rows, 2) + pow(hslCopy.cols, 2)) / 2.0;
    for(int i = 0; i < hslCopy.rows; i++){
        for(int j=0; j < hslCopy.cols; j++){
            auto hslVal = hslCopy.at<cv::Vec3b>(i,j);

            int hVal = hslVal[0] + _hueOffset;
            while (hVal > 180) {
                hVal -= 180;
            }

            int sval = hslVal[1] + _saturationOffset;
            if (sval > 255) {
                sval = 255;
            }
            if (sval < 0) {
                sval = 0;
            }

            unsigned char highlightFactor = highlightMask.at<unsigned char>(i, j);
            unsigned char shadowFactor = shadowMask.at<unsigned char>(i, j);

            int lval =  _contractScale * hslVal[2] + _lightnessOffset + highlightFactor / 255.0 * _highlightOffset + shadowFactor / 255.0 * _shadowOffset;
            if (lval > 255) {
                lval = 255;
            }
            if (lval < 0) {
                lval = 0;
            }

            // 简单的暗角效果
            float distanceToCenter = sqrt(pow(i - middleRow, 2) + pow(j - middleCol, 2));
            float cornerFactor = 1.0 - std::max(distanceToCenter / radius * 2.0 - 1.0, 0.0) * _cornerOffset;
            lval *= cornerFactor;

            hslCopy.at<cv::Vec3b>(i,j) = cv::Vec3b{(unsigned char)hVal, (unsigned char)sval, (unsigned char)lval};
        }
    }
    cv::Mat finalImg;
    cv::cvtColor(hslCopy, finalImg, cv::COLOR_HSV2BGR);
    for(int i = 0; i < finalImg.rows; i++){
        for(int j=0; j < finalImg.cols; j++){
            auto bgrVal = finalImg.at<cv::Vec3b>(i,j);

            int bVal = bgrVal[0];
            int gVal = bgrVal[1];
            int rVal = bgrVal[2];

            // 使用乘法，保证各个分量不会同步增长，导致全屏泛红（蓝色）
            if (_temperatureScale > 0) {
                rVal *= 1.0 + _temperatureScale;
                gVal *= (1.0 + _temperatureScale * 0.4f);
            } else {
                bVal *= 1.0f -_temperatureScale;
            }
            rVal = std::min(255, rVal);
            gVal = std::min(255, gVal);
            bVal = std::min(255, bVal);

            finalImg.at<cv::Vec3b>(i,j) = cv::Vec3b{(unsigned char)bVal, (unsigned char)gVal, (unsigned char)rVal};
        }
    }

    // 锐化
    cv::addWeighted(finalImg, 1.0 + _sharpenOffset, blurMask, -_sharpenOffset, 0, finalImg);
    return finalImg;
}

void ColorCorrection::genHighlightAndShadowMask() {
    cvtColor(originImg, highlightMask, cv::COLOR_BGR2GRAY);

    for(int i = 0; i < highlightMask.rows; i++) {
        for (int j = 0; j < highlightMask.cols; j++) {
            auto grayVal = highlightMask.at<unsigned char>(i, j);
            if (grayVal > 150) {
                highlightMask.at<unsigned char>(i, j) = 255;
            } else {
                float falloffFactor = highlightMask.at<unsigned char>(i, j) / 150.0f;
                falloffFactor = powf(falloffFactor, 2);
                highlightMask.at<unsigned char>(i, j) = (int)(falloffFactor * 255);
            }
        }
    }

    cvtColor(originImg, shadowMask, cv::COLOR_BGR2GRAY);
    for(int i = 0; i < shadowMask.rows; i++) {
        for (int j = 0; j < shadowMask.cols; j++) {
            auto grayVal = shadowMask.at<unsigned char>(i, j);
            if (grayVal < 50) {
                shadowMask.at<unsigned char>(i, j) = 255;
            } else {
                float falloffFactor = (255 - shadowMask.at<unsigned char>(i, j)) / (255.0 - 50.0);
                falloffFactor = powf(falloffFactor, 2);
                shadowMask.at<unsigned char>(i, j) = (int)(falloffFactor * 255);
            }
        }
    }
}
