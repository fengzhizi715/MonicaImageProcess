//
// Created by Tony on 2025/3/26.
//
#include "HttpUtils.h"

#include <string>
#include <vector>
#include <map>
#include <stdexcept>

std::map<std::string, std::vector<uint8_t>> parseMultipartFormDataManual(http::request<http::dynamic_body>& req) {
    std::map<std::string, std::vector<uint8_t>> fileParts;

    // 获取 boundary
    auto contentType = std::string(req[http::field::content_type]);
    auto boundaryPos = contentType.find("boundary=");
    if (boundaryPos == std::string::npos) {
        throw std::runtime_error("Content-Type missing boundary.");
    }
    std::string boundary = "--" + contentType.substr(boundaryPos + 9);

    std::string bodyStr(boost::beast::buffers_to_string(req.body().data()));
    size_t pos = 0;

    while ((pos = bodyStr.find(boundary, pos)) != std::string::npos) {
        pos += boundary.size();

        // 找到 Content-Disposition 标头
        size_t dispStart = bodyStr.find("Content-Disposition:", pos);
        size_t dispEnd = bodyStr.find("\r\n\r\n", dispStart);
        if (dispStart == std::string::npos || dispEnd == std::string::npos) {
            break;
        }

        std::string disposition = bodyStr.substr(dispStart, dispEnd - dispStart);
        size_t namePos = disposition.find("name=\"");
        if (namePos == std::string::npos) {
            continue;
        }
        namePos += 6; // 跳过 `name="`
        size_t nameEnd = disposition.find("\"", namePos);
        std::string name = disposition.substr(namePos, nameEnd - namePos);

        // 获取文件内容
        size_t fileStart = dispEnd + 4; // 跳过 \r\n\r\n
        size_t fileEnd = bodyStr.find(boundary, fileStart) - 2; // 跳过 \r\n
        if (fileStart >= fileEnd) {
            continue;
        }

        fileParts[name] = std::vector<uint8_t>(
                bodyStr.begin() + fileStart,
                bodyStr.begin() + fileEnd
        );
        pos = fileEnd;
    }

    return fileParts;
}

Mat binaryToCvMat(std::vector<uint8_t>& data) {
    cv::Mat rawData(1, data.size(), CV_8UC1, (void*)data.data());
    return cv::imdecode(rawData, cv::IMREAD_COLOR);
}

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