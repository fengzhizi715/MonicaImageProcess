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

std::map<std::string, std::vector<uint8_t>> parseMultipartFormDataManual(http::request<http::dynamic_body>& req);

Mat binaryToCvMat(std::vector<uint8_t>& data);

Mat requestBodyToCvMat(http::request<http::dynamic_body>& req);

string cvMatToResponseBody(Mat& image, string extension);

#endif //MONICAIMAGEPROCESS_HTTPUTILS_H
