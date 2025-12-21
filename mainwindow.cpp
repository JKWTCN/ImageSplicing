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
    // todo 等待优化功能
    ui->pushButton_editSelect->setVisible(false);
    ui->pushButton_editResult->setVisible(false);
    // 自动拼接功能已实现，显示按钮
    // ui->pushButton_auto->setVisible(false);
    if (GetSplicingTypeConfig() != ST_RAW)
        SetPaddingColorTypeConfig(PT_TRANSPARENT);

    // 启用拖放功能
    setAcceptDrops(true);
    // 让 graphicsView_result 的视口也接受拖放并安装事件过滤器
    if (ui->graphicsView_result && ui->graphicsView_result->viewport())
    {
        ui->graphicsView_result->viewport()->setAcceptDrops(true);
        ui->graphicsView_result->viewport()->installEventFilter(this);
    }
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

    // 应用文件排序
    filePaths = SortFilePaths(filePaths, GetFileSortTypeConfig());

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
    ui->pushButton_copy->setEnabled(true);
    ui->pushButton_add->setEnabled(true);
    ui->pushButton_deleteAll->setEnabled(true);
    ui->pushButton_editResult->setEnabled(true);
    ui->pushButton_editSelect->setEnabled(true);
}

// 锁定图片调整按钮
void MainWindow::LockPostionButton()
{
    ui->pushButton_5->setEnabled(false);
    ui->pushButton_6->setEnabled(false);
    ui->pushButton_7->setEnabled(false);
    ui->pushButton_copy->setEnabled(false);
    ui->pushButton_add->setEnabled(false);
    ui->pushButton_deleteAll->setEnabled(false);
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

        // 应用文件排序
        filePaths = SortFilePaths(filePaths, GetFileSortTypeConfig());

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

    // 添加第一张图片前的拼接线
    if (!imageItems.isEmpty())
    {
        MovablePixmapItem *firstItem = imageItems[0];
        qreal lineX = firstItem->pos().x();                                      // 在第一张图片左侧
        qreal lineY1 = firstItem->pos().y() - 30;                                // 向上扩展30像素
        qreal lineY2 = firstItem->pos().y() + firstItem->pixmap().height() + 30; // 向下扩展30像素

        // 创建第一张图片左侧的拼接线
        SplicingLine *leftSplicingLine = new SplicingLine(lineX, lineY1, lineX, lineY2,
                                                          SplicingLineOrientation::Vertical);
        leftSplicingLine->setExtensionLength(30);
        leftSplicingLine->setLineWidth(2.0, 8.0);
        leftSplicingLine->setZValue(1);
        scene->addItem(leftSplicingLine);

        // 设置第一张图片的左拼接线
        firstItem->setLeftSplicingLine(leftSplicingLine);
        leftSplicingLine->setNextItem(firstItem);
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

    // 添加最后一张图片后的拼接线
    if (!imageItems.isEmpty())
    {
        MovablePixmapItem *lastItem = imageItems.last();
        qreal lineX = lastItem->pos().x() + lastItem->pixmap().width();        // 在最后一张图片右侧处
        qreal lineY1 = lastItem->pos().y() - 30;                               // 向上扩展30像素
        qreal lineY2 = lastItem->pos().y() + lastItem->pixmap().height() + 30; // 向下扩展30像素

        // 创建最后一张图片右侧的拼接线
        SplicingLine *rightSplicingLine = new SplicingLine(lineX, lineY1, lineX, lineY2,
                                                           SplicingLineOrientation::Vertical);
        rightSplicingLine->setExtensionLength(30);
        rightSplicingLine->setLineWidth(2.0, 8.0);
        rightSplicingLine->setZValue(1);
        scene->addItem(rightSplicingLine);

        // 设置最后一张图片的右拼接线
        lastItem->setRightSplicingLine(rightSplicingLine);
        rightSplicingLine->setLastItem(lastItem);
    }
    ui->pushButton_auto->setEnabled(true);
    ui->pushButton_save->setEnabled(true);
    if (!scene || scene->items().isEmpty())
        return;

    // 只计算图片项目的边界矩形，忽略拼接线
    QRectF boundingRect;
    bool hasImageItems = false;
    for (auto item : scene->items())
    {
        // 只考虑MovablePixmapItem类型的项目
        if (MovablePixmapItem *pixmapItem = dynamic_cast<MovablePixmapItem *>(item))
        {
            if (!hasImageItems)
            {
                boundingRect = pixmapItem->boundingRect().translated(pixmapItem->pos());
                hasImageItems = true;
            }
            else
            {
                boundingRect = boundingRect.united(pixmapItem->boundingRect().translated(pixmapItem->pos()));
            }
        }
    }

    // 调整场景大小
    if (!boundingRect.isNull() && hasImageItems)
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

        // 应用文件排序
        filePaths = SortFilePaths(filePaths, GetFileSortTypeConfig());

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

    // 添加第一张图片前的拼接线
    if (!imageItems.isEmpty())
    {
        MovablePixmapItem *firstItem = imageItems[0];
        qreal lineY = firstItem->pos().y();                                     // 在第一张图片上方
        qreal lineX1 = firstItem->pos().x() - 30;                               // 向左扩展30像素
        qreal lineX2 = firstItem->pos().x() + firstItem->pixmap().width() + 30; // 向右扩展30像素

        // 创建第一张图片上方的拼接线
        SplicingLine *topSplicingLine = new SplicingLine(lineX1, lineY, lineX2, lineY,
                                                         SplicingLineOrientation::Horizontal);
        topSplicingLine->setExtensionLength(30);
        topSplicingLine->setLineWidth(2.0, 8.0);
        topSplicingLine->setZValue(1);
        scene->addItem(topSplicingLine);

        // 设置第一张图片的上拼接线
        firstItem->setTopSplicingLine(topSplicingLine);
        topSplicingLine->setNextItem(firstItem);
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

    // 添加最后一张图片后的拼接线
    if (!imageItems.isEmpty())
    {
        MovablePixmapItem *lastItem = imageItems.last();
        qreal lineY = lastItem->pos().y() + lastItem->pixmap().height();      // 在最后一张图片下方
        qreal lineX1 = lastItem->pos().x() - 30;                              // 向左扩展30像素
        qreal lineX2 = lastItem->pos().x() + lastItem->pixmap().width() + 30; // 向右扩展30像素

        // 创建最后一张图片下方的拼接线
        SplicingLine *bottomSplicingLine = new SplicingLine(lineX1, lineY, lineX2, lineY,
                                                            SplicingLineOrientation::Horizontal);
        bottomSplicingLine->setExtensionLength(30);
        bottomSplicingLine->setLineWidth(2.0, 8.0);
        bottomSplicingLine->setZValue(1);
        scene->addItem(bottomSplicingLine);

        // 设置最后一张图片的下拼接线
        lastItem->setBottomSplicingLine(bottomSplicingLine);
        bottomSplicingLine->setLastItem(lastItem);
    }
    ui->pushButton_auto->setEnabled(true);
    ui->pushButton_save->setEnabled(true);
    if (!scene || scene->items().isEmpty())
        return;

    // 只计算图片项目的边界矩形，忽略拼接线
    QRectF boundingRect;
    bool hasImageItems = false;
    for (auto item : scene->items())
    {
        // 只考虑MovablePixmapItem类型的项目
        if (MovablePixmapItem *pixmapItem = dynamic_cast<MovablePixmapItem *>(item))
        {
            if (!hasImageItems)
            {
                boundingRect = pixmapItem->boundingRect().translated(pixmapItem->pos());
                hasImageItems = true;
            }
            else
            {
                boundingRect = boundingRect.united(pixmapItem->boundingRect().translated(pixmapItem->pos()));
            }
        }
    }

    // 调整场景大小
    if (!boundingRect.isNull() && hasImageItems)
    {
        scene->setSceneRect(boundingRect);
        ui->graphicsView_result->fitInView(boundingRect, Qt::KeepAspectRatio);
    }
    ui->graphicsView_result->setScene(scene); // 设置场景到 graphicsView
    scene->installEventFilter(this);
}
// 智能自动拼接功能
void MainWindow::on_pushButton_auto_clicked()
{
    // 检查是否有图片
    if (filePaths.length() == 0)
    {
        filePaths = OpenImagePaths();

        if (filePaths.length() == 0)
        {
            showNoticeMessageBox("提示", "请先选择要拼接的图片。");
            return;
        }

        // 应用文件排序
        filePaths = SortFilePaths(filePaths, GetFileSortTypeConfig());

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

    // 执行智能自动拼接
    performIntelligentAutoStitch();
}

// 执行智能自动拼接的主函数
void MainWindow::performIntelligentAutoStitch()
{
    if (filePaths.isEmpty())
    {
        showErrorMessageBox("错误", "没有可用的图片文件。");
        return;
    }

    try
    {
        // 加载所有图片
        images.clear();
        for (const auto &path : filePaths)
        {
            cv::Mat img = cv::imread(path.toLocal8Bit().constData(), cv::IMREAD_COLOR);
            if (img.empty())
            {
                cout << "Failed to load image:" << path.toLocal8Bit().constData();
                showErrorMessageBox("错误", "不能读取文件: " + path.toStdString());
                return;
            }
            images.push_back(img);
        }

        if (images.size() == 1)
        {
            showNoticeMessageBox("提示", "只有一张图片，无需拼接。");
            return;
        }

        // 执行智能拼接，根据用户之前的选择决定拼接方向
        bool useVerticalStitching = (splicingState == SS_VERTICAL);
        cv::Mat stitchedResult = performSmartStitching(images, useVerticalStitching);

        if (stitchedResult.empty())
        {
            showErrorMessageBox("错误", "自动拼接失败，请尝试手动拼接。");
            return;
        }

        // 创建场景显示结果
        QGraphicsScene *scene = new QGraphicsScene(this);
        createAutoStitchScene(scene, stitchedResult);

        // 设置场景
        ui->graphicsView_result->setScene(scene);
        scene->installEventFilter(this);

        // 设置场景边界并适应视图
        scene->setSceneRect(scene->itemsBoundingRect());
        ui->graphicsView_result->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);

        // 根据用户之前选择的拼接模式来决定自动调整的方向
        // 如果之前选择了垂直拼接，自动调整也使用垂直拼接
        // 如果之前选择了水平拼接，自动调整也使用水平拼接
        if (splicingState == SS_VERTICAL)
        {
            splicingState = SS_AUTO_VERTICAL;
        }
        else if (splicingState == SS_HORIZONTAL)
        {
            splicingState = SS_AUTO_HORIZONTAL;
        }
        else
        {
            // 如果没有之前的选择，默认使用垂直拼接
            splicingState = SS_AUTO_VERTICAL;
        }
        ui->pushButton_save->setEnabled(true);

        showNoticeMessageBox("成功", "智能自动拼接完成！\n提示：使用Ctrl+滚轮可以缩放查看结果。");
    }
    catch (cv::Exception &e)
    {
        std::cerr << "OpenCV异常: " << e.what() << std::endl;
        showErrorMessageBox("错误", "图像处理异常: " + std::string(e.what()));
    }
    catch (std::exception &e)
    {
        std::cerr << "标准异常: " << e.what() << std::endl;
        showErrorMessageBox("错误", "处理异常: " + std::string(e.what()));
    }
    catch (...)
    {
        std::cerr << "未知异常" << std::endl;
        showErrorMessageBox("错误", "发生未知异常");
    }
}

// 执行智能拼接算法
cv::Mat MainWindow::performSmartStitching(const std::vector<cv::Mat> &images, bool useVerticalStitching)
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
        cv::Mat result;

        // 对于多张图片的情况，优先使用智能配对拼接策略
        if (images.size() > 2)
        {
            std::cout << "Processing " << images.size() << " images using smart multi-image strategy..." << std::endl;

            // 使用分步拼接策略，保持用户指定的方向
            result = performDirectionalMultiStitching(images, useVerticalStitching);

            if (!result.empty())
            {
                std::cout << "Multi-image directional stitching successful!" << std::endl;
                return result;
            }
        }
        else if (images.size() == 2)
        {
            // 对于两张图片，可以尝试智能拼接
            std::cout << "Processing 2 images, trying intelligent stitching..." << std::endl;

            if (AutoStitchImages(images, result))
            {
                // 检查方向是否符合用户期望
                bool directionCorrect = (useVerticalStitching && result.rows >= result.cols) ||
                                      (!useVerticalStitching && result.cols >= result.rows);

                if (directionCorrect)
                {
                    std::cout << "Two-image intelligent stitching successful with correct direction!" << std::endl;
                    return result;
                }
                else
                {
                    std::cout << "Direction mismatch, falling back to directional stitching..." << std::endl;
                }
            }
        }

        // 对于失败的情况或多图方向不对，使用定向拼接
        std::cout << "Using directional stitching as fallback..." << std::endl;

        if (useVerticalStitching)
        {
            return performEnhancedVerticalStitching(images);
        }
        else
        {
            return performEnhancedHorizontalStitching(images);
        }
    }
    catch (const cv::Exception &e)
    {
        std::cerr << "performSmartStitching exception: " << e.what() << std::endl;

        // 最后的备用方案：基础拼接
        try
        {
            cv::Mat simpleResult;
            if (useVerticalStitching)
            {
                cv::vconcat(images, simpleResult);
            }
            else
            {
                cv::hconcat(images, simpleResult);
            }
            return simpleResult;
        }
        catch (...)
        {
            return cv::Mat();
        }
    }
}

// 多图片定向拼接函数
cv::Mat MainWindow::performDirectionalMultiStitching(const std::vector<cv::Mat> &images, bool useVerticalStitching)
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
        std::cout << "Starting tournament-style multi-stitching with " << images.size() << " images" << std::endl;

        // 渐进式两两拼接策略
        std::vector<cv::Mat> currentImages = images;

        // 持续进行两两拼接，直到只剩一张图片
        while (currentImages.size() > 1)
        {
            std::cout << "Round with " << currentImages.size() << " images" << std::endl;

            // 找到最适合拼接的一对
            auto bestPair = findBestStitchingPair(currentImages, useVerticalStitching);

            if (bestPair.first == -1 || bestPair.second == -1)
            {
                // 如果找不到合适的配对，使用简单拼接
                std::cout << "No suitable pair found, using simple concatenation" << std::endl;
                return performSimpleDirectionalStitching(currentImages, useVerticalStitching);
            }

            // 拼接最佳配对
            cv::Mat stitchedPair = stitchPair(currentImages[bestPair.first],
                                            currentImages[bestPair.second],
                                            useVerticalStitching);

            if (stitchedPair.empty())
            {
                // 如果拼接失败，移除这张图片继续
                std::cout << "Pair stitching failed, removing one image" << std::endl;
                currentImages.erase(currentImages.begin() + bestPair.first);
                continue;
            }

            std::cout << "Successfully stitched images " << bestPair.first << " and " << bestPair.second << std::endl;

            // 创建新的图片列表
            std::vector<cv::Mat> nextRoundImages;

            // 添加未拼接的图片（从后往前添加避免索引变化）
            for (int i = currentImages.size() - 1; i >= 0; --i)
            {
                if (i != bestPair.first && i != bestPair.second)
                {
                    nextRoundImages.insert(nextRoundImages.begin(), currentImages[i]);
                }
            }

            // 添加拼接结果
            nextRoundImages.push_back(stitchedPair);

            // 更新当前图片列表
            currentImages = nextRoundImages;
        }

        std::cout << "Tournament stitching completed successfully!" << std::endl;

        // 不应用增强，保持原始颜色
        if (!currentImages.empty())
        {
            return currentImages[0];
        }

        return cv::Mat();
    }
    catch (const cv::Exception &e)
    {
        std::cerr << "Tournament multi-stitching exception: " << e.what() << std::endl;

        // 备用方案：简单拼接
        return performSimpleDirectionalStitching(images, useVerticalStitching);
    }
}

// 找到最适合拼接的一对图片
std::pair<int, int> MainWindow::findBestStitchingPair(const std::vector<cv::Mat> &images, bool useVerticalStitching)
{
    if (images.size() < 2)
    {
        return std::make_pair(-1, -1);
    }

    std::pair<int, int> bestPair = std::make_pair(0, 1);
    double bestScore = -1.0;

    std::cout << "Evaluating " << (images.size() * (images.size() - 1) / 2) << " possible pairs..." << std::endl;

    // 遍历所有可能的配对
    for (size_t i = 0; i < images.size(); ++i)
    {
        for (size_t j = i + 1; j < images.size(); ++j)
        {
            // 计算这对图片的拼接适应性评分
            double score = evaluatePairCompatibility(images[i], images[j], useVerticalStitching);

            if (score > bestScore)
            {
                bestScore = score;
                bestPair = std::make_pair(i, j);
            }
        }
    }

    std::cout << "Best pair found: (" << bestPair.first << ", " << bestPair.second
              << ") with score: " << bestScore << std::endl;

    // 如果评分太低，返回无效配对
    if (bestScore < 0.3)
    {
        return std::make_pair(-1, -1);
    }

    return bestPair;
}

// 评估两张图片的拼接适应性
double MainWindow::evaluatePairCompatibility(const cv::Mat &img1, const cv::Mat &img2, bool useVerticalStitching)
{
    if (img1.empty() || img2.empty())
    {
        return 0.0;
    }

    try
    {
        double score = 0.0;

        // 1. 尺寸兼容性评分
        double sizeScore = 0.0;
        if (useVerticalStitching)
        {
            // 垂直拼接：宽度应该相似
            double widthRatio = std::min(img1.cols, img2.cols) /
                              static_cast<double>(std::max(img1.cols, img2.cols));
            sizeScore = widthRatio;
        }
        else
        {
            // 水平拼接：高度应该相似
            double heightRatio = std::min(img1.rows, img2.rows) /
                               static_cast<double>(std::max(img1.rows, img2.rows));
            sizeScore = heightRatio;
        }

        score += sizeScore * 0.3; // 权重30%

        // 2. 智能拼接成功性评分（尝试快速拼接）
        double stitchingScore = 0.0;
        std::vector<cv::Mat> testPair = {img1, img2};
        cv::Mat testResult;

        // 创建小尺寸版本进行快速测试
        cv::Mat smallImg1, smallImg2;
        cv::resize(img1, smallImg1, cv::Size(), 0.3, 0.3, cv::INTER_AREA);
        cv::resize(img2, smallImg2, cv::Size(), 0.3, 0.3, cv::INTER_AREA);

        std::vector<cv::Mat> smallPair = {smallImg1, smallImg2};

        if (AutoStitchImages(smallPair, testResult))
        {
            // 检查方向是否符合要求
            bool directionCorrect = (useVerticalStitching && testResult.rows >= testResult.cols) ||
                                  (!useVerticalStitching && testResult.cols >= testResult.rows);

            stitchingScore = directionCorrect ? 1.0 : 0.5;
        }
        else
        {
            // 智能拼接失败，给较低分数
            stitchingScore = 0.2;
        }

        score += stitchingScore * 0.5; // 权重50%

        // 3. 内容相似性评分（简单的直方图比较）
        double similarityScore = calculateContentSimilarity(img1, img2);
        score += similarityScore * 0.2; // 权重20%

        return std::min(score, 1.0); // 限制在0-1范围内
    }
    catch (...)
    {
        return 0.1; // 异常情况下给很低的分数
    }
}

// 计算内容相似性
double MainWindow::calculateContentSimilarity(const cv::Mat &img1, const cv::Mat &img2)
{
    try
    {
        // 转换为HSV颜色空间进行更好的比较
        cv::Mat hsv1, hsv2;
        cv::cvtColor(img1, hsv1, cv::COLOR_BGR2HSV);
        cv::cvtColor(img2, hsv2, cv::COLOR_BGR2HSV);

        // 计算直方图
        int histSize[] = {50, 60};
        float h_ranges[] = {0, 180};
        float s_ranges[] = {0, 256};
        const float* ranges[] = {h_ranges, s_ranges};
        int channels[] = {0, 1};

        cv::Mat hist1, hist2;
        cv::calcHist(&hsv1, 1, channels, cv::Mat(), hist1, 2, histSize, ranges);
        cv::calcHist(&hsv2, 1, channels, cv::Mat(), hist2, 2, histSize, ranges);

        // 归一化
        cv::normalize(hist1, hist1, 0, 1, cv::NORM_L1);
        cv::normalize(hist2, hist2, 0, 1, cv::NORM_L1);

        // 计算相关性
        double correlation = cv::compareHist(hist1, hist2, cv::HISTCMP_CORREL);

        // 转换为0-1分数
        return (correlation + 1.0) / 2.0;
    }
    catch (...)
    {
        return 0.5; // 异常情况下返回中等相似性
    }
}

// 拼接一对图片
cv::Mat MainWindow::stitchPair(const cv::Mat &img1, const cv::Mat &img2, bool useVerticalStitching)
{
    try
    {
        std::vector<cv::Mat> pair = {img1, img2};
        cv::Mat result;

        // 首先尝试智能拼接
        if (AutoStitchImages(pair, result))
        {
            // 检查方向是否符合要求
            bool directionCorrect = (useVerticalStitching && result.rows >= result.cols) ||
                                  (!useVerticalStitching && result.cols >= result.rows);

            if (directionCorrect)
            {
                return result;
            }
        }

        // 智能拼接失败或方向不对，使用简单拼接
        if (useVerticalStitching)
        {
            cv::vconcat(pair, result);
        }
        else
        {
            cv::hconcat(pair, result);
        }

        return result;
    }
    catch (...)
    {
        return cv::Mat();
    }
}

// 简单定向拼接（备用方案）
cv::Mat MainWindow::performSimpleDirectionalStitching(const std::vector<cv::Mat> &images, bool useVerticalStitching)
{
    try
    {
        cv::Mat result;

        // 统一尺寸
        std::vector<cv::Mat> normalizedImages;

        if (useVerticalStitching)
        {
            int targetWidth = calculateOptimalWidth(images);
            if (targetWidth > 0)
            {
                for (const auto& img : images)
                {
                    cv::Mat resized = resizeToWidth(img, targetWidth);
                    if (!resized.empty())
                    {
                        normalizedImages.push_back(resized);
                    }
                }
            }
        }
        else
        {
            int targetHeight = calculateOptimalHeight(images);
            if (targetHeight > 0)
            {
                for (const auto& img : images)
                {
                    cv::Mat resized = resizeToHeight(img, targetHeight);
                    if (!resized.empty())
                    {
                        normalizedImages.push_back(resized);
                    }
                }
            }
        }

        if (normalizedImages.empty())
        {
            normalizedImages = images;
        }

        // 简单拼接
        if (useVerticalStitching)
        {
            cv::vconcat(normalizedImages, result);
        }
        else
        {
            cv::hconcat(normalizedImages, result);
        }

        return result;
    }
    catch (...)
    {
        return cv::Mat();
    }
}

// 计算最优统一宽度
int MainWindow::calculateOptimalWidth(const std::vector<cv::Mat> &images)
{
    if (images.empty())
    {
        return 0;
    }

    try
    {
        // 计算所有图片宽度的中位数，避免极端值影响
        std::vector<int> widths;
        for (const auto& img : images)
        {
            if (!img.empty())
            {
                widths.push_back(img.cols);
            }
        }

        if (widths.empty())
        {
            return 0;
        }

        // 排序并找到中位数
        std::sort(widths.begin(), widths.end());
        int medianWidth = widths[widths.size() / 2];

        // 限制最大宽度以避免过度缩放
        int maxWidth = *std::max_element(widths.begin(), widths.end());
        int minWidth = *std::min_element(widths.begin(), widths.end());

        // 选择一个合理的宽度，偏向较小值以减少放大操作
        int optimalWidth = medianWidth;
        if (medianWidth > maxWidth * 0.8)
        {
            optimalWidth = maxWidth * 0.8; // 不超过最大宽度的80%
        }
        if (optimalWidth < minWidth * 1.2)
        {
            optimalWidth = minWidth * 1.2; // 不小于最小宽度的120%
        }

        std::cout << "Calculated optimal width: " << optimalWidth << " (median: " << medianWidth << ")" << std::endl;
        return optimalWidth;
    }
    catch (...)
    {
        return images[0].cols; // 返回第一张图的宽度作为备用
    }
}

// 计算最优统一高度
int MainWindow::calculateOptimalHeight(const std::vector<cv::Mat> &images)
{
    if (images.empty())
    {
        return 0;
    }

    try
    {
        // 计算所有图片高度的中位数
        std::vector<int> heights;
        for (const auto& img : images)
        {
            if (!img.empty())
            {
                heights.push_back(img.rows);
            }
        }

        if (heights.empty())
        {
            return 0;
        }

        // 排序并找到中位数
        std::sort(heights.begin(), heights.end());
        int medianHeight = heights[heights.size() / 2];

        // 限制最大高度以避免过度缩放
        int maxHeight = *std::max_element(heights.begin(), heights.end());
        int minHeight = *std::min_element(heights.begin(), heights.end());

        // 选择一个合理的高度，偏向较小值
        int optimalHeight = medianHeight;
        if (optimalHeight > maxHeight * 0.8)
        {
            optimalHeight = maxHeight * 0.8;
        }
        if (optimalHeight < minHeight * 1.2)
        {
            optimalHeight = minHeight * 1.2;
        }

        std::cout << "Calculated optimal height: " << optimalHeight << " (median: " << medianHeight << ")" << std::endl;
        return optimalHeight;
    }
    catch (...)
    {
        return images[0].rows; // 返回第一张图的高度作为备用
    }
}

// 调整到指定宽度，保持高宽比
cv::Mat MainWindow::resizeToWidth(const cv::Mat &image, int targetWidth)
{
    if (image.empty() || targetWidth <= 0)
    {
        return cv::Mat();
    }

    try
    {
        // 计算对应的高度以保持高宽比
        double aspectRatio = static_cast<double>(image.rows) / image.cols;
        int targetHeight = static_cast<int>(targetWidth * aspectRatio);

        cv::Mat resized;
        cv::resize(image, resized, cv::Size(targetWidth, targetHeight), 0, 0, cv::INTER_AREA);
        return resized;
    }
    catch (...)
    {
        return image.clone();
    }
}

// 调整到指定高度，保持高宽比
cv::Mat MainWindow::resizeToHeight(const cv::Mat &image, int targetHeight)
{
    if (image.empty() || targetHeight <= 0)
    {
        return cv::Mat();
    }

    try
    {
        // 计算对应的宽度以保持高宽比
        double aspectRatio = static_cast<double>(image.cols) / image.rows;
        int targetWidth = static_cast<int>(targetHeight * aspectRatio);

        cv::Mat resized;
        cv::resize(image, resized, cv::Size(targetWidth, targetHeight), 0, 0, cv::INTER_AREA);
        return resized;
    }
    catch (...)
    {
        return image.clone();
    }
}

// 应用强制性轻微增强
cv::Mat MainWindow::applyMandatoryEnhancement(const cv::Mat &image)
{
    if (image.empty())
    {
        return cv::Mat();
    }

    try
    {
        cv::Mat enhanced = image.clone();

        // 应用非常轻微的对比度调整，避免过度处理
        enhanced.convertTo(enhanced, -1, 1.02, 2); // 轻微增加对比度和亮度

        // 轻微的锐化
        cv::Mat sharpenKernel = (cv::Mat_<float>(3,3) <<
                                 0, -0.5, 0,
                                -0.5,  3, -0.5,
                                 0, -0.5, 0);

        cv::Mat sharpened;
        cv::filter2D(enhanced, sharpened, -1, sharpenKernel);

        // 只混合少量锐化效果
        cv::addWeighted(enhanced, 0.85, sharpened, 0.15, 0, enhanced);

        return enhanced;
    }
    catch (...)
    {
        return image.clone();
    }
}

// 调整为垂直方向
cv::Mat MainWindow::adjustToVertical(const cv::Mat &image)
{
    if (image.empty())
    {
        return cv::Mat();
    }

    try
    {
        // 如果图像已经很接近垂直，使用智能裁剪
        if (image.rows >= image.cols * 0.8)
        {
            return trySmartCropping(image, true);
        }

        // 否则进行智能重排
        return tryVerticalReorganization(image);
    }
    catch (...)
    {
        return image.clone();
    }
}

// 调整为水平方向
cv::Mat MainWindow::adjustToHorizontal(const cv::Mat &image)
{
    if (image.empty())
    {
        return cv::Mat();
    }

    try
    {
        // 如果图像已经很接近水平，使用智能裁剪
        if (image.cols >= image.rows * 0.8)
        {
            return trySmartCropping(image, false);
        }

        // 否则进行智能重排
        return tryHorizontalReorganization(image);
    }
    catch (...)
    {
        return image.clone();
    }
}

// 垂直重组
cv::Mat MainWindow::tryVerticalReorganization(const cv::Mat &image)
{
    try
    {
        // 将图像分成上下两部分，然后垂直排列
        if (image.rows < 200 || image.cols < 100)
        {
            return cv::Mat(); // 图像太小
        }

        // 计算新的垂直尺寸
        int newHeight = static_cast<int>(image.cols * 1.5); // 3:2 宽高比
        if (newHeight > image.rows * 2)
        {
            newHeight = image.rows * 2; // 限制最大高度
        }

        // 创建目标画布
        cv::Mat result = cv::Mat::zeros(newHeight, image.cols, image.type());

        // 将原图像分成2-3部分，垂直排列
        int parts = 2;
        int partHeight = image.rows / parts;

        for (int i = 0; i < parts; ++i)
        {
            int startY = i * partHeight;
            int endY = (i == parts - 1) ? image.rows : startY + partHeight;
            int targetY = i * (newHeight / parts);

            cv::Rect srcRect(0, startY, image.cols, endY - startY);
            cv::Rect dstRect(0, targetY, image.cols, newHeight / parts);

            if (srcRect.x + srcRect.width <= image.cols &&
                srcRect.y + srcRect.height <= image.rows &&
                dstRect.x + dstRect.width <= result.cols &&
                dstRect.y + dstRect.height <= result.rows)
            {
                cv::Mat srcSegment = image(srcRect);
                cv::Mat dstSegment = result(dstRect);
                cv::resize(srcSegment, dstSegment, dstSegment.size());
            }
        }

        return result;
    }
    catch (...)
    {
        return cv::Mat();
    }
}

// 水平重组
cv::Mat MainWindow::tryHorizontalReorganization(const cv::Mat &image)
{
    try
    {
        // 将图像分成左右两部分，然后水平排列
        if (image.cols < 200 || image.rows < 100)
        {
            return cv::Mat(); // 图像太小
        }

        // 计算新的水平尺寸
        int newWidth = static_cast<int>(image.rows * 1.5); // 3:2 宽高比
        if (newWidth > image.cols * 2)
        {
            newWidth = image.cols * 2; // 限制最大宽度
        }

        // 创建目标画布
        cv::Mat result = cv::Mat::zeros(image.rows, newWidth, image.type());

        // 将原图像分成2-3部分，水平排列
        int parts = 2;
        int partWidth = image.cols / parts;

        for (int i = 0; i < parts; ++i)
        {
            int startX = i * partWidth;
            int endX = (i == parts - 1) ? image.cols : startX + partWidth;
            int targetX = i * (newWidth / parts);

            cv::Rect srcRect(startX, 0, endX - startX, image.rows);
            cv::Rect dstRect(targetX, 0, newWidth / parts, image.rows);

            if (srcRect.x + srcRect.width <= image.cols &&
                srcRect.y + srcRect.height <= image.rows &&
                dstRect.x + dstRect.width <= result.cols &&
                dstRect.y + dstRect.height <= result.rows)
            {
                cv::Mat srcSegment = image(srcRect);
                cv::Mat dstSegment = result(dstRect);
                cv::resize(srcSegment, dstSegment, dstSegment.size());
            }
        }

        return result;
    }
    catch (...)
    {
        return cv::Mat();
    }
}

// 智能方向调整函数
cv::Mat MainWindow::tryIntelligentDirectionAdjustment(const cv::Mat &image, bool preferVertical)
{
    if (image.empty())
    {
        return cv::Mat();
    }

    try
    {
        // 如果图像已经符合期望的方向，直接返回
        if (preferVertical && image.rows >= image.cols)
        {
            return image.clone();
        }
        if (!preferVertical && image.cols >= image.rows)
        {
            return image.clone();
        }

        std::cout << "Attempting intelligent direction adjustment..." << std::endl;

        // 方法1: 尝试智能裁剪
        cv::Mat croppedResult = trySmartCropping(image, preferVertical);
        if (!croppedResult.empty())
        {
            std::cout << "Smart cropping successful!" << std::endl;
            return croppedResult;
        }

        // 方法2: 尝试内容感知缩放
        cv::Mat resizedResult = tryContentAwareResize(image, preferVertical);
        if (!resizedResult.empty())
        {
            std::cout << "Content-aware resize successful!" << std::endl;
            return resizedResult;
        }

        // 方法3: 分割重组
        cv::Mat reorganizedResult = trySegmentReorganization(image, preferVertical);
        if (!reorganizedResult.empty())
        {
            std::cout << "Segment reorganization successful!" << std::endl;
            return reorganizedResult;
        }

        std::cout << "All direction adjustment methods failed" << std::endl;
        return cv::Mat();
    }
    catch (const cv::Exception &e)
    {
        std::cerr << "Direction adjustment exception: " << e.what() << std::endl;
        return cv::Mat();
    }
}

// 智能裁剪辅助函数
cv::Mat MainWindow::trySmartCropping(const cv::Mat &image, bool preferVertical)
{
    try
    {
        if (preferVertical)
        {
            // 如果希望垂直，尝试裁剪左右两侧，保持中间部分
            int targetWidth = static_cast<int>(image.cols * 0.7); // 保留70%宽度
            if (targetWidth <= 0) return cv::Mat();

            int startX = (image.cols - targetWidth) / 2;
            cv::Rect cropRect(startX, 0, targetWidth, image.rows);

            if (cropRect.x >= 0 && cropRect.y >= 0 &&
                cropRect.x + cropRect.width <= image.cols &&
                cropRect.y + cropRect.height <= image.rows)
            {
                cv::Mat cropped = image(cropRect).clone();
                if (cropped.rows >= cropped.cols)
                {
                    return cropped;
                }
            }
        }
        else
        {
            // 如果希望水平，尝试裁剪上下两侧，保持中间部分
            int targetHeight = static_cast<int>(image.rows * 0.7); // 保留70%高度
            if (targetHeight <= 0) return cv::Mat();

            int startY = (image.rows - targetHeight) / 2;
            cv::Rect cropRect(0, startY, image.cols, targetHeight);

            if (cropRect.x >= 0 && cropRect.y >= 0 &&
                cropRect.x + cropRect.width <= image.cols &&
                cropRect.y + cropRect.height <= image.rows)
            {
                cv::Mat cropped = image(cropRect).clone();
                if (cropped.cols >= cropped.rows)
                {
                    return cropped;
                }
            }
        }

        return cv::Mat();
    }
    catch (...)
    {
        return cv::Mat();
    }
}

// 内容感知缩放辅助函数
cv::Mat MainWindow::tryContentAwareResize(const cv::Mat &image, bool preferVertical)
{
    try
    {
        cv::Mat result;
        double aspectRatio;

        if (preferVertical)
        {
            // 创建垂直方向的结果
            aspectRatio = 0.75; // 高宽比 3:4
            int newWidth = static_cast<int>(image.rows * aspectRatio);
            if (newWidth <= 0 || newWidth >= image.cols) return cv::Mat();

            cv::resize(image, result, cv::Size(newWidth, image.rows), 0, 0, cv::INTER_AREA);
        }
        else
        {
            // 创建水平方向的结果
            aspectRatio = 1.33; // 宽高比 4:3
            int newHeight = static_cast<int>(image.cols / aspectRatio);
            if (newHeight <= 0 || newHeight >= image.rows) return cv::Mat();

            cv::resize(image, result, cv::Size(image.cols, newHeight), 0, 0, cv::INTER_AREA);
        }

        return result;
    }
    catch (...)
    {
        return cv::Mat();
    }
}

// 分割重组辅助函数
cv::Mat MainWindow::trySegmentReorganization(const cv::Mat &image, bool preferVertical)
{
    try
    {
        if (preferVertical)
        {
            // 尝试将横向图像分割为垂直方向的片段
            int segments = 2;
            int segmentWidth = image.cols / segments;

            cv::Mat result = cv::Mat::zeros(image.rows, segmentWidth, image.type());

            for (int i = 0; i < segments; ++i)
            {
                int startY = (image.rows / segments) * i;
                int endY = (i == segments - 1) ? image.rows : startY + (image.rows / segments);

                cv::Rect srcRect(0, startY, image.cols, endY - startY);
                cv::Rect dstRect(0, startY, segmentWidth, endY - startY);

                if (srcRect.x + srcRect.width <= image.cols &&
                    srcRect.y + srcRect.height <= image.rows &&
                    dstRect.x + dstRect.width <= result.cols &&
                    dstRect.y + dstRect.height <= result.rows)
                {
                    cv::Mat srcSegment = image(srcRect);
                    cv::resize(srcSegment, srcSegment, cv::Size(segmentWidth, endY - startY));
                    cv::Mat dstSegment = result(dstRect);
                    srcSegment.copyTo(dstSegment);
                }
            }

            if (result.rows >= result.cols)
            {
                return result;
            }
        }

        return cv::Mat();
    }
    catch (...)
    {
        return cv::Mat();
    }
}

// 创建自动拼接场景
void MainWindow::createAutoStitchScene(QGraphicsScene *scene, const cv::Mat &stitchedResult)
{
    if (!scene || stitchedResult.empty())
        return;

    try
    {
        // 不进行质量增强，保持原始颜色
        // 将OpenCV的BGR格式转换为RGB格式
        cv::Mat rgbImage;
        if (stitchedResult.channels() == 3)
        {
            cv::cvtColor(stitchedResult, rgbImage, cv::COLOR_BGR2RGB);
        }
        else
        {
            rgbImage = stitchedResult.clone();
        }

        // 创建QImage
        QImage qimg(rgbImage.data, rgbImage.cols, rgbImage.rows, rgbImage.step,
                    rgbImage.channels() == 3 ? QImage::Format_RGB888 : QImage::Format_Grayscale8);

        // 转换为QPixmap
        QPixmap pixmap = QPixmap::fromImage(qimg);

        // 创建可拖动的图片项（MV_NONE表示不可移动，因为这是最终结果）
        MovablePixmapItem *item = new MovablePixmapItem(pixmap, MV_V, 0, 0, 0);
        item->setFlag(QGraphicsItem::ItemIsMovable, false); // 设为不可移动
        item->setFlag(QGraphicsItem::ItemIsSelectable, true); // 可选择

        scene->addItem(item);

        // 居中显示并添加适当的边距
        QRectF itemRect = item->boundingRect();
        QRectF sceneRect = itemRect.adjusted(-10, -10, 10, 10); // 添加10像素边距
        scene->setSceneRect(sceneRect);

        std::cout << "Auto-stitch scene created successfully with original colors" << std::endl;
    }
    catch (const cv::Exception &e)
    {
        std::cerr << "Error in createAutoStitchScene: " << e.what() << std::endl;
    }
}

// 增强拼接图像质量
cv::Mat MainWindow::enhanceStitchedImage(const cv::Mat &image)
{
    if (image.empty())
    {
        return cv::Mat();
    }

    try
    {
        cv::Mat enhanced = image.clone();

        // 1. 应用对比度增强
        cv::Mat labImage;
        cv::cvtColor(enhanced, labImage, cv::COLOR_BGR2Lab);

        std::vector<cv::Mat> labChannels;
        cv::split(labImage, labChannels);

        // 对亮度通道进行直方图均衡化
        cv::equalizeHist(labChannels[0], labChannels[0]);
        cv::merge(labChannels, labImage);
        cv::cvtColor(labImage, enhanced, cv::COLOR_Lab2BGR);

        // 2. 应用轻微的锐化滤波
        cv::Mat sharpenKernel = (cv::Mat_<float>(3,3) <<
                                 0, -1, 0,
                                -1,  5, -1,
                                 0, -1, 0);

        cv::Mat sharpened;
        cv::filter2D(enhanced, sharpened, -1, sharpenKernel);

        // 混合原图和锐化图，避免过度锐化
        cv::addWeighted(enhanced, 0.7, sharpened, 0.3, 0, enhanced);

        // 3. 应用双边滤波减少噪声但保持边缘
        cv::Mat bilateral;
        cv::bilateralFilter(enhanced, bilateral, 9, 80, 80);
        enhanced = bilateral;

        // 4. 调整亮度和对比度
        enhanced.convertTo(enhanced, -1, 1.1, 10); // alpha=1.1 (对比度), beta=10 (亮度)

        // 5. 应用轻微的伽马校正
        cv::Mat gammaCorrected = enhanced.clone();
        cv::Mat lookUpTable(1, 256, CV_8U);
        uchar* p = lookUpTable.ptr();
        float gamma = 0.9; // 略微增加对比度

        for (int i = 0; i < 256; ++i)
        {
            p[i] = cv::saturate_cast<uchar>(pow(i / 255.0, gamma) * 255.0);
        }

        cv::LUT(enhanced, lookUpTable, gammaCorrected);

        std::cout << "Image enhancement applied successfully" << std::endl;
        return gammaCorrected;
    }
    catch (const cv::Exception &e)
    {
        std::cerr << "Image enhancement failed: " << e.what() << std::endl;
        return image.clone(); // 返回原图
    }
}
// 事件过滤器
bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    // Handle zooming via graphics scene wheel
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
    // If the event comes from the graphics view viewport, forward drag/drop events
    if (obj == ui->graphicsView_result->viewport())
    {
        if (event->type() == QEvent::DragEnter)
        {
            QDragEnterEvent *dragEvent = static_cast<QDragEnterEvent *>(event);
            this->dragEnterEvent(dragEvent);
            return true;
        }
        else if (event->type() == QEvent::DragMove)
        {
            QDragMoveEvent *moveEvent = static_cast<QDragMoveEvent *>(event);
            this->dragMoveEvent(moveEvent);
            return true;
        }
        else if (event->type() == QEvent::Drop)
        {
            QDropEvent *dropEv = static_cast<QDropEvent *>(event);
            this->dropEvent(dropEv);
            return true;
        }
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

    // 只计算图片项目的有效显示边界矩形（考虑拼接线裁剪），忽略拼接线
    QRectF boundingRect;
    bool hasImageItems = false;
    for (auto item : scene->items())
    {
        // 只考虑MovablePixmapItem类型的项目
        if (MovablePixmapItem *pixmapItem = dynamic_cast<MovablePixmapItem *>(item))
        {
            // 计算图片项的有效显示区域（考虑拼接线的裁剪）
            QRectF itemRect = pixmapItem->boundingRect();
            QPointF itemPos = pixmapItem->pos();
            QRectF clipRect = itemRect;

            if (pixmapItem->getMoveType() == MV_V)
            {
                // 垂直拼接模式：考虑上下拼接线的裁剪
                if (pixmapItem->getTopSplicingLine())
                {
                    qreal lineY = pixmapItem->getTopSplicingLine()->line().y1();
                    qreal localTopY = lineY - itemPos.y();
                    clipRect.setTop(qMax(itemRect.top(), localTopY));
                }
                if (pixmapItem->getBottomSplicingLine())
                {
                    qreal lineY = pixmapItem->getBottomSplicingLine()->line().y1();
                    qreal localBottomY = lineY - itemPos.y();
                    clipRect.setBottom(qMin(itemRect.bottom(), localBottomY));
                }
            }
            else if (pixmapItem->getMoveType() == MV_H)
            {
                // 水平拼接模式：考虑左右拼接线的裁剪
                if (pixmapItem->getLeftSplicingLine())
                {
                    qreal lineX = pixmapItem->getLeftSplicingLine()->line().x1();
                    qreal localLeftX = lineX - itemPos.x();
                    clipRect.setLeft(qMax(itemRect.left(), localLeftX));
                }
                if (pixmapItem->getRightSplicingLine())
                {
                    qreal lineX = pixmapItem->getRightSplicingLine()->line().x1();
                    qreal localRightX = lineX - itemPos.x();
                    clipRect.setRight(qMin(itemRect.right(), localRightX));
                }
            }

            // 将裁剪后的矩形转换到场景坐标系
            QRectF effectiveRect = clipRect.translated(itemPos);

            if (!hasImageItems)
            {
                boundingRect = effectiveRect;
                hasImageItems = true;
            }
            else
            {
                boundingRect = boundingRect.united(effectiveRect);
            }
        }
    }

    // 调整场景大小
    if (!boundingRect.isNull() && hasImageItems)
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

    // 应用文件排序
    tmp_filePaths = SortFilePaths(tmp_filePaths, GetFileSortTypeConfig());

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

void MainWindow::on_pushButton_deleteAll_clicked()
{
    ClearResult();
    filePaths.clear();
    images.clear();
    UpdateQListWidget();
    ui->pushButton_deleteAll->setEnabled(false);
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

void MainWindow::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasUrls())
    {
        QStringList supportedFormats = {"jpg", "jpeg", "png", "bmp", "gif", "tif", "tiff", "ico"};
        foreach (const QUrl &url, event->mimeData()->urls())
        {
            if (url.isLocalFile())
            {
                QString filePath = url.toLocalFile();
                QString suffix = QFileInfo(filePath).suffix().toLower();
                if (supportedFormats.contains(suffix))
                {
                    event->acceptProposedAction();
                    return;
                }
            }
        }
    }
    event->ignore();
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

        // 应用文件排序
        droppedFiles = SortFilePaths(droppedFiles, GetFileSortTypeConfig());

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
