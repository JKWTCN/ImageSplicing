#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <iostream>
#include <QFileDialog>
#include <QStringList>
#include <QGraphicsSceneWheelEvent>
#include <settingwindow.h>
#include <ctime>
#include "SplicingLine.h"
#include "MovablePixmapItem.h"
#include "config.hpp"
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QUrl>
#include <QFileInfo>
#include "aboutdialog.h"
using namespace std;

QStringList filePaths; // 当前打开图片存储路径
SplicingState splicingState = SS_NONE;
vector<cv::Mat> images;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->pushButton_editSelect->setVisible(false);
    ui->pushButton_editResult->setVisible(false);
    if (GetSplicingTypeConfig() != ST_RAW)
        SetPaddingColorTypeConfig(PT_TRANSPARENT);

    // 启用拖放功能
    setAcceptDrops(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}
// 手动刷新界面
void MainWindow::UpdateQListWidget()
{
    ui->listImageFilesWidget->clear();
    ui->listImageFilesWidget->setViewMode(QListWidget::IconMode);
    // ui->listImageFilesWidget->setIconSize(QSize(100, 100));//设置图片大小
    ui->listImageFilesWidget->setSpacing(10);                   // 间距
    ui->listImageFilesWidget->setResizeMode(QListView::Adjust); // 适应布局调整
    ui->listImageFilesWidget->setMovement(QListView::Static);   // 不能移动

    for (int i = 0; i < filePaths.length(); i++)
    {
        QListWidgetItem *imageItem = new QListWidgetItem;
        imageItem->setIcon(QIcon(filePaths[i]));
        imageItem->setText(QString::fromStdString(GetFileName(filePaths[i].toStdString())));
        //        imageItem->setSizeHint(QSize(120, 100));
        ui->listImageFilesWidget->addItem(imageItem);
    }
}
// 刷新拼接图片
void MainWindow::ReFreshResultWidget()
{
    switch (splicingState)
    {
    case SS_VERTICAL:
        MainWindow::on_pushButton_verticalSplicing_clicked();
        break;
    case SS_HORIZONTAL:
        MainWindow::on_pushButton_horizontalSplicing_clicked();
        break;
    case SS_AUTO_HORIZONTAL:
        MainWindow::on_pushButton_auto_clicked();
        break;
    case SS_AUTO_VERTICAL:
        MainWindow::on_pushButton_auto_clicked();
        break;
    case SS_NONE:
        break;
    }
}

// 打开文件按钮事件
void MainWindow::openFilesBtnPress()
{

    splicingState = SS_NONE;
    filePaths.clear();
    images.clear();
    UpdateQListWidget();

    QGraphicsScene *scene = new QGraphicsScene;
    ui->graphicsView_result->setScene(scene);
    scene->installEventFilter(this);
    ui->pushButton_auto->setEnabled(false);

    filePaths = OpenImagePaths();
    if (filePaths.length() == 0)
    {
        // showInfoMessageBox("提示", "拼接图片列表为空,请打开图片。");
        return;
    }
    if (GetOpenReverseConfig())
    {
        QStringList tmp = filePaths;
        for (int i = 0; i < tmp.length(); i++)
        {
            filePaths[i] = tmp[tmp.length() - i - 1];
        }
    }

    UnlockPostionButton();
    UpdateQListWidget();
}
// 图片排序 上
void MainWindow::upImagePosition()
{
    ClearResult();
    if (ui->listImageFilesWidget->selectedItems().isEmpty())
        return;
    int nowIndex = ui->listImageFilesWidget->currentRow();
    if (nowIndex != 0)
        filePaths.move(nowIndex, nowIndex - 1);
    else
        filePaths.move(nowIndex, filePaths.length() - 1);
    UpdateQListWidget();
    ReFreshResultWidget();
    if (nowIndex != 0)
        ui->listImageFilesWidget->setCurrentRow(nowIndex - 1);
    else
        ui->listImageFilesWidget->setCurrentRow(filePaths.length() - 1);
    ui->listImageFilesWidget->setFocus();
}
// 图片排序 下
void MainWindow::downImagePosition()
{
    ClearResult();
    if (ui->listImageFilesWidget->selectedItems().isEmpty())
        return;
    int nowIndex = ui->listImageFilesWidget->currentRow();
    if (nowIndex <= filePaths.length() - 2)
        filePaths.move(nowIndex, nowIndex + 1);
    else
        filePaths.move(nowIndex, 0);
    UpdateQListWidget();
    ReFreshResultWidget();
    if (nowIndex <= filePaths.length() - 2)
        ui->listImageFilesWidget->setCurrentRow(nowIndex + 1);
    else
        ui->listImageFilesWidget->setCurrentRow(0);
    ui->listImageFilesWidget->setFocus();
}
// 删除图片按钮事件
void MainWindow::deleteImagePosition()
{
    ClearResult();
    if (ui->listImageFilesWidget->selectedItems().isEmpty())
        return;
    int nowIndex = ui->listImageFilesWidget->currentRow();
    filePaths.removeAt(nowIndex);
    UpdateQListWidget();
    ReFreshResultWidget();
}

// 打开设置界面
void MainWindow::on_setting_action_triggered()
{
    SettingWindow *settingWindow = new SettingWindow();
    settingWindow->setWindowModality(Qt::ApplicationModal); // 设置为应用程序模态
    settingWindow->setAttribute(Qt::WA_DeleteOnClose);      // 关闭时自动删除
    settingWindow->show();
    settingWindow->activateWindow(); // 激活窗口并获取焦点
    settingWindow->raise();
}
// 打开关于界面
void MainWindow::on_action_about_triggered()
{
    AboutDialog *aboutDialog = new AboutDialog();
    aboutDialog->setWindowModality(Qt::ApplicationModal); // 设置为应用程序模态
    aboutDialog->setAttribute(Qt::WA_DeleteOnClose);      // 关闭时自动删除
    aboutDialog->show();
    aboutDialog->activateWindow(); // 激活窗口并获取焦点
    aboutDialog->raise();
}

// 解锁图片调整按钮
void MainWindow::UnlockPostionButton()
{
    ui->pushButton_5->setEnabled(true);
    ui->pushButton_6->setEnabled(true);
    ui->pushButton_7->setEnabled(true);
    ui->pushButton_LayerUp->setEnabled(true);
    ui->pushButton_LayerDown->setEnabled(true);
    ui->pushButton_copy->setEnabled(true);
    ui->pushButton_add->setEnabled(true);
    ui->pushButton_editResult->setEnabled(true);
    ui->pushButton_editSelect->setEnabled(true);
}

// 锁定图片调整按钮
void MainWindow::LockPostionButton()
{
    ui->pushButton_5->setEnabled(false);
    ui->pushButton_6->setEnabled(false);
    ui->pushButton_7->setEnabled(false);
    ui->pushButton_LayerUp->setEnabled(false);
    ui->pushButton_LayerDown->setEnabled(false);
    ui->pushButton_copy->setEnabled(false);
    ui->pushButton_add->setEnabled(false);
    ui->pushButton_editResult->setEnabled(false);
    ui->pushButton_editSelect->setEnabled(false);
}
// 横向拼接
void MainWindow::on_pushButton_horizontalSplicing_clicked()
{
    images.clear();
    if (filePaths.length() == 0)
    {

        filePaths = OpenImagePaths();
        if (GetOpenReverseConfig())
        {
            QStringList tmp = filePaths;
            for (int i = 0; i < tmp.length(); i++)
            {
                filePaths[i] = tmp[tmp.length() - i - 1];
            }
        }
        if (filePaths.length() == 0)
        {
            //            showInfoMessageBox("提示", "拼接图片列表为空,请打开图片。");
            return;
        }
    }
    UnlockPostionButton();
    splicingState = SS_HORIZONTAL;
    UpdateQListWidget();
    // 加载所有图片
    for (const auto &path : filePaths)
    {
        cv::Mat img = cv::imread(path.toLocal8Bit().constData(), cv::IMREAD_UNCHANGED);
        if (img.empty())
        {
            cout << "Failed to load image:" << path.toLocal8Bit().constData();
            showErrorMessageBox("错误", "不能读取文件" + path.toStdString());
        }
        else
        {
            images.push_back(img);
        }
    }
    int targetHeight = images[0].rows;
    for (const auto &img : images)
    {
        switch (GetSplicingTypeConfig())
        {
        case ST_HIGH2LOW:
            targetHeight = std::min(targetHeight, img.rows);
            break;
        case ST_LOW2HIGH:
            targetHeight = std::max(targetHeight, img.rows);
            break;
        case ST_RAW:
            targetHeight = std::max(targetHeight, img.rows);
            break;
        }
    }
    for (auto &img : images)
    {
        if (img.rows != targetHeight)
        {
            switch (GetSplicingTypeConfig())
            {
            case ST_HIGH2LOW:
                img = ResizeByHeight(img, targetHeight, GetShrinkTypeConfig());
                break;
            case ST_LOW2HIGH:
                img = ResizeByHeight(img, targetHeight, GetNarrowTypeConfig());
                break;
            case ST_RAW:
                break;
            }
        }
    }
    QGraphicsScene *scene = new QGraphicsScene(this);

    // 初始X坐标
    int xPos = 0;
    const int verticalSpacing = 0;   // 垂直居中位置
    const int horizontalSpacing = 0; // 图片间水平间距

    // 存储图片项目，用于后续添加拼接线
    QList<MovablePixmapItem *> imageItems;

    // 将所有图像添加到场景中
    for (const auto &image : images)
    {
        // 将 OpenCV 的 BGR 格式转换为 RGB 格式
        cv::Mat rgbImage;
        cv::cvtColor(image, rgbImage, cv::COLOR_BGR2RGB);

        // 创建 QImage 并从 cv::Mat 复制数据
        QImage qimg(rgbImage.data, rgbImage.cols, rgbImage.rows, rgbImage.step, QImage::Format_RGB888);

        // 将 QImage 转换为 QPixmap
        QPixmap pixmap = QPixmap::fromImage(qimg);

        // 创建自定义的可拖动项
        MovablePixmapItem *item = new MovablePixmapItem(pixmap, MV_H, xPos, verticalSpacing, 0);
        scene->addItem(item);

        // 添加到列表中
        imageItems.append(item);

        // 更新下一个图片的X位置
        xPos += pixmap.width() + horizontalSpacing;
    }

    // 在相邻图片之间添加拼接线
    for (int i = 0; i < imageItems.size() - 1; i++)
    {
        MovablePixmapItem *currentItem = imageItems[i];
        MovablePixmapItem *nextItem = imageItems[i + 1];

        // 计算拼接线位置（两个图片之间的边界）
        qreal lineX = currentItem->pos().x() + currentItem->pixmap().width();
        qreal lineY1 = qMin(currentItem->pos().y(), nextItem->pos().y()) - 30; // 向上扩展30像素
        qreal lineY2 = qMax(currentItem->pos().y() + currentItem->pixmap().height(),
                            nextItem->pos().y() + nextItem->pixmap().height()) +
                       30; // 向下扩展30像素

        // 创建垂直拼接线
        SplicingLine *splicingLine = new SplicingLine(lineX, lineY1, lineX, lineY2,
                                                      SplicingLineOrientation::Vertical);
        splicingLine->setExtensionLength(30);
        splicingLine->setLineWidth(2.0, 8.0);
        splicingLine->setZValue(1); // 确保拼接线在图片之上
        scene->addItem(splicingLine);

        // 设置图片项和拼接线的关联关系
        currentItem->setRightSplicingLine(splicingLine); // 当前图片的右拼接线
        nextItem->setLeftSplicingLine(splicingLine);     // 下一个图片的左拼接线
        splicingLine->setLastItem(currentItem);
        splicingLine->setNextItem(nextItem);
    }
    ui->pushButton_auto->setEnabled(true);
    ui->pushButton_save->setEnabled(true);
    if (!scene || scene->items().isEmpty())
        return;
    // 获取所有可见项目的边界矩形
    QRectF boundingRect = scene->itemsBoundingRect();
    // 调整场景大小
    if (!boundingRect.isNull())
    {
        scene->setSceneRect(boundingRect);
        ui->graphicsView_result->fitInView(boundingRect, Qt::KeepAspectRatio);
    }
    ui->graphicsView_result->setScene(scene); // 设置场景到 graphicsView
    scene->installEventFilter(this);
}

// 纵向拼接
void MainWindow::on_pushButton_verticalSplicing_clicked()
{
    images.clear();

    if (filePaths.length() == 0)
    {

        filePaths = OpenImagePaths();
        if (GetOpenReverseConfig())
        {
            QStringList tmp = filePaths;
            for (int i = 0; i < tmp.length(); i++)
            {
                filePaths[i] = tmp[tmp.length() - i - 1];
            }
        }

        if (filePaths.length() == 0)
        {
            //            showInfoMessageBox("提示", "拼接图片列表为空,请打开图片。");
            return;
        }
    }
    UnlockPostionButton();
    splicingState = SS_VERTICAL;
    UpdateQListWidget();
    // 加载所有图片
    for (const auto &path : filePaths)
    {
        cv::Mat img = cv::imread(path.toLocal8Bit().constData(), cv::IMREAD_UNCHANGED);
        if (img.empty())
        {
            cout << "Failed to load image:" << path.toLocal8Bit().constData();
            showErrorMessageBox("错误", "不能读取文件" + path.toStdString());
        }
        else
        {
            cout << "Loaded image successfully:" << path.toLocal8Bit().constData();
            images.push_back(img);
        }
    }
    int targetWidth = images[0].cols;
    for (const auto &img : images)
    {
        switch (GetSplicingTypeConfig())
        {
        case ST_HIGH2LOW:
            targetWidth = std::min(targetWidth, img.cols);
            break;
        case ST_LOW2HIGH:
            targetWidth = std::max(targetWidth, img.cols);
            break;
        case ST_RAW:
            targetWidth = std::max(targetWidth, img.cols);
            break;
        }
    }
    for (auto &img : images)
    {
        if (img.cols != targetWidth)
        {
            switch (GetSplicingTypeConfig())
            {
            case ST_HIGH2LOW:
                img = ResizeByWidth(img, targetWidth, GetShrinkTypeConfig());
                break;
            case ST_LOW2HIGH:
                img = ResizeByWidth(img, targetWidth, GetShrinkTypeConfig());
                break;
            case ST_RAW:
                break;
            }
        }
    }

    QGraphicsScene *scene = new QGraphicsScene(this);

    // 初始Y坐标
    int yPos = 0;
    const int horizontalSpacing = 0; // 水平居中位置
    const int verticalSpacing = 0;   // 图片间垂直间距

    // 存储图片项目，用于后续添加拼接线
    QList<MovablePixmapItem *> imageItems;

    // 将所有图像添加到场景中
    for (const auto &image : images)
    {
        // 将 OpenCV 的 BGR 格式转换为 RGB 格式
        cv::Mat rgbImage;
        cv::cvtColor(image, rgbImage, cv::COLOR_BGR2RGB);

        // 创建 QImage 并从 cv::Mat 复制数据
        QImage qimg(rgbImage.data, rgbImage.cols, rgbImage.rows, rgbImage.step, QImage::Format_RGB888);

        // 将 QImage 转换为 QPixmap
        QPixmap pixmap = QPixmap::fromImage(qimg);

        // 创建自定义的可拖动项
        MovablePixmapItem *item = new MovablePixmapItem(pixmap, MV_V, horizontalSpacing, yPos, 0);
        scene->addItem(item);

        // 添加到列表中
        imageItems.append(item);

        // 更新下一个图片的Y位置
        yPos += pixmap.height() + verticalSpacing;
    }

    // 在相邻图片之间添加拼接线
    for (int i = 0; i < imageItems.size() - 1; i++)
    {
        MovablePixmapItem *currentItem = imageItems[i];
        MovablePixmapItem *nextItem = imageItems[i + 1];

        // 计算拼接线位置（两个图片之间的边界）
        qreal lineY = currentItem->pos().y() + currentItem->pixmap().height();
        qreal lineX1 = qMin(currentItem->pos().x(), nextItem->pos().x()) - 30; // 向左扩展30像素
        qreal lineX2 = qMax(currentItem->pos().x() + currentItem->pixmap().width(),
                            nextItem->pos().x() + nextItem->pixmap().width()) +
                       30; // 向右扩展30像素

        // 创建水平拼接线
        SplicingLine *splicingLine = new SplicingLine(lineX1, lineY, lineX2, lineY,
                                                      SplicingLineOrientation::Horizontal);
        splicingLine->setExtensionLength(30);
        splicingLine->setLineWidth(2.0, 8.0);
        splicingLine->setZValue(1); // 确保拼接线在图片之上
        scene->addItem(splicingLine);

        // 设置图片项和拼接线的关联关系
        currentItem->setBottomSplicingLine(splicingLine); // 当前图片的下拼接线
        nextItem->setTopSplicingLine(splicingLine);       // 下一个图片的上拼接线
        splicingLine->setLastItem(currentItem);
        splicingLine->setNextItem(nextItem);
    }
    ui->pushButton_auto->setEnabled(true);
    ui->pushButton_save->setEnabled(true);
    if (!scene || scene->items().isEmpty())
        return;
    // 获取所有可见项目的边界矩形
    QRectF boundingRect = scene->itemsBoundingRect();
    // 调整场景大小
    if (!boundingRect.isNull())
    {
        scene->setSceneRect(boundingRect);
        ui->graphicsView_result->fitInView(boundingRect, Qt::KeepAspectRatio);
    }
    ui->graphicsView_result->setScene(scene); // 设置场景到 graphicsView
    scene->installEventFilter(this);
}
// 自动调整图像
void MainWindow::on_pushButton_auto_clicked()
{
    if (images.empty() || splicingState == SS_NONE)
    {
        showNoticeMessageBox("提示", "请先进行图像拼接后再使用自动调整功能。");
        return;
    }

    SplicingState originalState = splicingState;

    switch (splicingState)
    {
    case SS_VERTICAL:
        splicingState = SS_AUTO_VERTICAL;
        break;
    case SS_HORIZONTAL:
        splicingState = SS_AUTO_HORIZONTAL;
        break;
    default:
        break;
    }

    try
    {
        // 使用改进的自动调整算法，但保持拼接线显示
        bool success = false;
        std::vector<cv::Mat> optimizedImages;

        // 首先尝试使用特征匹配进行图像优化
        if (images.size() >= 2)
        {
            success = performAutoAlignment(images, optimizedImages);
        }

        // 如果自动对齐失败，使用原始图像
        if (!success || optimizedImages.empty())
        {
            optimizedImages = images;
            success = true;
        }

        // 根据拼接方向创建优化后的拼接场景
        QGraphicsScene *scene = new QGraphicsScene(this);

        if (splicingState == SS_AUTO_VERTICAL || originalState == SS_VERTICAL)
        {
            createOptimizedVerticalScene(scene, optimizedImages);
        }
        else if (splicingState == SS_AUTO_HORIZONTAL || originalState == SS_HORIZONTAL)
        {
            createOptimizedHorizontalScene(scene, optimizedImages);
        }
        else
        {
            // 默认使用垂直拼接
            createOptimizedVerticalScene(scene, optimizedImages);
        }

        // 设置场景
        ui->graphicsView_result->setScene(scene);
        scene->installEventFilter(this);

        // 获取场景边界并设置
        if (!scene->items().isEmpty())
        {
            QRectF boundingRect = scene->itemsBoundingRect();
            if (!boundingRect.isNull())
            {
                scene->setSceneRect(boundingRect);
                ui->graphicsView_result->fitInView(boundingRect, Qt::KeepAspectRatio);
            }
        }

        ui->pushButton_auto->setEnabled(true);
        ui->pushButton_save->setEnabled(true);

        showNoticeMessageBox("提示", success ? "自动调整完成" : "使用原始图像完成拼接");
    }
    catch (cv::Exception &e)
    {
        std::cerr << "OpenCV异常: " << e.what() << std::endl;
        showErrorMessageBox("错误", "图像处理异常: " + std::string(e.what()));
        splicingState = originalState;
    }
    catch (std::exception &e)
    {
        std::cerr << "标准异常: " << e.what() << std::endl;
        showErrorMessageBox("错误", "处理异常: " + std::string(e.what()));
        splicingState = originalState;
    }
    catch (...)
    {
        std::cerr << "未知异常" << std::endl;
        showErrorMessageBox("错误", "发生未知异常");
        splicingState = originalState;
    }
}
// 事件过滤器
bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::GraphicsSceneWheel)
    {
        ui->graphicsView_result->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
        double scaleFactor = 1.15;
        bool ok = QApplication::keyboardModifiers() & Qt::ControlModifier;
        if (ok)
        {
            QGraphicsSceneWheelEvent *scrollevent = static_cast<QGraphicsSceneWheelEvent *>(event);
            if (scrollevent->delta() > 0)
            {
                ui->graphicsView_result->scale(scaleFactor, scaleFactor);
            }
            else
            {
                ui->graphicsView_result->scale(1 / scaleFactor, 1 / scaleFactor);
            }
        }

        event->accept();
        return true;
    }
    return false;
}
// 重写滚轮事件处理函数
// 该函数用于处理滚轮事件，实现缩放功能
// void MainWindow::wheelEvent(QWheelEvent *event)
// {
//     // 检查是否按下 Ctrl 键
//     if (QApplication::keyboardModifiers() & Qt::ControlModifier)
//     {
//         // 获取滚轮滚动角度（通常 1 步 = 120 度）
//         int delta = event->angleDelta().y();

//         if (delta > 0)
//         {
//             ui->graphicsView_result->scale(1.1, 1.1); // 放大10%
//         }
//         else if (delta < 0)
//         {
//             ui->graphicsView_result->scale(0.9, 0.9); // 缩小10%
//         }

//         event->accept(); // 表示事件已处理
//     }
//     else
//     {
//         // 其他情况（如普通滚轮滚动）交给父类处理
//         QWidget::wheelEvent(event);
//     }
// }
// 保存图像
void MainWindow::on_pushButton_save_clicked()
{
    time_t now = time(0);
    QSettings settings("iCloudWar", "ImageSplicing");
    QString lastDir = settings.value("LastOpenDirectory", "ImageSplicing_" + QDir::homePath()).toString();
    QGraphicsScene *scene = ui->graphicsView_result->scene();
    for (auto &item : scene->items())
    {
        if (item->type() == SplicingLine::Type)
        {
            SplicingLine *splicingLine = static_cast<SplicingLine *>(item);
            splicingLine->setVisible(false); // 隐藏拼接线
        }
    }
    scene->clearSelection();
    if (!scene || scene->items().isEmpty())
        return;
    // 获取所有可见项目的边界矩形
    QRectF boundingRect = scene->itemsBoundingRect();
    // 调整场景大小
    if (!boundingRect.isNull())
    {
        scene->setSceneRect(boundingRect);
        ui->graphicsView_result->fitInView(boundingRect, Qt::KeepAspectRatio);
    }
    bool isSaved = false;
    switch (GetSaveTypeConfig())
    {
    case ST_PNG:
        isSaved = saveGraphicsViewToImage(ui->graphicsView_result, "ImageSplicing_" + QString::number(now) + ".png", PaddingColorType2QtColor(GetPaddingColorTypeConfig()));
        break;
    case ST_JPG:
        isSaved = saveGraphicsViewToImage(ui->graphicsView_result, "ImageSplicing_" + QString::number(now) + ".jpg", PaddingColorType2QtColor(GetPaddingColorTypeConfig()));
        break;
    default:
        isSaved = saveGraphicsViewToImage(ui->graphicsView_result, "ImageSplicing_" + QString::number(now) + ".png", PaddingColorType2QtColor(GetPaddingColorTypeConfig()));
        break;
    }
    if (!isSaved)
    {
        showErrorMessageBox("错误", "未保存图片");
        return;
    }
    if (GetFinishRAWPhictureConfig())
    {
        bool allDeleted = true;
        for (const auto &path : filePaths)
        {
            if (remove(path.toLocal8Bit().toStdString().c_str()) != 0)
            { // 尝试删除文件
                cout << "Failed to delete file." << path.toStdString() << endl;
                allDeleted = false;
            }
        }
        if (allDeleted)
        {
            showNoticeMessageBox("提示", "原图片已成功删除。");
        }
        else
        {
            showNoticeMessageBox("提示", "原图片删除失败。");
        }
    }
    filePaths.clear();
    images.clear();
    ClearResult();
    LockPostionButton();
    ui->pushButton_auto->setEnabled(false);
    ui->pushButton_save->setEnabled(false);
    splicingState = SS_NONE;
    UpdateQListWidget();
}
// 清除所有显示
void MainWindow::ClearDisplay()
{
    ui->listImageFilesWidget->clear();
    ClearResult();
}
// 清除结果显示
void MainWindow::ClearResult()
{
    QGraphicsScene *scene = ui->graphicsView_result->scene();
    if (scene)
    {
        scene->clear();
    }
}

void MainWindow::on_pushButton_LayerUp_clicked()
{
    QGraphicsScene *scene = ui->graphicsView_result->scene();
    if (!scene)
        return;
    QList<QGraphicsItem *> selectedItems = scene->selectedItems();
    foreach (QGraphicsItem *item, selectedItems)
    {
        item->setZValue(item->zValue() + 1);
    }
}

void MainWindow::on_pushButton_LayerDown_clicked()
{
    QGraphicsScene *scene = ui->graphicsView_result->scene();
    if (!scene)
        return;
    QList<QGraphicsItem *> selectedItems = scene->selectedItems();
    foreach (QGraphicsItem *item, selectedItems)
    {
        item->setZValue(item->zValue() - 1);
    }
}
// 复制图片
void MainWindow::on_pushButton_copy_clicked()
{
    ClearResult();
    if (ui->listImageFilesWidget->selectedItems().isEmpty())
        return;
    int nowIndex = ui->listImageFilesWidget->currentRow();
    filePaths.append(filePaths[nowIndex]);
    filePaths.move(filePaths.length() - 1, nowIndex + 1);
    UpdateQListWidget();
    ReFreshResultWidget();
}
// 添加图片
void MainWindow::on_pushButton_add_clicked()
{
    auto tmp_filePaths = OpenImagePaths();
    if (tmp_filePaths.length() == 0)
    {
        //            showInfoMessageBox("提示", "拼接图片列表为空,请打开图片。");
        return;
    }
    if (GetOpenReverseConfig())
    {
        QStringList tmp = tmp_filePaths;
        for (int i = 0; i < tmp.length(); i++)
        {
            tmp_filePaths[i] = tmp[tmp.length() - i - 1];
        }
    }
    filePaths.append(tmp_filePaths);
    UpdateQListWidget();
    ReFreshResultWidget();
}

void MainWindow::on_pushButton_editSelect_clicked()
{
    //    int nowIndex = ui->listImageFilesWidget->currentRow();
}

void MainWindow::on_pushButton_editResult_clicked()
{
    //    int nowIndex = ui->listImageFilesWidget->currentRow();
}

// 拖拽进入事件
void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    // 检查拖入的数据是否包含文件URL
    if (event->mimeData()->hasUrls())
    {
        // 检查拖入的文件是否为图片文件
        bool hasImageFile = false;
        QStringList supportedFormats = {"jpg", "jpeg", "png", "bmp", "gif", "tif", "tiff", "ico"};

        foreach (const QUrl &url, event->mimeData()->urls())
        {
            if (url.isLocalFile())
            {
                QString filePath = url.toLocalFile();
                QString suffix = QFileInfo(filePath).suffix().toLower();
                if (supportedFormats.contains(suffix))
                {
                    hasImageFile = true;
                    break;
                }
            }
        }

        if (hasImageFile)
        {
            event->acceptProposedAction();
        }
        else
        {
            event->ignore();
        }
    }
    else
    {
        event->ignore();
    }
}

// 拖拽放下事件
void MainWindow::dropEvent(QDropEvent *event)
{
    // 获取拖入的文件URL列表
    QStringList droppedFiles;
    QStringList supportedFormats = {"jpg", "jpeg", "png", "bmp", "gif", "tif", "tiff", "ico"};

    foreach (const QUrl &url, event->mimeData()->urls())
    {
        if (url.isLocalFile())
        {
            QString filePath = url.toLocalFile();
            QString suffix = QFileInfo(filePath).suffix().toLower();
            if (supportedFormats.contains(suffix))
            {
                droppedFiles.append(filePath);
            }
        }
    }

    if (!droppedFiles.isEmpty())
    {
        // 如果当前没有图片文件，则重置状态
        if (filePaths.isEmpty())
        {
            splicingState = SS_NONE;
            images.clear();

            QGraphicsScene *scene = new QGraphicsScene;
            ui->graphicsView_result->setScene(scene);
            scene->installEventFilter(this);
            ui->pushButton_auto->setEnabled(false);
        }

        // 应用反向配置
        if (GetOpenReverseConfig())
        {
            QStringList tmp = droppedFiles;
            droppedFiles.clear();
            for (int i = tmp.length() - 1; i >= 0; i--)
            {
                droppedFiles.append(tmp[i]);
            }
        }

        // 添加拖入的文件到文件列表
        filePaths.append(droppedFiles);

        // 解锁位置调整按钮
        UnlockPostionButton();

        // 更新界面
        UpdateQListWidget();

        // 如果有当前的拼接状态，刷新结果
        if (splicingState != SS_NONE)
        {
            ReFreshResultWidget();
        }

        event->acceptProposedAction();
    }
    else
    {
        event->ignore();
    }
}

// 增强垂直拼接函数
cv::Mat MainWindow::performEnhancedVerticalStitching(const std::vector<cv::Mat> &images)
{
    if (images.empty())
    {
        return cv::Mat();
    }

    if (images.size() == 1)
    {
        return images[0].clone();
    }

    try
    {
        // 统一图像宽度
        std::vector<cv::Mat> resizedImages;
        int targetWidth = images[0].cols;

        // 根据配置确定目标宽度
        for (const auto &img : images)
        {
            switch (GetSplicingTypeConfig())
            {
            case ST_HIGH2LOW:
                targetWidth = std::min(targetWidth, img.cols);
                break;
            case ST_LOW2HIGH:
                targetWidth = std::max(targetWidth, img.cols);
                break;
            case ST_RAW:
                targetWidth = std::max(targetWidth, img.cols);
                break;
            }
        }

        // 调整所有图像到目标宽度
        for (const auto &img : images)
        {
            cv::Mat resizedImg;
            if (img.cols != targetWidth && GetSplicingTypeConfig() != ST_RAW)
            {
                switch (GetSplicingTypeConfig())
                {
                case ST_HIGH2LOW:
                    resizedImg = ResizeByWidth(img, targetWidth, GetShrinkTypeConfig());
                    break;
                case ST_LOW2HIGH:
                    resizedImg = ResizeByWidth(img, targetWidth, GetNarrowTypeConfig());
                    break;
                default:
                    resizedImg = img.clone();
                    break;
                }
            }
            else
            {
                resizedImg = img.clone();
            }
            resizedImages.push_back(resizedImg);
        }

        // 执行垂直拼接
        cv::Mat result;
        cv::vconcat(resizedImages, result);

        return result;
    }
    catch (const cv::Exception &e)
    {
        std::cerr << "增强垂直拼接异常: " << e.what() << std::endl;
        return cv::Mat();
    }
}

// 增强水平拼接函数
cv::Mat MainWindow::performEnhancedHorizontalStitching(const std::vector<cv::Mat> &images)
{
    if (images.empty())
    {
        return cv::Mat();
    }

    if (images.size() == 1)
    {
        return images[0].clone();
    }

    try
    {
        // 统一图像高度
        std::vector<cv::Mat> resizedImages;
        int targetHeight = images[0].rows;

        // 根据配置确定目标高度
        for (const auto &img : images)
        {
            switch (GetSplicingTypeConfig())
            {
            case ST_HIGH2LOW:
                targetHeight = std::min(targetHeight, img.rows);
                break;
            case ST_LOW2HIGH:
                targetHeight = std::max(targetHeight, img.rows);
                break;
            case ST_RAW:
                targetHeight = std::max(targetHeight, img.rows);
                break;
            }
        }

        // 调整所有图像到目标高度
        for (const auto &img : images)
        {
            cv::Mat resizedImg;
            if (img.rows != targetHeight && GetSplicingTypeConfig() != ST_RAW)
            {
                switch (GetSplicingTypeConfig())
                {
                case ST_HIGH2LOW:
                    resizedImg = ResizeByHeight(img, targetHeight, GetShrinkTypeConfig());
                    break;
                case ST_LOW2HIGH:
                    resizedImg = ResizeByHeight(img, targetHeight, GetNarrowTypeConfig());
                    break;
                default:
                    resizedImg = img.clone();
                    break;
                }
            }
            else
            {
                resizedImg = img.clone();
            }
            resizedImages.push_back(resizedImg);
        }

        // 执行水平拼接
        cv::Mat result;
        cv::hconcat(resizedImages, result);

        return result;
    }
    catch (const cv::Exception &e)
    {
        std::cerr << "增强水平拼接异常: " << e.what() << std::endl;
        return cv::Mat();
    }
}

// 自动对齐函数
bool MainWindow::performAutoAlignment(const std::vector<cv::Mat> &images, std::vector<cv::Mat> &optimizedImages)
{
    optimizedImages.clear();

    if (images.empty())
    {
        return false;
    }

    if (images.size() == 1)
    {
        optimizedImages.push_back(images[0].clone());
        return true;
    }

    try
    {
        // 对于简单的对齐，我们主要进行尺寸标准化和位置微调
        std::vector<cv::Mat> alignedImages;

        // 统一图像尺寸（根据拼接方向）
        if (splicingState == SS_AUTO_VERTICAL || splicingState == SS_VERTICAL)
        {
            // 垂直拼接 - 统一宽度
            int targetWidth = images[0].cols;
            for (const auto &img : images)
            {
                switch (GetSplicingTypeConfig())
                {
                case ST_HIGH2LOW:
                    targetWidth = std::min(targetWidth, img.cols);
                    break;
                case ST_LOW2HIGH:
                    targetWidth = std::max(targetWidth, img.cols);
                    break;
                case ST_RAW:
                    targetWidth = std::max(targetWidth, img.cols);
                    break;
                }
            }

            for (const auto &img : images)
            {
                cv::Mat aligned;
                if (img.cols != targetWidth && GetSplicingTypeConfig() != ST_RAW)
                {
                    switch (GetSplicingTypeConfig())
                    {
                    case ST_HIGH2LOW:
                        aligned = ResizeByWidth(img, targetWidth, GetShrinkTypeConfig());
                        break;
                    case ST_LOW2HIGH:
                        aligned = ResizeByWidth(img, targetWidth, GetNarrowTypeConfig());
                        break;
                    default:
                        aligned = img.clone();
                        break;
                    }
                }
                else
                {
                    aligned = img.clone();
                }
                alignedImages.push_back(aligned);
            }
        }
        else
        {
            // 水平拼接 - 统一高度
            int targetHeight = images[0].rows;
            for (const auto &img : images)
            {
                switch (GetSplicingTypeConfig())
                {
                case ST_HIGH2LOW:
                    targetHeight = std::min(targetHeight, img.rows);
                    break;
                case ST_LOW2HIGH:
                    targetHeight = std::max(targetHeight, img.rows);
                    break;
                case ST_RAW:
                    targetHeight = std::max(targetHeight, img.rows);
                    break;
                }
            }

            for (const auto &img : images)
            {
                cv::Mat aligned;
                if (img.rows != targetHeight && GetSplicingTypeConfig() != ST_RAW)
                {
                    switch (GetSplicingTypeConfig())
                    {
                    case ST_HIGH2LOW:
                        aligned = ResizeByHeight(img, targetHeight, GetShrinkTypeConfig());
                        break;
                    case ST_LOW2HIGH:
                        aligned = ResizeByHeight(img, targetHeight, GetNarrowTypeConfig());
                        break;
                    default:
                        aligned = img.clone();
                        break;
                    }
                }
                else
                {
                    aligned = img.clone();
                }
                alignedImages.push_back(aligned);
            }
        }

        optimizedImages = alignedImages;
        return true;
    }
    catch (const cv::Exception &e)
    {
        std::cerr << "自动对齐异常: " << e.what() << std::endl;
        return false;
    }
}

// 创建优化的垂直场景
void MainWindow::createOptimizedVerticalScene(QGraphicsScene *scene, const std::vector<cv::Mat> &images)
{
    if (!scene || images.empty())
        return;

    // 初始Y坐标
    int yPos = 0;
    const int horizontalSpacing = 0; // 水平居中位置
    const int verticalSpacing = 0;   // 图片间垂直间距

    // 存储图片项目，用于后续添加拼接线
    QList<MovablePixmapItem *> imageItems;

    // 将所有图像添加到场景中
    for (const auto &image : images)
    {
        // 将 OpenCV 的 BGR 格式转换为 RGB 格式
        cv::Mat rgbImage;
        if (image.channels() == 3)
        {
            cv::cvtColor(image, rgbImage, cv::COLOR_BGR2RGB);
        }
        else
        {
            rgbImage = image.clone();
        }

        // 创建 QImage 并从 cv::Mat 复制数据
        QImage qimg(rgbImage.data, rgbImage.cols, rgbImage.rows, rgbImage.step,
                    image.channels() == 3 ? QImage::Format_RGB888 : QImage::Format_Grayscale8);

        // 将 QImage 转换为 QPixmap
        QPixmap pixmap = QPixmap::fromImage(qimg);

        // 创建自定义的可拖动项
        MovablePixmapItem *item = new MovablePixmapItem(pixmap, MV_V, horizontalSpacing, yPos, 0);
        scene->addItem(item);

        // 添加到列表中
        imageItems.append(item);

        // 更新下一个图片的Y位置
        yPos += pixmap.height() + verticalSpacing;
    }

    // 在相邻图片之间添加拼接线
    for (int i = 0; i < imageItems.size() - 1; i++)
    {
        MovablePixmapItem *currentItem = imageItems[i];
        MovablePixmapItem *nextItem = imageItems[i + 1];

        // 计算拼接线位置（两个图片之间的边界）
        qreal lineY = currentItem->pos().y() + currentItem->pixmap().height();
        qreal lineX1 = qMin(currentItem->pos().x(), nextItem->pos().x()) - 30; // 向左扩展30像素
        qreal lineX2 = qMax(currentItem->pos().x() + currentItem->pixmap().width(),
                            nextItem->pos().x() + nextItem->pixmap().width()) +
                       30; // 向右扩展30像素

        // 创建水平拼接线
        SplicingLine *splicingLine = new SplicingLine(lineX1, lineY, lineX2, lineY,
                                                      SplicingLineOrientation::Horizontal);
        splicingLine->setExtensionLength(30);
        splicingLine->setLineWidth(2.0, 8.0);
        splicingLine->setZValue(1); // 确保拼接线在图片之上
        scene->addItem(splicingLine);

        // 设置图片项和拼接线的关联关系
        currentItem->setBottomSplicingLine(splicingLine); // 当前图片的下拼接线
        nextItem->setTopSplicingLine(splicingLine);       // 下一个图片的上拼接线
        splicingLine->setLastItem(currentItem);
        splicingLine->setNextItem(nextItem);
    }
}

// 创建优化的水平场景
void MainWindow::createOptimizedHorizontalScene(QGraphicsScene *scene, const std::vector<cv::Mat> &images)
{
    if (!scene || images.empty())
        return;

    // 初始X坐标
    int xPos = 0;
    const int verticalSpacing = 0;   // 垂直居中位置
    const int horizontalSpacing = 0; // 图片间水平间距

    // 存储图片项目，用于后续添加拼接线
    QList<MovablePixmapItem *> imageItems;

    // 将所有图像添加到场景中
    for (const auto &image : images)
    {
        // 将 OpenCV 的 BGR 格式转换为 RGB 格式
        cv::Mat rgbImage;
        if (image.channels() == 3)
        {
            cv::cvtColor(image, rgbImage, cv::COLOR_BGR2RGB);
        }
        else
        {
            rgbImage = image.clone();
        }

        // 创建 QImage 并从 cv::Mat 复制数据
        QImage qimg(rgbImage.data, rgbImage.cols, rgbImage.rows, rgbImage.step,
                    image.channels() == 3 ? QImage::Format_RGB888 : QImage::Format_Grayscale8);

        // 将 QImage 转换为 QPixmap
        QPixmap pixmap = QPixmap::fromImage(qimg);

        // 创建自定义的可拖动项
        MovablePixmapItem *item = new MovablePixmapItem(pixmap, MV_H, xPos, verticalSpacing, 0);
        scene->addItem(item);
        // 添加到列表中
        imageItems.append(item);

        // 更新下一个图片的X位置
        xPos += pixmap.width() + horizontalSpacing;
    }

    // 在相邻图片之间添加拼接线
    for (int i = 0; i < imageItems.size() - 1; i++)
    {
        MovablePixmapItem *currentItem = imageItems[i];
        MovablePixmapItem *nextItem = imageItems[i + 1];

        // 计算拼接线位置（两个图片之间的边界）
        qreal lineX = currentItem->pos().x() + currentItem->pixmap().width();
        qreal lineY1 = qMin(currentItem->pos().y(), nextItem->pos().y()) - 30; // 向上扩展30像素
        qreal lineY2 = qMax(currentItem->pos().y() + currentItem->pixmap().height(),
                            nextItem->pos().y() + nextItem->pixmap().height()) +
                       30; // 向下扩展30像素

        // 创建垂直拼接线
        SplicingLine *splicingLine = new SplicingLine(lineX, lineY1, lineX, lineY2,
                                                      SplicingLineOrientation::Vertical);
        splicingLine->setExtensionLength(30);
        splicingLine->setLineWidth(2.0, 8.0);
        splicingLine->setZValue(1); // 确保拼接线在图片之上
        scene->addItem(splicingLine);

        // 设置图片项和拼接线的关联关系
        currentItem->setRightSplicingLine(splicingLine); // 当前图片的右拼接线
        nextItem->setLeftSplicingLine(splicingLine);     // 下一个图片的左拼接线
        splicingLine->setLastItem(currentItem);
        splicingLine->setNextItem(nextItem);
    }
}
