//
// Created by Tony on 2024/7/26.
//

#include "../../include/faceDetect/FaceDetect.h"

FaceDetect::FaceDetect(string faceProto,string faceModel,string ageProto,string ageModel,string genderProto,string genderModel) {

    // Load Network
    ageNet = readNet(ageModel, ageProto);
    genderNet = readNet(genderModel, genderProto);
    faceNet = readNet(faceModel, faceProto);

    cout << "Using CPU device" << endl;
    ageNet.setPreferableBackend(DNN_TARGET_CPU);
    genderNet.setPreferableBackend(DNN_TARGET_CPU);
    faceNet.setPreferableBackend(DNN_TARGET_CPU);

    MODEL_MEAN_VALUES = Scalar(78.4263377603, 87.7689143744, 114.895847746);
    ageList = {"(0-2)", "(4-6)", "(8-12)", "(15-20)", "(25-32)",
                              "(38-43)", "(48-53)", "(60-100)"};

    genderList = {"Male", "Female"};
}

void FaceDetect::inferImage(Mat& src, Mat& dst) {

    int padding = 20;
    vector<vector<int>> bboxes;

    tie(dst, bboxes) = getFaceBox(faceNet, src, 0.7);

    if(bboxes.size() == 0) {
        cout << "No face detected..." << endl;
        dst = src;
        return;
    }

    for (auto it = begin(bboxes); it != end(bboxes); ++it) {
        Rect rec(it->at(0) - padding, it->at(1) - padding, it->at(2) - it->at(0) + 2*padding, it->at(3) - it->at(1) + 2*padding);
        Mat face = src(rec); // take the ROI of box on the frame

        Mat blob;
        blob = blobFromImage(face, 1, Size(227, 227), MODEL_MEAN_VALUES, false);
        genderNet.setInput(blob);
        // string gender_preds;
        vector<float> genderPreds = genderNet.forward();
        // printing gender here
        // find max element index
        // distance function does the argmax() work in C++
        int max_index_gender = std::distance(genderPreds.begin(), max_element(genderPreds.begin(), genderPreds.end()));
        string gender = genderList[max_index_gender];
        cout << "Gender: " << gender << endl;

        /* // Uncomment if you want to iterate through the gender_preds vector
        for(auto it=begin(gender_preds); it != end(gender_preds); ++it) {
          cout << *it << endl;
        }
        */

        ageNet.setInput(blob);
        vector<float> agePreds = ageNet.forward();
        /* // uncomment below code if you want to iterate through the age_preds
         * vector
        cout << "PRINTING AGE_PREDS" << endl;
        for(auto it = age_preds.begin(); it != age_preds.end(); ++it) {
          cout << *it << endl;
        }
        */

        // finding maximum indicd in the age_preds vector
        int max_indice_age = std::distance(agePreds.begin(), max_element(agePreds.begin(), agePreds.end()));
        string age = ageList[max_indice_age];
        cout << "Age: " << age << endl;
        string label = gender + ", " + age; // label
        cv::putText(dst, label, Point(it->at(0), it->at(1) -15), cv::FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 255, 0), 4, cv::LINE_AA);
    }
}

tuple<Mat, vector<vector<int>>> FaceDetect::getFaceBox(Net net, Mat &frame, double conf_threshold) {
    Mat frameOpenCVDNN = frame.clone();
    int frameHeight = frameOpenCVDNN.rows;
    int frameWidth = frameOpenCVDNN.cols;
    double inScaleFactor = 1.0;
    Size size = Size(300, 300);
    Scalar meanVal = Scalar(104, 117, 123);

    cv::Mat inputBlob;
    inputBlob = cv::dnn::blobFromImage(frameOpenCVDNN, inScaleFactor, size, meanVal, true, false);

    net.setInput(inputBlob, "data");
    cv::Mat detection = net.forward("detection_out");

    cv::Mat detectionMat(detection.size[2], detection.size[3], CV_32F, detection.ptr<float>());

    vector<vector<int>> bboxes;

    for(int i = 0; i < detectionMat.rows; i++)
    {
        float confidence = detectionMat.at<float>(i, 2);
        if(confidence > conf_threshold)
        {
            int x1 = static_cast<int>(detectionMat.at<float>(i, 3) * frameWidth);
            int y1 = static_cast<int>(detectionMat.at<float>(i, 4) * frameHeight);
            int x2 = static_cast<int>(detectionMat.at<float>(i, 5) * frameWidth);
            int y2 = static_cast<int>(detectionMat.at<float>(i, 6) * frameHeight);
            vector<int> box = {x1, y1, x2, y2};
            bboxes.push_back(box);
            rectangle(frameOpenCVDNN, Point(x1, y1), Point(x2, y2), Scalar(0, 0, 255), 2, 8);
        }
    }

    return make_tuple(frameOpenCVDNN, bboxes);
}