//
// Created by Tony on 2025/3/25.
//

#ifndef MONICAIMAGEPROCESS_GLOBALRESOURCE_H
#define MONICAIMAGEPROCESS_GLOBALRESOURCE_H

using namespace std;

class GlobalResource {
public:
    GlobalResource(string modelPath);
    void processImage();
    void initSketchDrawingModel();

private:
    string modelPath;
};

#endif //MONICAIMAGEPROCESS_GLOBALRESOURCE_H
