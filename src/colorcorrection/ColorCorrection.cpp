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


Mat ColorCorrection::adjust() {
    cout << "_contractScale = " << _contractScale << ", _hueOffset = " << _hueOffset
         << ", _saturationOffset = " << _saturationOffset << ", _lightnessOffset = " << _lightnessOffset
         << ", _temperatureScale = " << _temperatureScale << ", _highlightOffset = " << _highlightOffset
         << ", _shadowOffset = " << _shadowOffset << ", _sharpenOffset = " << _sharpenOffset
         << ", _cornerOffset = " << _cornerOffset << endl;

    cv::Mat hslCopy = _cachedHSLImg.clone();
    cv::Mat dst;
    const int rows = hslCopy.rows;
    const int cols = hslCopy.cols;

    // 保持原始色相处理逻辑
    auto processHSL = [&](const cv::Range& range) {
        for (int i = range.start; i < range.end; i++) {
            cv::Vec3b* hslRow = hslCopy.ptr<cv::Vec3b>(i);
            const uchar* hlRow = highlightMask.ptr<uchar>(i);
            const uchar* shRow = shadowMask.ptr<uchar>(i);

            for (int j = 0; j < cols; j++) {
                cv::Vec3b& hslVal = hslRow[j];
                int hVal = hslVal[0] + _hueOffset;

                // 保持原始色相处理（不处理负值）
                while (hVal > 180) hVal -= 180;

                // 饱和度调整（原始手动钳制方式）
                int sVal = hslVal[1] + _saturationOffset;
                sVal = (sVal > 255) ? 255 : (sVal < 0) ? 0 : sVal;

                // 亮度调整（完全保持原始公式）
                float hlFactor = hlRow[j] / 255.0f;
                float shFactor = shRow[j] / 255.0f;
                int lVal = _contractScale * hslVal[2] +
                           _lightnessOffset +
                           hlFactor * _highlightOffset +
                           shFactor * _shadowOffset;

                // 手动钳制亮度
                lVal = (lVal > 255) ? 255 : (lVal < 0) ? 0 : lVal;

                // 暗角效果（实时计算保持原始逻辑）
                float distance = sqrt(pow(i - middleRow, 2) + pow(j - middleCol, 2));
                float cornerFactor = 1.0f - std::max(distance / radius * 2.0f - 1.0f, 0.0f) * _cornerOffset;
                lVal *= cornerFactor;
                lVal = (lVal > 255) ? 255 : (lVal < 0) ? 0 : lVal;

                hslVal = cv::Vec3b(
                        static_cast<uchar>(hVal),
                        static_cast<uchar>(sVal),
                        static_cast<uchar>(lVal)
                );
            }
        }
    };

    // 并行执行HSL调整
    cv::parallel_for_(cv::Range(0, rows), processHSL);

    // 转换到BGR
    cv::cvtColor(hslCopy, dst, cv::COLOR_HSV2BGR);

    // 温度调整（保持原始逻辑）
    auto processTemperature = [&](const cv::Range& range) {
        for (int i = range.start; i < range.end; i++) {
            cv::Vec3b* row = dst.ptr<cv::Vec3b>(i);
            for (int j = 0; j < cols; j++) {
                int bVal = row[j][0];
                int gVal = row[j][1];
                int rVal = row[j][2];

                if (_temperatureScale > 0) {
                    rVal *= (1.0f + _temperatureScale);
                    gVal *= (1.0f + _temperatureScale * 0.4f);
                    rVal = (rVal > 255) ? 255 : rVal;
                    gVal = (gVal > 255) ? 255 : gVal;
                } else {
                    bVal *= (1.0f - _temperatureScale);
                    bVal = (bVal > 255) ? 255 : bVal;
                }

                row[j] = cv::Vec3b(
                        static_cast<uchar>(bVal),
                        static_cast<uchar>(gVal),
                        static_cast<uchar>(rVal)
                );
            }
        }
    };

    // 并行执行温度调整
    cv::parallel_for_(cv::Range(0, rows), processTemperature);

    // 锐化处理
    cv::addWeighted(dst, 1.0f + _sharpenOffset, blurMask, -_sharpenOffset, 0, dst);

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