#include "FaceDetector.h"
//用于输出调试信息类
#include <QDebug>

FaceDetector::FaceDetector()
    : m_isInitialized(false)
{
}

FaceDetector::~FaceDetector()
{
    // 无需手动释放 CascadeClassifier，析构时会自动处理
}

//初始化人脸检测器，参数是模型文件路径 opencv中的预编译模型文件 XML
//加载预编译模型文件
bool FaceDetector::init(const QString& xmlPath)
{
    //根据标识符判断是否已经初始化
    if (m_isInitialized) {
        qWarning() << "FaceDetector 已经初始化过了";
        return true;
    }

    // 加载 Haar 级联分类器模型 
    if (!m_faceCascade.load(xmlPath.toStdString())) {
        qCritical() << "无法加载 Haar 模型文件：" << xmlPath;
        return false;
    }
    
    //标识符设置为已初始化
    m_isInitialized = true;
    qInfo() << "FaceDetector 初始化成功";
    return true;
}

// 检测人脸 传入捕获的帧图像 返回人脸矩形框列表
std::vector<cv::Rect> FaceDetector::detectFaces(const cv::Mat& frame)
{
    //锥形款列表
    std::vector<cv::Rect> faces;

    // 检测器未初始化 或 帧图像为空 直接返回
    if (!m_isInitialized || frame.empty()) {
        return faces;
    }

    // 转为灰度图（Haar 分类器在灰度图上检测效率更高）
    cv::Mat grayFrame;
    //转换颜色空间：BGR 转为灰度图
    cv::cvtColor(frame, grayFrame, cv::COLOR_BGR2GRAY); //根据颜色空间参数 cv::COLOR_BGR2GRAY

    // 直方图均衡化（增强图像对比度，提高检测率）
    cv::equalizeHist(grayFrame, grayFrame);

    // 执行人脸检测 调用 OpenCV 的 CascadeClassifier 类的 detectMultiScale 方法
    m_faceCascade.detectMultiScale(
        grayFrame,                // 输入灰度图
        faces,                    // 输出的人脸矩形框列表
        1.1,                      // 缩放比例（每次检测后图像缩小 10%）
        3,                        // 最小邻居数（越高检测越严格，误检越少）
        0 | cv::CASCADE_SCALE_IMAGE,
        cv::Size(30, 30)          // 最小人脸尺寸（小于此尺寸的人脸忽略）
    );

    return faces;
}

// 在帧图像上绘制人脸矩形框
void FaceDetector::drawFaceRects(cv::Mat& frame,
    const std::vector<cv::Rect>& faces,
    const cv::Scalar& color,  //采用默认颜色 红
    int thickness)            // 采用默认线宽 2
{
    // 帧图像为空 直接返回
    if (frame.empty()) {
        return;
    }

    // 遍历所有人脸矩形框，在图上绘制 
    // 把这一帧里检测到的所有人脸，一个个都画出来
    for (const cv::Rect& face : faces) {
        // 在图上绘制
        cv::rectangle(frame, face, color, thickness);
    }
}