//
// Created by Tony on 2024/8/30.
//
#include "../../include/faceSwap/FaceEnhance.h"

using namespace cv;
using namespace std;
using namespace Ort;

FaceEnhance::FaceEnhance(string modelPath, const char* logId, const char* provider): OnnxRuntimeBase(modelPath, logId, provider)
{
    this->input_height = input_node_dims[0][2];
    this->input_width = input_node_dims[0][3];

    ////在这里就直接定义了，没有像python程序里的那样normed_template = TEMPLATES.get(template) * crop_size
    this->normed_template.emplace_back(Point2f(192.98138112, 239.94707968));
    this->normed_template.emplace_back(Point2f(318.90276864, 240.19360256));
    this->normed_template.emplace_back(Point2f(256.63415808, 314.01934848));
    this->normed_template.emplace_back(Point2f(201.26116864, 371.410432));
    this->normed_template.emplace_back(Point2f(313.0890496,  371.1511808));
}

void FaceEnhance::preprocess(Mat target, const vector<Point2f> face_landmark_5, Mat& affine_matrix, Mat& box_mask)
{
    Mat crop_img;
    affine_matrix = warp_face_by_face_landmark_5(target, crop_img, face_landmark_5, this->normed_template, Size(512, 512));
    const int crop_size[2] = {crop_img.cols, crop_img.rows};
    box_mask = create_static_box_mask(crop_size, this->FACE_MASK_BLUR, this->FACE_MASK_PADDING);

    vector<cv::Mat> bgrChannels(3);
    split(crop_img, bgrChannels);
    for (int c = 0; c < 3; c++)
    {
        bgrChannels[c].convertTo(bgrChannels[c], CV_32FC1, 1 / (255.0*0.5), -1.0);
    }

    const int image_area = this->input_height * this->input_width;
    this->input_image.resize(3 * image_area);
    size_t single_chn_size = image_area * sizeof(float);
    memcpy(this->input_image.data(), (float *)bgrChannels[2].data, single_chn_size);    ///rgb顺序
    memcpy(this->input_image.data() + image_area, (float *)bgrChannels[1].data, single_chn_size);
    memcpy(this->input_image.data() + image_area * 2, (float *)bgrChannels[0].data, single_chn_size);
}

Mat FaceEnhance::process(Mat target, const vector<Point2f> target_landmark_5)
{
    Mat affine_matrix;
    Mat box_mask;
    this->preprocess(target, target_landmark_5, affine_matrix, box_mask);

    std::vector<int64_t> input_img_shape = {1, 3, this->input_height, this->input_width};
    Value input_tensor_ = Value::CreateTensor<float>(memory_info_handler, this->input_image.data(), this->input_image.size(), input_img_shape.data(), input_img_shape.size());

    vector<Value> ort_outputs = this -> forward(input_tensor_);

    float* pdata = ort_outputs[0].GetTensorMutableData<float>();
    std::vector<int64_t> outs_shape = ort_outputs[0].GetTensorTypeAndShapeInfo().GetShape();
    const int out_h = outs_shape[2];
    const int out_w = outs_shape[3];
    const int channel_step = out_h * out_w;
    Mat rmat(out_h, out_w, CV_32FC1, pdata);
    Mat gmat(out_h, out_w, CV_32FC1, pdata + channel_step);
    Mat bmat(out_h, out_w, CV_32FC1, pdata + 2 * channel_step);
    rmat.setTo(-1, rmat < -1);
    rmat.setTo(1, rmat > 1);
    rmat = (rmat+1)*0.5;
    gmat.setTo(-1, gmat < -1);
    gmat.setTo(1, gmat > 1);
    gmat = (gmat+1)*0.5;
    bmat.setTo(-1, bmat < -1);
    bmat.setTo(1, bmat > 1);
    bmat = (bmat+1)*0.5;

    rmat *= 255.f;
    gmat *= 255.f;
    bmat *= 255.f;
    rmat.setTo(0, rmat < 0);
    rmat.setTo(255, rmat > 255);
    gmat.setTo(0, gmat < 0);
    gmat.setTo(255, gmat > 255);
    bmat.setTo(0, bmat < 0);
    bmat.setTo(255, bmat > 255);

    vector<Mat> channel_mats(3);
    channel_mats[0] = bmat;
    channel_mats[1] = gmat;
    channel_mats[2] = rmat;
    Mat result;
    merge(channel_mats, result);
    result.convertTo(result, CV_8UC3);

    box_mask.setTo(0, box_mask < 0);
    box_mask.setTo(1, box_mask > 1);
    Mat paste_frame = paste_back(target, result, box_mask, affine_matrix);
    Mat dst = blend_frame(target, paste_frame);
    return dst;
}