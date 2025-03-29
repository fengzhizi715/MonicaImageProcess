//
// Created by Tony on 2025/3/26.
//

#ifndef MONICAIMAGEPROCESS_HTTPUTILS_H
#define MONICAIMAGEPROCESS_HTTPUTILS_H

#include <boost/beast.hpp>
#include <boost/beast/http.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <string>
#include <vector>
#include <map>
#include <stdexcept>

using namespace std;
using namespace cv;
namespace beast = boost::beast;
namespace http = beast::http;

// 解析 multipart/form-data 并转换成 map
std::map<std::string, std::vector<uint8_t>> parseMultipartFormDataManual(http::request<http::dynamic_body>& req);

// 将二进制数据转换为 Mat
Mat binaryToCvMat(std::vector<uint8_t>& data);

// 将 HTTP 请求体转换为 Mat 对象
Mat requestBodyToCvMat(http::request<http::dynamic_body>& req);

// 将 Mat 转换为 std::string，用于 HTTP 响应返回图片
string cvMatToResponseBody(Mat& image, string extension);

#endif //MONICAIMAGEPROCESS_HTTPUTILS_H
