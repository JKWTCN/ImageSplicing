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
        "JPEG 图片 (*.jpg *.jpeg);;PNG 图片 (*.png)");
    if (saveFileName.isEmpty())
        return false;

    // 根据文件后缀选择保存格式
    if (saveFileName.endsWith(".png", Qt::CaseInsensitive))
    {
        return image.save(saveFileName, "PNG");
    }
    else if (saveFileName.endsWith(".jpg", Qt::CaseInsensitive) || saveFileName.endsWith(".jpeg", Qt::CaseInsensitive))
    {
        int jpgQuality = settings.value("JpgQuality", 100).toInt(); // 默认 JPEG 质量
        return image.save(saveFileName, "JPEG", jpgQuality);
    }
    else
    {
        // 默认保存为 PNG
        return image.save(saveFileName + ".png", "PNG");
    }
}

// 辅助函数声明
static bool trySimpleFeatureMatching(const std::vector<cv::Mat> &images, cv::Mat &result);
static bool performSimpleConcatenation(const std::vector<cv::Mat> &images, cv::Mat &result);

static bool AutoStitchImages(const std::vector<cv::Mat> &images, cv::Mat &result)
{
    if (images.empty())
    {
        return false;
    }

    if (images.size() == 1)
    {
        result = images[0].clone();
        return true;
    }

    try
    {
        // 创建拼接器 - 使用不同的模式尝试
        std::vector<cv::Stitcher::Mode> modes = {
            cv::Stitcher::PANORAMA,
            cv::Stitcher::SCANS};

        for (auto mode : modes)
        {
            cv::Ptr<cv::Stitcher> stitcher = cv::Stitcher::create(mode);

            // 设置拼接器参数以提高成功率
            stitcher->setRegistrationResol(0.6);
            stitcher->setSeamEstimationResol(0.1);
            stitcher->setCompositingResol(-1);
            stitcher->setPanoConfidenceThresh(1.0);

            // 尝试拼接图像
            cv::Stitcher::Status status = stitcher->stitch(images, result);

            if (status == cv::Stitcher::OK)
            {
                return true;
            }

            // 如果第一种模式失败，记录错误信息但继续尝试
            if (mode == cv::Stitcher::PANORAMA)
            {
                std::cout << "PANORAMA mode failed, trying SCANS mode..." << std::endl;
            }
        }

        // 如果所有模式都失败，尝试简单的特征匹配方法
        return trySimpleFeatureMatching(images, result);
    }
    catch (const cv::Exception &e)
    {
        std::cerr << "AutoStitchImages exception: " << e.what() << std::endl;
        return false;
    }
}

// 简单特征匹配拼接方法（备用方案）
static bool trySimpleFeatureMatching(const std::vector<cv::Mat> &images, cv::Mat &result)
{
    if (images.size() < 2)
    {
        if (!images.empty())
        {
            result = images[0].clone();
            return true;
        }
        return false;
    }

    try
    {
        // 对于只有两张图片的情况，尝试简单的特征匹配
        if (images.size() == 2)
        {
            cv::Mat img1 = images[0];
            cv::Mat img2 = images[1];

            // 检测特征点
            cv::Ptr<cv::SIFT> detector = cv::SIFT::create();
            std::vector<cv::KeyPoint> keypoints1, keypoints2;
            cv::Mat descriptors1, descriptors2;

            detector->detectAndCompute(img1, cv::noArray(), keypoints1, descriptors1);
            detector->detectAndCompute(img2, cv::noArray(), keypoints2, descriptors2);

            if (keypoints1.size() < 10 || keypoints2.size() < 10)
            {
                // 特征点太少，直接进行简单拼接
                return performSimpleConcatenation(images, result);
            }

            // 匹配特征点
            cv::BFMatcher matcher;
            std::vector<cv::DMatch> matches;
            matcher.match(descriptors1, descriptors2, matches);

            if (matches.size() < 10)
            {
                // 匹配点太少，使用简单拼接
                return performSimpleConcatenation(images, result);
            }

            // 如果有足够的匹配点，尝试计算单应性矩阵
            std::vector<cv::Point2f> src_pts, dst_pts;
            for (const auto &match : matches)
            {
                src_pts.push_back(keypoints1[match.queryIdx].pt);
                dst_pts.push_back(keypoints2[match.trainIdx].pt);
            }

            cv::Mat H = cv::findHomography(src_pts, dst_pts, cv::RANSAC);
            if (H.empty())
            {
                return performSimpleConcatenation(images, result);
            }

            // 执行简单的透视变换拼接
            cv::Mat warped;
            cv::warpPerspective(img1, warped, H, cv::Size(img1.cols + img2.cols, std::max(img1.rows, img2.rows)));

            // 将第二张图片复制到结果中
            cv::Mat roi(warped, cv::Rect(0, 0, img2.cols, img2.rows));
            img2.copyTo(roi);

            result = warped;
            return true;
        }

        // 多张图片的情况，使用简单拼接
        return performSimpleConcatenation(images, result);
    }
    catch (const cv::Exception &e)
    {
        std::cerr << "Simple feature matching failed: " << e.what() << std::endl;
        return performSimpleConcatenation(images, result);
    }
}

// 简单拼接方法（最后的备用方案）
static bool performSimpleConcatenation(const std::vector<cv::Mat> &images, cv::Mat &result)
{
    if (images.empty())
    {
        return false;
    }

    try
    {
        // 判断应该进行水平还是垂直拼接
        // 简单启发式：如果图片宽度大于高度，进行垂直拼接，否则水平拼接
        bool doVertical = true;
        for (const auto &img : images)
        {
            if (img.cols > img.rows)
            {
                doVertical = true;
                break;
            }
            else
            {
                doVertical = false;
            }
        }

        if (doVertical)
        {
            cv::vconcat(images, result);
        }
        else
        {
            cv::hconcat(images, result);
        }

        return true;
    }
    catch (const cv::Exception &e)
    {
        std::cerr << "Simple concatenation failed: " << e.what() << std::endl;
        return false;
    }
}
