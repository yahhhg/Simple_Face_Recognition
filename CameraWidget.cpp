#include "CameraWidget.h"
#include "ui_CameraWidget.h"  // 包含UI文件生成的头文件
//消息框
#include <QMessageBox>

CameraWidget::CameraWidget(QWidget* parent)
    //构造函数初始化法
    : QWidget(parent)
    , ui(new Ui::CameraWidgetClass)
    , m_timer(new QTimer(this))
    , m_displayLabel(nullptr)
{
    // 关联UI文件
    ui->setupUi(this);

    // 获取UI中的QLabel控件
    m_displayLabel = ui->displayLabel;

    // 初始化摄像头
    if (!initCamera()) {
        // 显示错误消息框
        QMessageBox::critical(this, "错误", "无法打开摄像头！");
        return;
    }

    // 初始化人脸检测器（使用已经创建的独立的 FaceDetector 类）
    //opencv的预编译模型路径
    QString xmlPath = "../../../haarcascade_frontalface_default.xml";
    if (!m_faceDetector.init(xmlPath)) {
        QMessageBox::critical(this, "错误", "无法加载人脸检测模型！\n请确保 haarcascade_frontalface_default.xml 在项目目录下。");
        qDebug() << "无法加载人脸检测模型！请确保 haarcascade_frontalface_default.xml 在项目目录下。";
        return;
    }

    // 连接定时器信号到槽函数：每30ms触发一次（约33fps）
    connect(m_timer, &QTimer::timeout, this, &CameraWidget::onTimerUpdate);

    // 启动定时器  0.3秒触发一次，刷新显示
    m_timer->start(30);
}

CameraWidget::~CameraWidget()
{
    // 释放资源：停止定时器 + 释放摄像头
    if (m_timer->isActive()) {
        m_timer->stop();
    }
    if (m_capture.isOpened()) {
        m_capture.release();
    }
    delete ui;
}

// 初始化摄像头
bool CameraWidget::initCamera()
{
    // 打开默认摄像头（0表示第一个摄像头）
    /*
    这里的 int 是摄像头设备索引：
    通常 0 代表系统默认摄像头，1 代表第二个接入的摄像头，
    以此类推，是 OpenCV 给设备分配的序号，方便快速调用
    */
    if (!m_capture.open(0)) {
        return false;
    }

    // 可选：设置摄像头分辨率（根据实际情况调整）
    //set用于设置某个属性的值 图像分辨率指的是图像中包含的像素数量，通常用「宽度 × 高度」的形式表示
    m_capture.set(cv::CAP_PROP_FRAME_WIDTH, 640);               // 设置摄像头的宽度 横向有多少像素
    m_capture.set(cv::CAP_PROP_FRAME_HEIGHT, 480);               // 设置摄像头的高度 纵向有多少像素

    return true;
}

//定时器槽函数 每30ms调用一次
void CameraWidget::onTimerUpdate()
{
    //创建新位图矩阵 Mat对象 
    cv::Mat frame;

    // 从摄像头读取一帧 摄像头必须已启动 将当前的一帧保存在图frame中
    if (!m_capture.read(frame)) {
        return;
    }

    /* 使用独立的 FaceDetector 类进行人脸检测和绘制 */
    // 检测人脸
    std::vector<cv::Rect> faces = m_faceDetector.detectFaces(frame);
    // 在原图上绘制人脸框
    m_faceDetector.drawFaceRects(frame, faces);


    // 将OpenCV的BGR格式转换为Qt的RGB格式 使用QT接口 创建QImage对象
    QImage qImage = matToQImage(frame);

    // 显示到QLabel上（自适应QLabel大小）
    if (m_displayLabel) {
        m_displayLabel->setPixmap(QPixmap::fromImage(qImage).scaled(  //将像素图缩放至给定的size
            m_displayLabel->size(),
            Qt::KeepAspectRatio, //像素图将按比例缩放到size 内尽可能大的矩形内，并保留纵横比。
            Qt::SmoothTransformation  //平滑 使图像质量变好 缩放后的图片边缘更平滑
        ));
    }
}

// 将OpenCV的矩阵图Mat转换为QImage
QImage CameraWidget::matToQImage(const cv::Mat& mat)
{
    // 根据OpenCV的Mat类型转换为Qt的QImage
    switch (mat.type()) {
        // 8位3通道（BGR）彩色图：OpenCV默认格式
    case CV_8UC3: {
        // 转换BGR到RGB
        cv::Mat rgbMat;
        //颜色空间转换 将指定矩阵转换为指定转换码指定的颜色通道，最后赋给rgbMat
        cv::cvtColor(mat, rgbMat, cv::COLOR_BGR2RGB);
        // 构造QImage（注意：数据需拷贝，避免Mat释放后悬空）
        return QImage(
            rgbMat.data,
            rgbMat.cols,
            rgbMat.rows,
            //step是 OpenCV 里的「步长」，意思是图像每一行像素占用的总字节数
            //为 size_t 类型，无符号长整型，通常用于表示内存地址，长度为 64 位。
            /* 因为有些图像为了内存读写效率，每一行的末尾会加几个冗余的填充字节，
            不是正好「宽度 × 每个像素的字节数」。
            如果不传step，QImage 会按默认的方式计算行长度，遇到带填充的图像，就会显示错位、花屏 */
            static_cast<int>(rgbMat.step),   
            //告诉 QImage像素数据的格式 颜色通道及各占用位数
            QImage::Format_RGB888
        ).copy(); // copy()确保数据独立 将创建一个副本 避免 rgbMat 释放后数据也跟着释放,从而无法使用返回值
    }
      // 8位单通道（灰度图）无需转换，直接构造QImage
    case CV_8UC1: {
        return QImage(
            mat.data,
            mat.cols,
            mat.rows,
            static_cast<int>(mat.step),
            QImage::Format_Grayscale8   //代表灰度图格式
        ).copy();
    }
    default:
        return QImage();  // 返回空图片
    }
}