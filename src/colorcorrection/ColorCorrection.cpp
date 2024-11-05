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

void ColorCorrection::genHighlightAndShadowMask() {
    cv::cvtColor(originImg, highlightMask, cv::COLOR_BGR2GRAY);
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

    cv::cvtColor(originImg, shadowMask, cv::COLOR_BGR2GRAY);
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
