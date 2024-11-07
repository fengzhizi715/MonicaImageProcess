//
// Created by Tony on 2024/11/5.
//

#include "../../include/colorcorrection//ColorCorrection.h"

typedef cv::Point3_<uint8_t> Pixel;

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
    width = originImg.rows;
    height = originImg.cols;
    middleRow = originImg.rows / 2;
    middleCol = originImg.cols / 2;
    radius = sqrt(pow(width, 2) + pow(height, 2)) / 2.0;

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

void ColorCorrection::doColorCorrection(ColorCorrectionSettings colorCorrectionSettings, Mat& dst) {

    switch( colorCorrectionSettings.status) {
        case 1:
            dst = adjustContrast(colorCorrectionSettings.contrast);
            break;
        case 2:
            dst = adjustHue(colorCorrectionSettings.hue);
            break;
        case 3:
            dst = adjustSaturation(colorCorrectionSettings.saturation);
            break;
        case 4:
            dst = adjustLightness(colorCorrectionSettings.lightness);
            break;
        case 5:
            dst = adjustTemperature(colorCorrectionSettings.temperature);
            break;
        case 6:
            dst = adjustHighlight(colorCorrectionSettings.highlight);
            break;
        case 7:
            dst = adjustShadow(colorCorrectionSettings.shadow);
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
    printf("_contractScale = %f, _hueOffset = %d, _saturationOffset = %d, _lightnessOffset = %d, _temperatureScale = %f, _highlightOffset = %d, _shadowOffset = %d, _sharpenOffset = %f, _cornerOffset =%f\n",_contractScale,_hueOffset,_saturationOffset,_lightnessOffset,_temperatureScale,_highlightOffset,_shadowOffset,_sharpenOffset,_cornerOffset);

    cv::Mat hslCopy = _cachedHSLImg.clone();
    _cachedHSLImg.forEach<Pixel>([&](Pixel &p, const int * position) -> void {
        int i = position[0];
        int j = position[1];

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
    });

    cv::Mat dst;
    cv::cvtColor(hslCopy, dst, cv::COLOR_HSV2BGR);

    hslCopy.forEach<Pixel>([&](Pixel &p, const int * position) -> void {
        int i = position[0];
        int j = position[1];

        auto bgrVal = dst.at<cv::Vec3b>(i,j);

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

        dst.at<cv::Vec3b>(i,j) = cv::Vec3b{(unsigned char)bVal, (unsigned char)gVal, (unsigned char)rVal};
    });

    // 锐化
    cv::addWeighted(dst, 1.0 + _sharpenOffset, blurMask, -_sharpenOffset, 0, dst);
    return dst;
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
