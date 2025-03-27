//
// Created by Tony on 2025/3/26.
//
#include "HttpUtils.h"

Mat requestBodyToCvMat(http::request<http::dynamic_body>& req) {
    // 直接访问请求体的原始字节
    auto& body = req.body();
    auto buffers = body.data();
    size_t total_size = boost::beast::buffer_bytes(buffers);

    // 将缓冲区数据直接拷贝到 vector<uchar>
    std::vector<uchar> data;
    data.reserve(total_size);
    for (auto b : buffers) {
        const uchar* ptr = static_cast<const uchar*>(b.data());
        data.insert(data.end(), ptr, ptr + b.size());
    }

    // 解码图像
    cv::Mat image = cv::imdecode(data, cv::IMREAD_COLOR);
    if (image.empty()) {
        throw std::runtime_error("图像解码失败");
    }
    return image;
}

// 将 cv::Mat 编码为指定格式的二进制数据，并返回一个 std::string
string cvMatToResponseBody(Mat& image, string extension) {
    std::vector<uchar> buf;
    if (!cv::imencode(extension, image, buf)) {
        throw std::runtime_error("编码失败");
    }
    return string(reinterpret_cast<char*>(buf.data()), buf.size()); // 避免二次拷贝
}