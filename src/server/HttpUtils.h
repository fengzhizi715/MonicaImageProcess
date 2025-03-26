//
// Created by Tony on 2025/3/26.
//

#ifndef MONICAIMAGEPROCESS_HTTPUTILS_H
#define MONICAIMAGEPROCESS_HTTPUTILS_H

#include <boost/beast.hpp>
#include <boost/beast/http.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

using namespace std;
using namespace cv;
namespace beast = boost::beast;
namespace http = beast::http;

Mat requestBodyToCvMat(http::request<http::dynamic_body>& req);

string cvMatToResponseBody(Mat& image, string extension);

#endif //MONICAIMAGEPROCESS_HTTPUTILS_H
