//
// Created by Tony on 2024/8/30.
//
#include "../../include/faceSwap/FaceSwap.h"

using namespace cv;
using namespace std;
using namespace Ort;

FaceSwap::FaceSwap(string modelPath, string modePath2, const char* logId, const char* provider): OnnxRuntimeBase(modelPath, logId, provider)
{
    this->input_height = input_node_dims[0][2];
    this->input_width = input_node_dims[0][3];

    const int length = this->len_feature*this->len_feature;
    this->model_matrix = new float[length];
    cout<<"start read model_matrix.bin"<<endl;
    FILE* fp = fopen(modePath2.c_str(), "rb");
    fread(this->model_matrix, sizeof(float), length, fp);//导入数据
    fclose(fp);//关闭文件
    cout<<"read model_matrix.bin finish"<<endl;

    ////在这里就直接定义了，没有像python程序里的那样normed_template = TEMPLATES.get(template) * crop_size
    this->normed_template.emplace_back(Point2f(46.29459968, 51.69629952));
    this->normed_template.emplace_back(Point2f(81.53180032, 51.50140032));
    this->normed_template.emplace_back(Point2f(64.02519936, 71.73660032));
    this->normed_template.emplace_back(Point2f(49.54930048, 92.36550016));
    this->normed_template.emplace_back(Point2f(78.72989952, 92.20409984));
}

FaceSwap::~FaceSwap()
{
    delete[] this->model_matrix;
    this->model_matrix = nullptr;
    this->normed_template.clear();
}

void FaceSwap::preprocess(Mat target, const vector<Point2f> face_landmark_5, const vector<float> source_face_embedding, Mat& affine_matrix, Mat& box_mask)
{
    Mat crop_img;
    affine_matrix = warp_face_by_face_landmark_5(target, crop_img, face_landmark_5, this->normed_template, Size(128, 128));
    const int crop_size[2] = {crop_img.cols, crop_img.rows};
    box_mask = create_static_box_mask(crop_size, this->FACE_MASK_BLUR, this->FACE_MASK_PADDING);

    vector<cv::Mat> bgrChannels(3);
    split(crop_img, bgrChannels);
    for (int c = 0; c < 3; c++)
    {
        bgrChannels[c].convertTo(bgrChannels[c], CV_32FC1, 1 / (255.0*this->INSWAPPER_128_MODEL_STD[c]), -this->INSWAPPER_128_MODEL_MEAN[c]/this->INSWAPPER_128_MODEL_STD[c]);
    }

    const int image_area = this->input_height * this->input_width;
    this->input_image.resize(3 * image_area);
    size_t single_chn_size = image_area * sizeof(float);
    memcpy(this->input_image.data(), (float *)bgrChannels[2].data, single_chn_size);    ///rgb顺序
    memcpy(this->input_image.data() + image_area, (float *)bgrChannels[1].data, single_chn_size);
    memcpy(this->input_image.data() + image_area * 2, (float *)bgrChannels[0].data, single_chn_size);

    float linalg_norm = 0;
    for(int i=0;i<this->len_feature;i++)
    {
        linalg_norm += powf(source_face_embedding[i], 2);
    }
    linalg_norm = sqrt(linalg_norm);
    this->input_embedding.resize(this->len_feature);
    for(int i=0;i<this->len_feature;i++)
    {
        float sum=0;
        for(int j=0;j<this->len_feature;j++)
        {
            sum += (source_face_embedding[j]*this->model_matrix[j*this->len_feature+i]);
        }
        this->input_embedding[i] = sum/linalg_norm;
    }
}

Mat FaceSwap::process(Mat target, const vector<float> source_face_embedding, const vector<Point2f> target_landmark_5)
{
    Mat affine_matrix;
    Mat box_mask;
    this->preprocess(target, target_landmark_5, source_face_embedding, affine_matrix, box_mask);

    std::vector<Ort::Value> inputs_tensor;
    std::vector<int64_t> input_img_shape = {1, 3, this->input_height, this->input_width};
    inputs_tensor.emplace_back(Value::CreateTensor<float>(memory_info_handler, this->input_image.data(), this->input_image.size(), input_img_shape.data(), input_img_shape.size()));
    std::vector<int64_t> input_embedding_shape = {1, this->len_feature};
    inputs_tensor.emplace_back(Value::CreateTensor<float>(memory_info_handler, this->input_embedding.data(), this->input_embedding.size(), input_embedding_shape.data(), input_embedding_shape.size()));

    Ort::RunOptions runOptions;
    vector<Value> ort_outputs = this -> ort_session.Run(runOptions, this->input_names.data(), inputs_tensor.data(), inputs_tensor.size(), this->output_names.data(), output_names.size());

    float* pdata = ort_outputs[0].GetTensorMutableData<float>();
    std::vector<int64_t> outs_shape = ort_outputs[0].GetTensorTypeAndShapeInfo().GetShape();
    const int out_h = outs_shape[2];
    const int out_w = outs_shape[3];
    const int channel_step = out_h * out_w;
    Mat rmat(out_h, out_w, CV_32FC1, pdata);
    Mat gmat(out_h, out_w, CV_32FC1, pdata + channel_step);
    Mat bmat(out_h, out_w, CV_32FC1, pdata + 2 * channel_step);
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

    box_mask.setTo(0, box_mask < 0);
    box_mask.setTo(1, box_mask > 1);
    Mat dst = paste_back(target, result, box_mask, affine_matrix);
    return dst;
}