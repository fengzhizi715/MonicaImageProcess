//
// Created by Tony on 2024/8/22.
//

#include "../../include/utils/Utils.h"
#include "../../include/faceSwap/Face68Landmarks.h"

Face68Landmarks::Face68Landmarks(string modelPath, const char* logId, const char* provider): OnnxRuntimeBase(modelPath, logId, provider)
{
    this->input_height = input_node_dims[0][2];
    this->input_width = input_node_dims[0][3];
}

void Face68Landmarks::preprocess(Mat src, const Bbox bounding_box)
{
    float sub_max = max(bounding_box.xmax - bounding_box.xmin, bounding_box.ymax - bounding_box.ymin);
    const float scale = 195.f / sub_max;
    const float translation[2] = {(256.f - (bounding_box.xmax + bounding_box.xmin) * scale) * 0.5f, (256.f - (bounding_box.ymax + bounding_box.ymin) * scale) * 0.5f};
    ////python程序里的warp_face_by_translation函数////
    Mat affine_matrix = (Mat_<float>(2, 3) << scale, 0.f, translation[0], 0.f, scale, translation[1]);
    Mat crop_img;
    warpAffine(src, crop_img, affine_matrix, Size(256, 256));
    ////python程序里的warp_face_by_translation函数////
    cv::invertAffineTransform(affine_matrix, this->inv_affine_matrix);

    vector<cv::Mat> bgrChannels(3);
    split(crop_img, bgrChannels);
    for (int c = 0; c < 3; c++)
    {
        bgrChannels[c].convertTo(bgrChannels[c], CV_32FC1, 1 / 255.0);
    }

    const int image_area = this->input_height * this->input_width;
    this->input_image.resize(3 * image_area);
    size_t single_chn_size = image_area * sizeof(float);
    memcpy(this->input_image.data(), (float *)bgrChannels[0].data, single_chn_size);
    memcpy(this->input_image.data() + image_area, (float *)bgrChannels[1].data, single_chn_size);
    memcpy(this->input_image.data() + image_area * 2, (float *)bgrChannels[2].data, single_chn_size);
}

vector<Point2f> Face68Landmarks::detect(Mat src, const Bbox bounding_box, vector<Point2f> &face_landmark_5of68)
{
    this->preprocess(src, bounding_box);

    std::vector<int64_t> input_img_shape = {1, 3, this->input_height, this->input_width};
    Value input_tensor_ = Value::CreateTensor<float>(memory_info_handler, this->input_image.data(), this->input_image.size(), input_img_shape.data(), input_img_shape.size());
    vector<Value> ort_outputs = this -> forward(input_tensor_);

    float *pdata = ort_outputs[0].GetTensorMutableData<float>(); /// 形状是(1, 68, 3), 每一行的长度是3，表示一个关键点坐标x,y和置信度
    const int num_points = ort_outputs[0].GetTensorTypeAndShapeInfo().GetShape()[1];
    vector<Point2f> face_landmark_68(num_points);
    for (int i = 0; i < num_points; i++)
    {
        float x = pdata[i * 3] / 64.0 * 256.0;
        float y = pdata[i * 3 + 1] / 64.0 * 256.0;
        face_landmark_68[i] = Point2f(x, y);
    }
    vector<Point2f> face68landmarks;
    cv::transform(face_landmark_68, face68landmarks, this->inv_affine_matrix);

    ////python程序里的convert_face_landmark_68_to_5函数////
    face_landmark_5of68.resize(5);
    float x = 0, y = 0;
    for (int i = 36; i < 42; i++) /// left_eye
    {
        x += face68landmarks[i].x;
        y += face68landmarks[i].y;
    }
    x /= 6;
    y /= 6;
    face_landmark_5of68[0] = Point2f(x, y); /// left_eye

    x = 0, y = 0;
    for (int i = 42; i < 48; i++) /// right_eye
    {
        x += face68landmarks[i].x;
        y += face68landmarks[i].y;
    }
    x /= 6;
    y /= 6;
    face_landmark_5of68[1] = Point2f(x, y); /// right_eye

    face_landmark_5of68[2] = face68landmarks[30]; /// nose
    face_landmark_5of68[3] = face68landmarks[48]; /// left_mouth_end
    face_landmark_5of68[4] = face68landmarks[54]; /// right_mouth_end
    ////python程序里的convert_face_landmark_68_to_5函数////
    return face68landmarks;
}