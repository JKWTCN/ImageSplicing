#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <iostream>
#include <ShlObj.h>
#include <windows.h>
#include <Commdlg.h>
#include <QFileDialog>
#include <QStringList>
#include <opencv2/calib3d.hpp>
#include <settingwindow.h>
#include <ctime>
#include <cstdio>

using namespace std;

QStringList filePaths; // 当前打开图片存储路径
Setting setting;
SplicingState splicingState = SS_NONE;
vector<cv::Mat> images;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setting = GetSetting();
    if (setting.getSplicingType() != ST_RAW)
        setting.setPaddingType(PT_TRANSPARENT);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::UpdateQListWidget()
{
    ui->listImageFilesWidget->clear();
    ui->listImageFilesWidget->setViewMode(QListWidget::IconMode);
    //    ui->listImageFilesWidget->setIconSize(QSize(100, 100));//设置图片大小
    ui->listImageFilesWidget->setSpacing(10);                   // 间距
    ui->listImageFilesWidget->setResizeMode(QListView::Adjust); // 适应布局调整
    ui->listImageFilesWidget->setMovement(QListView::Static);   // 不能移动

    for (int i = 0; i < filePaths.length(); i++)
    {
        QListWidgetItem *imageItem = new QListWidgetItem;
        imageItem->setIcon(QIcon(filePaths[i]));
        imageItem->setText(QString::fromStdString(GetFileName(filePaths[i].toStdString())));
        imageItem->setSizeHint(QSize(120, 100));
        ui->listImageFilesWidget->addItem(imageItem);
    }
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

void MainWindow::openFilesBtnPress()
{
    // 设置文件过滤器
    QString filter = "All Image Files (*.JPG *.jpeg *.PNG *.bmp *.gif *.tif *.tiff *.ico);;"
                     "JPEG Files (*.JPG *.jpeg);;"
                     "PNG Files (*.PNG);;"
                     "Bitmap Files (*.bmp);;"
                     "GIF Files (*.gif);;"
                     "TIFF Files (*.tif *.tiff);;"
                     "Icon Files (*.ico);;"
                     "All Files (*.*)";

    // 从设置中获取上次使用的目录
    QSettings settings("iCloudWar", "ImageSplicing");
    QString lastDir = settings.value("LastOpenDirectory", QDir::homePath()).toString();

    // 打开文件对话框，允许多选
    filePaths = QFileDialog::getOpenFileNames(
        this,       // 父窗口
        "选择文件", // 对话框标题
        lastDir,    // 使用上次的目录作为初始目录
        filter      // 文件过滤器
    );

    // 如果选择了文件，保存当前目录
    if (!filePaths.isEmpty())
    {
        // 获取第一个文件的目录路径
        QString currentDir = QFileInfo(filePaths.first()).absolutePath();
        settings.setValue("LastOpenDirectory", currentDir);
    }

    if (setting.getopenReverse())
    {
        QStringList tmp = filePaths;
        for (int i = 0; i < tmp.length(); i++)
        {
            filePaths[i] = tmp[tmp.length() - i - 1];
        }
    }

    // 检查是否选择了文件
    if (filePaths.isEmpty())
    {
        return; // 用户取消了选择
    }
    ui->pushButton_verticalSplicing->setEnabled(true);
    ui->pushButton_horizontalSplicing->setEnabled(true);
    ui->pushButton_5->setEnabled(true);
    ui->pushButton_6->setEnabled(true);
    ui->pushButton_7->setEnabled(true);
    UpdateQListWidget();
}

void MainWindow::upImagePosition()
{
    if (ui->listImageFilesWidget->selectedItems().isEmpty())
        return;
    int nowIndex = ui->listImageFilesWidget->currentRow();
    if (nowIndex != 0)
        filePaths.move(nowIndex, nowIndex - 1);
    else
        filePaths.move(nowIndex, filePaths.length() - 1);
    UpdateQListWidget();
    if (nowIndex != 0)
        ui->listImageFilesWidget->setCurrentRow(nowIndex - 1);
    else
        ui->listImageFilesWidget->setCurrentRow(filePaths.length() - 1);
    ui->listImageFilesWidget->setFocus();
}

void MainWindow::downImagePosition()
{
    if (ui->listImageFilesWidget->selectedItems().isEmpty())
        return;
    int nowIndex = ui->listImageFilesWidget->currentRow();
    if (nowIndex <= filePaths.length() - 2)
        filePaths.move(nowIndex, nowIndex + 1);
    else
        filePaths.move(nowIndex, 0);
    UpdateQListWidget();
    if (nowIndex <= filePaths.length() - 2)
        ui->listImageFilesWidget->setCurrentRow(nowIndex + 1);
    else
        ui->listImageFilesWidget->setCurrentRow(0);
    ui->listImageFilesWidget->setFocus();
}

void MainWindow::deleteImagePosition()
{
    if (ui->listImageFilesWidget->selectedItems().isEmpty())
        return;
    int nowIndex = ui->listImageFilesWidget->currentRow();
    filePaths.removeAt(nowIndex);
    UpdateQListWidget();
}
void MainWindow::on_setting_action_triggered()
{
    SettingWindow *settingWindow = new SettingWindow();
    settingWindow->setWindowModality(Qt::ApplicationModal); // 设置为应用程序模态
    settingWindow->setAttribute(Qt::WA_DeleteOnClose);      // 关闭时自动删除
    settingWindow->show();
    settingWindow->activateWindow(); // 激活窗口并获取焦点
    settingWindow->raise();
}

void MainWindow::on_pushButton_horizontalSplicing_clicked()
{
    if (filePaths.length() == 0)
    {
        showInfoMessageBox("提示", "拼接图片列表为空,请打开图片。");
        return;
    }
    splicingState = SS_HORIZONTAL;

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
        switch (setting.getSplicingType())
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
            switch (setting.getSplicingType())
            {
            case ST_HIGH2LOW:
                img = ResizeByHeight(img, targetHeight, setting.getShrinkType());
                break;
            case ST_LOW2HIGH:
                img = ResizeByHeight(img, targetHeight, setting.getNarrowType());
                break;
            case ST_RAW:
                break;
            }
        }
    }
    cv::Mat result;
    try
    {
        cv::hconcat(images, result);
        // 将 OpenCV 的 BGR 格式转换为 RGB 格式
        cv::cvtColor(result, result, cv::COLOR_BGR2RGB);
        // 创建 QImage 并从 cv::Mat 复制数据
        QImage qimg(result.data, result.cols, result.rows, result.step, QImage::Format_RGB888);
        // 将 QImage 转换为 QPixmap
        QPixmap pixmap = QPixmap::fromImage(qimg);
        QGraphicsScene *scene = new QGraphicsScene;
        scene->addPixmap(pixmap); // 传入转换后的 pixmap
        ui->graphicsView_result->setScene(scene);
        ui->pushButton_auto->setEnabled(true);
        ui->pushButton_manual->setEnabled(true);
        ui->pushButton_save->setEnabled(true);
    }
    catch (cv::Exception &e)
    {
        std::cerr << "OpenCV异常: " << e.what() << std::endl;
        showErrorMessageBox("错误", e.what());
    }
}

void MainWindow::on_pushButton_verticalSplicing_clicked()
{
    if (filePaths.length() == 0)
    {
        showInfoMessageBox("提示", "拼接图片列表为空,请打开图片。");
        return;
    }
    splicingState = SS_VERTICAL;
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
    int targetWidth = images[0].cols;
    for (const auto &img : images)
    {
        switch (setting.getSplicingType())
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
            switch (setting.getSplicingType())
            {
            case ST_HIGH2LOW:
                img = ResizeByWidth(img, targetWidth, setting.getShrinkType());
                break;
            case ST_LOW2HIGH:
                img = ResizeByWidth(img, targetWidth, setting.getShrinkType());
                break;
            case ST_RAW:
                break;
            }
        }
    }
    cv::Mat result;
    try
    {
        cv::vconcat(images, result);
        // 将 OpenCV 的 BGR 格式转换为 RGB 格式
        cv::cvtColor(result, result, cv::COLOR_BGR2RGB);
        // 创建 QImage 并从 cv::Mat 复制数据
        QImage qimg(result.data, result.cols, result.rows, result.step, QImage::Format_RGB888);
        // 将 QImage 转换为 QPixmap
        QPixmap pixmap = QPixmap::fromImage(qimg);
        QGraphicsScene *scene = new QGraphicsScene;
        scene->addPixmap(pixmap); // 传入转换后的 pixmap
        ui->graphicsView_result->setScene(scene);
        ui->pushButton_auto->setEnabled(true);
        ui->pushButton_manual->setEnabled(true);
        ui->pushButton_save->setEnabled(true);
    }
    catch (cv::Exception &e)
    {
        std::cerr << "OpenCV异常: " << e.what() << std::endl;
        showErrorMessageBox("错误", e.what());
    }
}
void MainWindow::on_pushButton_auto_clicked()
{
    switch (splicingState)
    {
    case SS_VERTICAL:
        splicingState = SS_AUTO_VERTICAL;
        break;
    case SS_HORIZONTAL:
        splicingState = SS_AUTO_HORIZONTAL;
        break;
    }

    cv::Mat result;
    if (!AutoStitchImages(images, result))
    {
        showNoticeMessageBox("提示", "图像自动调整失败，请检查图像是否适合拼接。");
        switch (splicingState)
        {
        case SS_AUTO_VERTICAL:
            splicingState = SS_VERTICAL;
            break;
        case SS_AUTO_HORIZONTAL:
            splicingState = SS_HORIZONTAL;
            break;
        }
        return;
    }
    try
    {
        cv::vconcat(images, result);
        // 将 OpenCV 的 BGR 格式转换为 RGB 格式
        cv::cvtColor(result, result, cv::COLOR_BGR2RGB);
        // 创建 QImage 并从 cv::Mat 复制数据
        QImage qimg(result.data, result.cols, result.rows, result.step, QImage::Format_RGB888);
        // 将 QImage 转换为 QPixmap
        QPixmap pixmap = QPixmap::fromImage(qimg);
        QGraphicsScene *scene = new QGraphicsScene;
        scene->addPixmap(pixmap); // 传入转换后的 pixmap
        ui->graphicsView_result->setScene(scene);
        ui->pushButton_auto->setEnabled(true);
        ui->pushButton_manual->setEnabled(true);
        ui->pushButton_save->setEnabled(true);
    }
    catch (cv::Exception &e)
    {
        std::cerr << "OpenCV异常: " << e.what() << std::endl;
        showErrorMessageBox("错误", e.what());
    }
}

void MainWindow::wheelEvent(QWheelEvent *event)
{
    // 检查是否按下 Ctrl 键
    if (QApplication::keyboardModifiers() & Qt::ControlModifier)
    {
        // 获取滚轮滚动角度（通常 1 步 = 120 度）
        int delta = event->angleDelta().y();

        if (delta > 0)
        {
            ui->graphicsView_result->scale(1.1, 1.1); // 放大10%
        }
        else if (delta < 0)
        {
            ui->graphicsView_result->scale(0.9, 0.9); // 缩小10%
        }

        event->accept(); // 表示事件已处理
    }
    else
    {
        // 其他情况（如普通滚轮滚动）交给父类处理
        QWidget::wheelEvent(event);
    }
}

void MainWindow::on_pushButton_save_clicked()
{
    time_t now = time(0);
    QSettings settings("iCloudWar", "ImageSplicing");
    QString lastDir = settings.value("LastOpenDirectory", "ImageSplicing_" + QDir::homePath()).toString();

    switch (setting.getSaveType())
    {
    case ST_PNG:
        saveGraphicsViewToImage(ui->graphicsView_result, "ImageSplicing_" + QString::number(now) + ".png", PaddingType2QtColor(setting.getPaddingType()));
        break;
    case ST_JPG:
        saveGraphicsViewToImage(ui->graphicsView_result, "ImageSplicing_" + QString::number(now) + ".jpg", PaddingType2QtColor(setting.getPaddingType()));
        break;
    default:
        saveGraphicsViewToImage(ui->graphicsView_result, "ImageSplicing_" + QString::number(now) + ".png", PaddingType2QtColor(setting.getPaddingType()));
        break;
    }
    if (setting.getfinishRAWPhicture())
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
    ui->pushButton_verticalSplicing->setEnabled(false);
    ui->pushButton_horizontalSplicing->setEnabled(false);
    ui->pushButton_5->setEnabled(false);
    ui->pushButton_6->setEnabled(false);
    ui->pushButton_7->setEnabled(false);
    ui->pushButton_auto->setEnabled(false);
    ui->pushButton_manual->setEnabled(false);
    ui->pushButton_save->setEnabled(false);
    splicingState = SS_NONE;
    UpdateQListWidget();
}
