//
// Created by Tony on 2024/11/5.
//

#ifndef MONICAIMAGEPROCESS_COLORCORRECTION_H
#define MONICAIMAGEPROCESS_COLORCORRECTION_H

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace cv;
using namespace std;

typedef struct {
    int contrast;
    int hue;
    int saturation;
    int lightness;
    int temperature;
    int highlight;
    int shadow;
    int sharpen;
    int corner;
    int status;
} ColorCorrectionSettings;

class ColorCorrection{

public:
    ColorCorrection(Mat src);
    ~ColorCorrection();

    Mat adjustContrast(int contrastScale); // 0 ~ 1
    Mat adjustHue(int hueOffset); // 0 ~ 360
    Mat adjustSaturation(int saturationOffset);
    Mat adjustLightness(int lightnessOffset);
    Mat adjustTemperature(int temperatureScale); // -1 到 1
    Mat adjustHighlight(int highlightOffset);
    Mat adjustShadow(int shadowOffset);
    Mat adjustSharpen(int sharpenOffset);
    Mat adjustCorner(int cornerOffset);

    void doColorCorrection(ColorCorrectionSettings colorCorrectionSettings, Mat& dst);

    Mat originImg;
    Mat grayImg;
    Mat highlightMask;
    Mat shadowMask;
    Mat blurMask;

private:
    Mat _cachedHSLImg;
    float _contractScale;
    int _hueOffset;
    int _saturationOffset;
    int _lightnessOffset;
    float _temperatureScale;
    int _highlightOffset;
    int _shadowOffset;
    float _sharpenOffset; // 0 ~ 1
    float _cornerOffset; // 0 ~ 1

    int width;
    int height;
    int middleRow;
    int middleCol;
    float radius;

    Mat adjust();
    void genHighlightAndShadowMask();
};

#endif //MONICAIMAGEPROCESS_COLORCORRECTION_H