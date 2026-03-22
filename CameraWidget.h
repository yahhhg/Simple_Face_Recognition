#pragma once     // 防止重复包含 VS2022编译器的编译器命令
#ifndef CAMERAWIDGET_H  // 防止重复包含，等价与#pragma once
#define CAMERAWIDGET_H

#include <QWidget>
//定时器
#include <QTimer>
//标签类，可以显示图片或文本
#include <QLabel>
// 包含OpenCV头文件 opencv核心类
#include <opencv2/opencv.hpp>
// 人脸检测类
#include "FaceDetector.h" 

// 前置声明UI类（由Qt的UIC自动生成）
namespace Ui {
    class CameraWidgetClass;
}

class CameraWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CameraWidget(QWidget* parent = nullptr);
    ~CameraWidget();

private slots:
    // 定时器触发的槽函数：用于定时读取摄像头帧，刷新显示从而模拟摄像头视频
    void onTimerUpdate();

private:
    // 初始化摄像头
    bool initCamera();

    // 将OpenCV的Mat转换为Qt的QImage
    QImage matToQImage(const cv::Mat& mat);

    Ui::CameraWidgetClass* ui;          // UI指针（关联UI文件）

    // OpenCV对象, 用于读取摄像头
    cv::VideoCapture m_capture;    // OpenCV摄像头捕获对象

    // 人脸检测器对象
    FaceDetector m_faceDetector;
    
    QTimer* m_timer;                // 定时器：用于定时刷新画面
    QLabel* m_displayLabel;         // 显示画面的QLabel（你在UI中放置）


};

#endif // CAMERAWIDGET_H