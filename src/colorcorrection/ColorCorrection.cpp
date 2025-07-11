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
    this->origin = src;
    width = origin.rows;
    height = origin.cols;
    middleRow = origin.rows / 2;
    middleCol = origin.cols / 2;
    radius = sqrt(pow(width, 2) + pow(height, 2)) / 2.0;

    cvtColor(origin, _cachedHSLImg, cv::COLOR_BGR2HSV);
    GaussianBlur(origin, blurMask, {0, 0}, 5);
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

cv::Mat ColorCorrection::adjust() {
    cout << "_contractScale = " << _contractScale << ", _hueOffset = " << _hueOffset
         << ", _saturationOffset = " << _saturationOffset << ", _lightnessOffset = " << _lightnessOffset
         << ", _temperatureScale = " << _temperatureScale << ", _highlightOffset = " << _highlightOffset
         << ", _shadowOffset = " << _shadowOffset << ", _sharpenOffset = " << _sharpenOffset
         << ", _cornerOffset = " << _cornerOffset << endl;

    cv::Mat hslCopy = _cachedHSLImg.clone();

    for (int i = 0; i < _cachedHSLImg.rows; ++i) {
        cv::Vec3b* dstRow = _cachedHSLImg.ptr<cv::Vec3b>(i);
        const cv::Vec3b* srcRow = hslCopy.ptr<cv::Vec3b>(i);
        const uchar* hlRow = highlightMask.ptr<uchar>(i);
        const uchar* shRow = shadowMask.ptr<uchar>(i);

        for (int j = 0; j < _cachedHSLImg.cols; ++j) {
            const auto& hslVal = srcRow[j];

            int hVal = hslVal[0] + _hueOffset;
            while (hVal > 180) hVal -= 180;
            while (hVal < 0) hVal += 180;

            int sVal = hslVal[1] + _saturationOffset;
            sVal = std::clamp(sVal, 0, 255);

            float highlightFactor = hlRow[j] / 255.0f;
            float shadowFactor = shRow[j] / 255.0f;

            int lVal = _contractScale * hslVal[2] + _lightnessOffset +
                       highlightFactor * _highlightOffset +
                       shadowFactor * _shadowOffset;

            lVal = std::clamp(lVal, 0, 255);

            float distanceToCenter = std::sqrt(
                    (i - middleRow) * (i - middleRow) + (j - middleCol) * (j - middleCol));
            float cornerFactor = 1.0f - std::max(distanceToCenter / radius * 2.0f - 1.0f, 0.0f) * _cornerOffset;
            lVal = std::clamp(int(lVal * cornerFactor), 0, 255);

            dstRow[j] = cv::Vec3b(hVal, sVal, lVal);
        }
    }

    cv::Mat dst;
    cv::cvtColor(_cachedHSLImg, dst, cv::COLOR_HSV2BGR);

    for (int i = 0; i < dst.rows; ++i) {
        cv::Vec3b* row = dst.ptr<cv::Vec3b>(i);
        for (int j = 0; j < dst.cols; ++j) {
            int bVal = row[j][0];
            int gVal = row[j][1];
            int rVal = row[j][2];

            if (_temperatureScale > 0) {
                rVal = std::min(255, int(rVal * (1.0 + _temperatureScale)));
                gVal = std::min(255, int(gVal * (1.0 + _temperatureScale * 0.4f)));
            } else {
                bVal = std::min(255, int(bVal * (1.0 - _temperatureScale)));
            }

            row[j] = cv::Vec3b(bVal, gVal, rVal);
        }
    }

    cv::addWeighted(dst, 1.0 + _sharpenOffset, blurMask, -_sharpenOffset, 0, dst);
    return dst;
}


void ColorCorrection::genHighlightAndShadowMask() {
    cvtColor(origin, highlightMask, cv::COLOR_BGR2GRAY);

    origin.forEach<Pixel>([&](Pixel &p, const int * position) -> void {
        int i = position[0];
        int j = position[1];

        auto grayVal = highlightMask.at<unsigned char>(i, j);
        if (grayVal > 150) {
            highlightMask.at<unsigned char>(i, j) = 255;
        } else {
            float falloffFactor = highlightMask.at<unsigned char>(i, j) / 150.0f;
            falloffFactor = powf(falloffFactor, 2);
            highlightMask.at<unsigned char>(i, j) = (int)(falloffFactor * 255);
        }
    });

    cvtColor(origin, shadowMask, cv::COLOR_BGR2GRAY);

    origin.forEach<Pixel>([&](Pixel &p, const int * position) -> void {
        int i = position[0];
        int j = position[1];

        auto grayVal = shadowMask.at<unsigned char>(i, j);
        if (grayVal < 50) {
            shadowMask.at<unsigned char>(i, j) = 255;
        } else {
            float falloffFactor = (255 - shadowMask.at<unsigned char>(i, j)) / (255.0 - 50.0);
            falloffFactor = powf(falloffFactor, 2);
            shadowMask.at<unsigned char>(i, j) = (int)(falloffFactor * 255);
        }
    });
}