#include "../include/library.h"

#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>

using namespace std;
using namespace cv;
using namespace cv::ml;

uint8_t gammaLUT[256] = { 0 };

namespace ACE {
    Mat stretchImage(Mat src) {
        int row = src.rows;
        int col = src.cols;
        Mat dst(row, col, CV_64FC1);
        double MaxValue = *max_element(src.begin<double>(), src.end<double>());
        double MinValue = *min_element(src.begin<double>(), src.end<double>());
        for (int i = 0; i < row; i++) {
            double *ptr_dst=dst.ptr<double>(i);
            double *ptr_src=src.ptr<double>(i);
            for (int j = 0; j < col; j++) {
                double src_data = *ptr_src++;
                *ptr_dst++=(src_data- MinValue) / (MaxValue - MinValue);
            }
        }
        return dst;
    }

    Mat getPara(int radius) {
        int size = radius * 2 + 1;
        Mat dst(size, size, CV_64FC1);
        for (int i = -radius; i <= radius; i++) {
            for (int j = -radius; j <= radius; j++) {
                if (i == 0 && j == 0) {
                    dst.at<double>(i + radius, j + radius) = 0;
                }
                else {
                    dst.at<double>(i + radius, j + radius) = 1.0 / sqrt(i * i + j * j);
                }
            }
        }
        double sum = 0;
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                sum += dst.at<double>(i, j);
            }
        }
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                dst.at<double>(i, j) = dst.at<double>(i, j) / sum;
            }
        }
        return dst;
    }

    Mat NormalACE(Mat src, int ratio, int radius) {
        Mat para = getPara(radius);
        int row = src.rows;
        int col = src.cols;
        int size = 2 * radius + 1;
        Mat Z(row + 2 * radius, col + 2 * radius, CV_64FC1);
        for (int i = 0; i < Z.rows; i++) {
            double *ptr_Z=Z.ptr<double>(i);
            double *ptr_src=src.ptr<double>(i-radius);
            for (int j = 0; j < Z.cols; j++) {
                if((i - radius >= 0) && (i - radius < row) && (j - radius >= 0) && (j - radius < col)) {
                    *ptr_Z++=ptr_src[j-radius];
                    //Z.at<double>(i, j) = src.at<double>(i - radius, j - radius);
                }
                else {
                    *ptr_Z++=0;
                    //Z.at<double>(i, j) = 0;
                }
            }
        }
        Mat dst=Mat::zeros(row,col,CV_64FC1);
        for (int i = 0; i < size; i++) {
            double *p_para=para.ptr<double>(i);
            for (int j = 0; j < size; j++) {
                double para_data=*p_para;
                if (*p_para++ == 0) continue;
                for (int x = 0; x < row; x++) {
                    double *p_src=src.ptr<double>(x);
                    double *p_dst=dst.ptr<double>(x);
                    double *p_Z=Z.ptr<double>(x+i,j);
                    for (int y = 0; y < col; y++) {
                        double scr_at_xy = *p_src++;
                        double sub = scr_at_xy -*p_Z++;// Z.at<double>(x + i, y + j);
                        //double sub = src.at<double>(x, y) - Z.at<double>(x + i, y + j);
                        double tmp = sub * ratio;
                        if (tmp > 1.0) tmp = 1.0;
                        if (tmp < -1.0) tmp = -1.0;
                        *(p_dst++)+=tmp * para_data;
                    }
                }
            }
        }

        return dst;
    }

    Mat FastACE(Mat src, int ratio, int radius) {
        int row = src.rows;
        int col = src.cols;
        if (min(row, col) <= 2) {
            Mat dst(row, col, CV_64FC1);
            for (int i = 0; i < row; i++) {
                for (int j = 0; j < col; j++) {
                    dst.at<double>(i, j) = 0.5;
                }
            }
            return dst;
        }

        Mat Rs((row + 1) / 2, (col + 1) / 2, CV_64FC1);
        resize(src, Rs, Size((col + 1) / 2, (row + 1) / 2));
        Mat Rf= FastACE(Rs, ratio, radius);
        resize(Rf, Rf, Size(col, row));
        resize(Rs, Rs, Size(col, row));
        Mat dst(row, col, CV_64FC1);
        Mat dst1 = NormalACE(src, ratio, radius);
        Mat dst2 = NormalACE(Rs, ratio, radius);

        dst=Rf+dst1-dst2;
        return dst;
    }

    Mat getACE(Mat src, int ratio, int radius) {
        int row = src.rows;
        int col = src.cols;
        vector <Mat> v;
        split(src, v);
        v[0].convertTo(v[0], CV_64FC1);
        v[1].convertTo(v[1], CV_64FC1);
        v[2].convertTo(v[2], CV_64FC1);
        Mat src1(row, col, CV_64FC1);
        Mat src2(row, col, CV_64FC1);
        Mat src3(row, col, CV_64FC1);

        for (int i = 0; i < row; i++) {
            for (int j = 0; j < col; j++) {
                src1.at<double>(i, j) = 1.0 * src.at<Vec3b>(i, j)[0] / 255.0;
                src2.at<double>(i, j) = 1.0 * src.at<Vec3b>(i, j)[1] / 255.0;
                src3.at<double>(i, j) = 1.0 * src.at<Vec3b>(i, j)[2] / 255.0;
            }
        }
        src1 = stretchImage(FastACE(src1, ratio, radius));
        src2 = stretchImage(FastACE(src2, ratio, radius));
        src3 = stretchImage(FastACE(src3, ratio, radius));
        Mat dst1(row, col, CV_8UC1);
        Mat dst2(row, col, CV_8UC1);
        Mat dst3(row, col, CV_8UC1);
        for (int i = 0; i < row; i++) {
            for (int j = 0; j < col; j++) {
                dst1.at<uchar>(i, j) = (int)(src1.at<double>(i, j) * 255);
                if (dst1.at<uchar>(i, j) > 255) dst1.at<uchar>(i, j) = 255;
                else if (dst1.at<uchar>(i, j) < 0) dst1.at<uchar>(i, j) = 0;
                dst2.at<uchar>(i, j) = (int)(src2.at<double>(i, j) * 255);
                if (dst2.at<uchar>(i, j) > 255) dst2.at<uchar>(i, j) = 255;
                else if (dst2.at<uchar>(i, j) < 0) dst2.at<uchar>(i, j) = 0;
                dst3.at<uchar>(i, j) = (int)(src3.at<double>(i, j) * 255);
                if (dst3.at<uchar>(i, j) > 255) dst3.at<uchar>(i, j) = 255;
                else if (dst3.at<uchar>(i, j) < 0) dst3.at<uchar>(i, j) = 0;
            }
        }

        vector <Mat> out;
        out.push_back(dst1);
        out.push_back(dst2);
        out.push_back(dst3);
        Mat dst;
        merge(out, dst);

        return dst;
    }
}

Mat shearing(Mat src, float x, float y) {
    int width = src.cols;
    int height = src.rows;

    Mat dst;
    Mat warp_matrix = (cv::Mat_<float>(2, 3) <<1, x, 0, y, 1, 0);
    warpAffine(src, dst, warp_matrix, Size(width, height), INTER_LINEAR);

    return dst;
}

Mat equalizeHistImage(Mat src) {
    int channel = src.channels();

    if (channel == 3) {
        Mat imageRGB[3];
        split(src, imageRGB);
        for (int i = 0; i < 3; i++)
        {
            equalizeHist(imageRGB[i], imageRGB[i]);
        }
        merge(imageRGB, 3, src);
    } else if (channel == 1){
        equalizeHist(src, src);
    }

    return src;
}

/**
 * 限制对比度自适应直方图均衡
 */
void clahe(Mat& src, Mat& dst, double clipLimit, int size) {
    int channel = src.channels();

    if (channel == 3) {
        // 将图像转换为 LAB 色彩空间
        cv::Mat lab_image;
        cv::cvtColor(src, lab_image, cv::COLOR_BGR2Lab);

        // 分离 LAB 图像的通道
        std::vector<cv::Mat> lab_planes(3);
        cv::split(lab_image, lab_planes);

        // 创建 CLAHE 对象并设置参数
        cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE();
        clahe->setClipLimit(clipLimit);                 // 设置对比度限制
        clahe->setTilesGridSize(cv::Size(size, size));  // 设置网格大小

        // 对 L 通道应用 CLAHE
        clahe->apply(lab_planes[0], lab_planes[0]);

        // 合并处理后的 LAB 通道
        cv::Mat merged_lab_image;
        cv::merge(lab_planes, merged_lab_image);

        // 将 LAB 图像转换回 BGR 色彩空间
        cv::cvtColor(merged_lab_image, dst, cv::COLOR_Lab2BGR);
    } else {
        cv::Ptr<CLAHE> clahe = cv::createCLAHE();
        clahe->setClipLimit(clipLimit);                 // 设置对比度限制
        clahe->setTilesGridSize(cv::Size(size, size));  // 设置网格大小
        clahe->apply(src, dst);
    }
}

void gammaCorrection(Mat& src, Mat& dst, float K) {
    src.copyTo(dst);
    for (int i = 0; i < 256; i++) {
        float f = i / 255.0f; // 注意不可以写成 i / 255
        f = pow(f, K);
        gammaLUT[i] = static_cast<uint8_t>(f * 255.0);
    }

    MatIterator_<Vec3b> it = dst.begin<Vec3b>();
    MatIterator_<Vec3b> it_end = dst.end<Vec3b>();

    for (; it != it_end; it++)
    {
        (*it)[0] = gammaLUT[(*it)[0]];
        (*it)[1] = gammaLUT[(*it)[1]];
        (*it)[2] = gammaLUT[(*it)[2]];
    }
}

void laplace(Mat& src, Mat& dst) {

    cv::Mat kernel = (Mat_<char>(3, 3) << -1, -1, -1, -1, 9, -1, -1, -1, -1);
    cv::filter2D(src, dst, CV_8UC3, kernel);
}

void unsharpMask(const Mat& src, Mat& dst, int radius, int threshold, int amount) {
    int height = src.rows;
    int width = src.cols;
    GaussianBlur(src, dst, cv::Size(radius, radius), 2, 2);

    for (int h = 0; h < height; ++h) {
        for (int w = 0; w < width; ++w) {
            int b = src.at<Vec3b>(h, w)[0] - dst.at<Vec3b>(h, w)[0];
            int g = src.at<Vec3b>(h, w)[1] - dst.at<Vec3b>(h, w)[1];
            int r = src.at<Vec3b>(h, w)[2] - dst.at<Vec3b>(h, w)[2];
            if (abs(b) > threshold) {
                b = src.at<Vec3b>(h, w)[0] + amount * b / 100;
                dst.at<Vec3b>(h, w)[0] = saturate_cast<uchar>(b);
            }
            if (abs(g) > threshold) {
                g = src.at<Vec3b>(h, w)[1] + amount * g / 100;
                dst.at<Vec3b>(h, w)[1] = saturate_cast<uchar>(g);
            }
            if (abs(r) > threshold) {
                r = src.at<Vec3b>(h, w)[2] + amount * r / 100;
                dst.at<Vec3b>(h, w)[2] = saturate_cast<uchar>(r);
            }
        }
    }
}

void ace(Mat& src, Mat& dst, int ratio, int radius) {
    dst = ACE::getACE(src, ratio, radius);
}