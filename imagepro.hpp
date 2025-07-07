#include <enumerate.h>
#include <opencv2/opencv.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/stitching.hpp>
#include <QFileDialog>
#include <QImage>
#include <QPainter>
#include <QGraphicsView>
#include <QSettings>
#include <tools.hpp>
#include <QString>
static Qt::GlobalColor PaddingColorType2QtColor(PaddingColorType type)
{
    switch (type)
    {
    case PT_TRANSPARENT:
        return Qt::transparent;
    case PT_BLACK:
        return Qt::black;
    case PT_WHITE:
        return Qt::white;
    case PT_DARKGRAY:
        return Qt::darkGray;
    case PT_GRAY:
        return Qt::gray;
    case PT_LIGHTGRAY:
        return Qt::lightGray;
    case PT_RED:
        return Qt::red;
    case PT_GREEN:
        return Qt::green;
    case PT_BLUE:
        return Qt::blue;
    case PT_CYAN:
        return Qt::cyan;
    case PT_MAGENTA:
        return Qt::magenta;
    case PT_YELLOW:
        return Qt::yellow;
    case PT_DARKRED:
        return Qt::darkRed;
    case PT_DARKGREEN:
        return Qt::darkGreen;
    case PT_DARKBLUE:
        return Qt::darkBlue;
    case PT_DARKCYAN:
        return Qt::darkCyan;
    case PT_DARKMAGENTA:
        return Qt::darkMagenta;
    case PT_DARKYELLOW:
        return Qt::darkYellow;
    default:
        return Qt::black; // 默认返回黑色
    }
}

static cv::InterpolationFlags InterpolationType2CVInterpolation(InterpolationType type)
{
    switch (type)
    {
    case IT_INTER_LINEAR:
        return cv::INTER_LINEAR;
    case IT_INTER_NEAREST:
        return cv::INTER_NEAREST;
    case IT_INTER_AREA:
        return cv::INTER_AREA;
    case IT_INTER_CUBIC:
        return cv::INTER_CUBIC;
    case IT_INTER_LANCZOS4:
        return cv::INTER_LANCZOS4;
    default:
        return cv::INTER_LINEAR; // 默认返回线性插值
    }
}

static cv::Mat ResizeByWidth(const cv::Mat &input, int targetWidth, InterpolationType interpolationType)
{
    if (input.empty())
    {
        return cv::Mat(); // 如果输入为空，返回空矩阵
    }

    // 计算等比缩放后的宽度（保持宽高比）
    double scaleFactor = static_cast<double>(targetWidth) / input.cols;
    int targetHeight = static_cast<int>(input.rows * scaleFactor);

    cv::Mat output;
    cv::resize(
        input,
        output,
        cv::Size(targetWidth, targetHeight), // 按比例计算新尺寸
        0,
        0,
        InterpolationType2CVInterpolation(interpolationType));
    return output;
}

static cv::Mat ResizeByHeight(const cv::Mat &input, int targetHeight, InterpolationType interpolationType)
{
    if (input.empty())
    {
        return cv::Mat(); // 如果输入为空，返回空矩阵
    }

    // 计算等比缩放后的高度（保持宽高比）
    double scaleFactor = static_cast<double>(targetHeight) / input.rows;
    int targetWidth = static_cast<int>(input.cols * scaleFactor);

    cv::Mat output;
    resize(input, output, cv::Size(targetWidth, targetHeight), 0, 0, InterpolationType2CVInterpolation(interpolationType));
    // 处理逻辑
    return output;
}

/**
 * @brief 保存 QGraphicsView 的内容为图片
 * @param view 要保存的 QGraphicsView 对象
 * @param fileName 文件名（如果为空，则弹出文件对话框）
 * @return 是否保存成功
 */
static bool saveGraphicsViewToImage(QGraphicsView *view, const QString &fileName, const Qt::GlobalColor &backgroundColor = Qt::white)
{
    QSettings settings("iCloudWar", "ImageSplicing");
    QString lastDir = settings.value("LastOpenDirectory", QDir::homePath()).toString();
    if (!view || !view->scene())
        return false;

    // 获取场景的边界（所有 items 的矩形范围）
    QRectF sceneRect = view->scene()->sceneRect();

    // 创建一个与场景大小相同的 QImage
    QImage image(sceneRect.size().toSize(), QImage::Format_ARGB32);
    image.fill(backgroundColor); // 背景填充指定颜色

    // 使用 QPainter 将场景渲染到 QImage
    QPainter painter(&image);
    view->scene()->render(&painter, QRectF(), sceneRect);
    painter.end();

    // 如果没有提供文件名，弹出文件对话框
    QString saveFileName = fileName;

    saveFileName = QFileDialog::getSaveFileName(
        view,
        "保存图片",
        lastDir + "/" + fileName, // 默认文件名
        "PNG 图片 (*.png);;JPEG 图片 (*.jpg *.jpeg)");
    if (saveFileName.isEmpty())
        return false;

    // 根据文件后缀选择保存格式
    if (saveFileName.endsWith(".png", Qt::CaseInsensitive))
    {
        return image.save(saveFileName, "PNG");
    }
    else if (saveFileName.endsWith(".jpg", Qt::CaseInsensitive) || saveFileName.endsWith(".jpeg", Qt::CaseInsensitive))
    {
        return image.save(saveFileName, "JPEG", 100); // JPEG 质量（0-100）
    }
    else
    {
        // 默认保存为 PNG
        return image.save(saveFileName + ".png", "PNG");
    }
}

static bool AutoStitchImages(const std::vector<cv::Mat> &images, cv::Mat &result)
{
    if (images.empty()) {
           return false;
       }

       // 创建拼接器 - 使用默认特征检测器
       cv::Ptr<cv::Stitcher> stitcher = cv::Stitcher::create(cv::Stitcher::PANORAMA);

       // 尝试拼接图像
       cv::Stitcher::Status status = stitcher->stitch(images, result);

       return status == cv::Stitcher::OK;
}
