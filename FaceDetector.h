#ifndef FACEDETECTOR_H
#define FACEDETECTOR_H

// OpenCV 头文件，核心函数库
#include <opencv2/opencv.hpp>
//字符串处理头文件
#include <QString>
//C++标准库头文件 动态数组
#include <vector>

/*
 人脸检测器类
 封装了基于 OpenCV Haar 级联分类器的人脸检测功能，利用 opencv 内置预编译的模型，
 "D:\opencv\opencv\sources\data\haarcascades\haarcascade_frontalface_default.xml"
 检测到的人脸会以矩形框的形式返回。
 */
class FaceDetector
{
public:
    explicit FaceDetector();
    ~FaceDetector();

    /*
     初始化人脸检测器（加载 Haar 级联分类器模型）
     传入模型文件的路径
     返回检查初始化是否成功
     */
    bool init(const QString& xmlPath);

    /*
     检测图像中的人脸
     输入 OpenCV 图像（BGR 格式）
     返回检测到的人脸矩形框列表
     */
    std::vector<cv::Rect> detectFaces(const cv::Mat& frame);

    /*
     在图像上绘制人脸矩形框
     frame 输入/输出的 OpenCV 图像（会直接在图上绘制）
     faces 人脸矩形框列表
     color 矩形框颜色（默认红色：BGR格式）
     thickness 线条宽度（默认2）
     */
    void drawFaceRects(cv::Mat& frame,
        const std::vector<cv::Rect>& faces,
        const cv::Scalar& color = cv::Scalar(0, 0, 255),
        int thickness = 2);

private:
    //OpenCV 中专门用于加载和使用级联分类器模型的类,
    // 从图像 / 视频中快速检测特定目标（如人脸、眼睛、行人等）
    cv::CascadeClassifier m_faceCascade; // Haar 级联分类器对象，自动释放
    // 初始化标志位,用于检测初始化是否成功
    bool m_isInitialized;                 
};

#endif // FACEDETECTOR_H