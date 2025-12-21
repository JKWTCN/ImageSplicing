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
        // 预处理图像 - 统一大小和格式
        std::vector<cv::Mat> processedImages;
        for (const auto &img : images)
        {
            cv::Mat processed;

            // 确保图像是3通道BGR格式
            if (img.channels() == 1)
            {
                cv::cvtColor(img, processed, cv::COLOR_GRAY2BGR);
            }
            else if (img.channels() == 4)
            {
                cv::cvtColor(img, processed, cv::COLOR_BGRA2BGR);
            }
            else
            {
                processed = img.clone();
            }

            // 如果图像太大，进行适当缩放以提高处理速度
            if (processed.cols > 3000 || processed.rows > 3000)
            {
                double scale = std::min(3000.0 / processed.cols, 3000.0 / processed.rows);
                cv::resize(processed, processed, cv::Size(), scale, scale, cv::INTER_AREA);
            }

            processedImages.push_back(processed);
        }

        // 创建拼接器 - 使用不同的模式尝试
        std::vector<cv::Stitcher::Mode> modes = {
            cv::Stitcher::SCANS,   // 优先尝试SCANS模式，适合文档拼接
            cv::Stitcher::PANORAMA}; // 然后尝试PANORAMA模式

        for (auto mode : modes)
        {
            cv::Ptr<cv::Stitcher> stitcher = cv::Stitcher::create(mode);

            // 根据模式调整参数
            if (mode == cv::Stitcher::SCANS)
            {
                stitcher->setRegistrationResol(0.8);  // 提高配准分辨率
                stitcher->setSeamEstimationResol(0.2); // 提高接缝估计分辨率
                stitcher->setCompositingResol(-1);     // 使用原始分辨率
                stitcher->setPanoConfidenceThresh(0.8); // 降低置信度阈值
            }
            else
            {
                stitcher->setRegistrationResol(0.6);
                stitcher->setSeamEstimationResol(0.1);
                stitcher->setCompositingResol(-1);
                stitcher->setPanoConfidenceThresh(1.0);
            }

            // 尝试拼接图像
            cv::Stitcher::Status status = stitcher->stitch(processedImages, result);

            if (status == cv::Stitcher::OK)
            {
                // 如果结果太小，可能是拼接失败了
                if (result.cols < 100 || result.rows < 100)
                {
                    std::cout << mode << " mode produced too small result, trying next method..." << std::endl;
                    continue;
                }

                std::cout << "Successfully stitched using " << (mode == cv::Stitcher::SCANS ? "SCANS" : "PANORAMA") << " mode" << std::endl;
                return true;
            }
            else
            {
                std::cout << mode << " mode failed with status: " << status << std::endl;
            }
        }

        // 如果所有模式都失败，尝试简单的特征匹配方法
        std::cout << "All Stitcher modes failed, trying simple feature matching..." << std::endl;
        return trySimpleFeatureMatching(processedImages, result);
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
        // 对于只有两张图片的情况，尝试改进的特征匹配
        if (images.size() == 2)
        {
            cv::Mat img1 = images[0];
            cv::Mat img2 = images[1];

            // 使用多种特征检测器
            std::vector<std::pair<cv::Ptr<cv::Feature2D>, std::string>> detectors = {
                {cv::SIFT::create(), "SIFT"},
                {cv::ORB::create(1000), "ORB"}
            };

            for (auto &detector_pair : detectors)
            {
                auto detector = detector_pair.first;
                std::string detector_name = detector_pair.second;

                std::vector<cv::KeyPoint> keypoints1, keypoints2;
                cv::Mat descriptors1, descriptors2;

                // 检测特征点和计算描述符
                detector->detectAndCompute(img1, cv::noArray(), keypoints1, descriptors1);
                detector->detectAndCompute(img2, cv::noArray(), keypoints2, descriptors2);

                std::cout << detector_name << " detected " << keypoints1.size() << " and "
                         << keypoints2.size() << " keypoints" << std::endl;

                if (keypoints1.size() < 15 || keypoints2.size() < 15)
                {
                    continue; // 尝试下一个检测器
                }

                // 匹配特征点
                cv::Ptr<cv::DescriptorMatcher> matcher;
                std::vector<cv::DMatch> matches;

                if (detector_name == "SIFT")
                {
                    matcher = cv::BFMatcher::create(cv::NORM_L2);
                }
                else
                {
                    matcher = cv::BFMatcher::create(cv::NORM_HAMMING);
                }

                matcher->match(descriptors1, descriptors2, matches);

                if (matches.size() < 15)
                {
                    continue; // 尝试下一个检测器
                }

                // 筛选好的匹配点（使用距离阈值）
                double minDist = matches[0].distance;
                for (const auto &match : matches)
                {
                    if (match.distance < minDist)
                        minDist = match.distance;
                }

                std::vector<cv::DMatch> goodMatches;
                double threshold = std::max(3.0 * minDist, 0.02);

                for (const auto &match : matches)
                {
                    if (match.distance < threshold)
                    {
                        goodMatches.push_back(match);
                    }
                }

                std::cout << detector_name << " found " << goodMatches.size() << " good matches" << std::endl;

                if (goodMatches.size() < 10)
                {
                    continue; // 尝试下一个检测器
                }

                // 获取匹配点坐标
                std::vector<cv::Point2f> src_pts, dst_pts;
                for (const auto &match : goodMatches)
                {
                    src_pts.push_back(keypoints1[match.queryIdx].pt);
                    dst_pts.push_back(keypoints2[match.trainIdx].pt);
                }

                // 计算单应性矩阵，使用更严格的RANSAC参数
                cv::Mat H = cv::findHomography(src_pts, dst_pts, cv::RANSAC, 3.0);

                if (H.empty())
                {
                    continue; // 尝试下一个检测器
                }

                // 检查单应性矩阵的合理性
                double det = cv::determinant(H(cv::Rect(0, 0, 2, 2)));
                if (std::abs(det) < 0.1 || std::abs(det) > 10.0)
                {
                    continue; // 单应性矩阵不合理，尝试下一个检测器
                }

                std::cout << "Successfully computed homography using " << detector_name << std::endl;

                // 执行透视变换拼接
                cv::Mat warped;

                // 计算变换后的图像尺寸
                std::vector<cv::Point2f> corners1 = {
                    cv::Point2f(0, 0),
                    cv::Point2f(img1.cols, 0),
                    cv::Point2f(img1.cols, img1.rows),
                    cv::Point2f(0, img1.rows)
                };

                std::vector<cv::Point2f> corners1_transformed;
                cv::perspectiveTransform(corners1, corners1_transformed, H);

                // 计算拼接后的画布大小
                double maxX = img2.cols;
                double maxY = img2.rows;
                double minX = 0;
                double minY = 0;

                for (const auto &corner : corners1_transformed)
                {
                    maxX = std::max(maxX, (double)corner.x);
                    maxY = std::max(maxY, (double)corner.y);
                    minX = std::min(minX, (double)corner.x);
                    minY = std::min(minY, (double)corner.y);
                }

                int warpWidth = static_cast<int>(maxX - minX);
                int warpHeight = static_cast<int>(maxY - minY);

                // 调整变换矩阵以考虑偏移
                cv::Mat H_adjusted = H.clone();
                H_adjusted.at<double>(0, 2) -= minX;
                H_adjusted.at<double>(1, 2) -= minY;

                cv::warpPerspective(img1, warped, H_adjusted, cv::Size(warpWidth, warpHeight));

                // 将第二张图片放到正确的位置
                int offsetX = static_cast<int>(-minX);
                int offsetY = static_cast<int>(-minY);

                if (offsetX >= 0 && offsetY >= 0 &&
                    offsetX + img2.cols <= warped.cols &&
                    offsetY + img2.rows <= warped.rows)
                {
                    cv::Mat roi(warped, cv::Rect(offsetX, offsetY, img2.cols, img2.rows));
                    img2.copyTo(roi);
                }
                else
                {
                    // 如果位置超出边界，需要扩展画布
                    int finalWidth = std::max(warped.cols, offsetX + img2.cols);
                    int finalHeight = std::max(warped.rows, offsetY + img2.rows);

                    cv::Mat finalResult = cv::Mat::zeros(finalHeight, finalWidth, warped.type());

                    // 复制变换后的图像
                    cv::Mat roi1(finalResult, cv::Rect(0, 0, warped.cols, warped.rows));
                    warped.copyTo(roi1);

                    // 复制第二张图像
                    if (offsetX >= 0 && offsetY >= 0)
                    {
                        cv::Mat roi2(finalResult, cv::Rect(offsetX, offsetY, img2.cols, img2.rows));
                        img2.copyTo(roi2);
                    }

                    warped = finalResult;
                }

                result = warped;
                return true;
            }

            // 所有特征检测器都失败，使用简单拼接
            std::cout << "All feature detectors failed, using simple concatenation" << std::endl;
            return performSimpleConcatenation(images, result);
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
