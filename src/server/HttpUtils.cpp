//
// Created by Tony on 2025/3/26.
//
#include "HttpUtils.h"

Mat requestBodyToCvMat(http::request<http::dynamic_body>& req) {
    // 将请求体转换为字符串
    std::string body = beast::buffers_to_string(req.body().data());

    // 将字符串转换为字节数组
    std::vector<uchar> data(body.begin(), body.end());

    // 将字节数组解码为 OpenCV 的 cv::Mat 对象
    cv::Mat image = cv::imdecode(data, cv::IMREAD_COLOR);

    // 检查图像是否解码成功
    if (image.empty()) {
        throw std::runtime_error("图像解码失败，请检查上传的文件是否为有效的图像格式。");
    }

    return image;
}

// 将 cv::Mat 编码为指定格式的二进制数据，并返回一个 std::string
string cvMatToResponseBody(Mat& image, string extension) {
    // 检查图像是否为空
    if (image.empty()) {
        throw std::runtime_error("输入图像为空");
    }

    std::vector<uchar> buf;
    // 编码图像到指定格式，例如 JPEG
    if (!cv::imencode(extension, image, buf)) {
        throw std::runtime_error("图像编码失败");
    }

    return std::string(buf.begin(), buf.end());
}