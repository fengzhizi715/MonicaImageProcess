//
// Created by Tony on 2024/8/22.
//

#include "../../include/utils/Utils.h"
#include "../../include/faceSwap/Yolov8Face.h"

Yolov8Face::Yolov8Face(string modelPath, const char* logId, const char* provider, const float conf_thresh, const float iou_thresh): OnnxRuntimeBase(modelPath, logId, provider)
{
    this->input_height = input_node_dims[0][2];
    this->input_width = input_node_dims[0][3];
    this->conf_threshold = conf_thresh;
    this->iou_threshold = iou_thresh;
}

void Yolov8Face::preprocess(Mat src)
{
    const int height = src.rows;
    const int width = src.cols;
    Mat temp = src.clone();
    if (height > this->input_height || width > this->input_width)
    {
        const float scale = std::min((float)this->input_height / height, (float)this->input_width / width);
        Size new_size = Size(int(width * scale), int(height * scale));
        resize(src, temp, new_size);
    }
    this->ratio_height = (float)height / temp.rows;
    this->ratio_width = (float)width / temp.cols;
    Mat input;
    copyMakeBorder(temp, input, 0, this->input_height - temp.rows, 0, this->input_width - temp.cols, BORDER_CONSTANT, 0);

    vector<cv::Mat> bgrChannels(3);
    split(input, bgrChannels);
    for (int c = 0; c < 3; c++)
    {
        bgrChannels[c].convertTo(bgrChannels[c], CV_32FC1, 1 / 128.0, -127.5 / 128.0);
    }

    const int image_area = this->input_height * this->input_width;
    this->input_image.resize(3 * image_area);
    size_t single_chn_size = image_area * sizeof(float);
    memcpy(this->input_image.data(), (float *)bgrChannels[0].data, single_chn_size);
    memcpy(this->input_image.data() + image_area, (float *)bgrChannels[1].data, single_chn_size);
    memcpy(this->input_image.data() + image_area * 2, (float *)bgrChannels[2].data, single_chn_size);
}

//只返回检测框,因为在下游的模块里,置信度和5个关键点这两个信息在后续的模块里没有用到
void Yolov8Face::detect(Mat src, std::vector<Bbox> &boxes)
{
    this->preprocess(src);
    std::vector<int64_t> input_img_shape = {1, 3, this->input_height, this->input_width};
    Value input_tensor_ = Value::CreateTensor<float>(memory_info_handler, this->input_image.data(), this->input_image.size(), input_img_shape.data(), input_img_shape.size());
    vector<Value> ort_outputs = this -> forward(input_tensor_);

    float *pdata = ort_outputs[0].GetTensorMutableData<float>(); /// 形状是(1, 20, 8400),不考虑第0维batchsize，每一列的长度20,前4个元素是检测框坐标(cx,cy,w,h)，第4个元素是置信度，剩下的15个元素是5个关键点坐标x,y和置信度
    const int num_box = ort_outputs[0].GetTensorTypeAndShapeInfo().GetShape()[2];
    vector<Bbox> bounding_box_raw;
    vector<float> score_raw;
    for (int i = 0; i < num_box; i++)
    {
        const float score = pdata[4 * num_box + i];
        if (score > this->conf_threshold)
        {
            float xmin = (pdata[i] - 0.5 * pdata[2 * num_box + i]) * this->ratio_width;            // (cx,cy,w,h)转到(x,y,w,h)并还原到原图
            float ymin = (pdata[num_box + i] - 0.5 * pdata[3 * num_box + i]) * this->ratio_height; // (cx,cy,w,h)转到(x,y,w,h)并还原到原图
            float xmax = (pdata[i] + 0.5 * pdata[2 * num_box + i]) * this->ratio_width;            // (cx,cy,w,h)转到(x,y,w,h)并还原到原图
            float ymax = (pdata[num_box + i] + 0.5 * pdata[3 * num_box + i]) * this->ratio_height; // (cx,cy,w,h)转到(x,y,w,h)并还原到原图
            // 坐标的越界检查保护，可以添加一下
            bounding_box_raw.emplace_back(Bbox{xmin, ymin, xmax, ymax});
            score_raw.emplace_back(score);
            // 剩下的5个关键点坐标的计算,暂时不写,因为在下游的模块里没有用到5个关键点坐标信息
        }
    }
    vector<int> keep_inds = nms(bounding_box_raw, score_raw, this->iou_threshold);
    const int keep_num = keep_inds.size();
    boxes.clear();
    boxes.resize(keep_num);
    for (int i = 0; i < keep_num; i++)
    {
        const int ind = keep_inds[i];
        boxes[i] = bounding_box_raw[ind];
    }
}