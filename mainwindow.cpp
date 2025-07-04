#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <iostream>
#include <ShlObj.h>
#include <windows.h>
#include <Commdlg.h>
#include <QFileDialog>
#include <QStringList>
#include <tools.h>
#include <opencv2/calib3d.hpp>
#include <settingwindow.h>
using namespace std;

QStringList filePaths; // 当前打开图片存储路径
Setting setting;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setting = tools::GetSetting();
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
        imageItem->setText(QString::fromStdString(tools::GetFileName(filePaths[i].toStdString())));
        imageItem->setSizeHint(QSize(120, 100));
        ui->listImageFilesWidget->addItem(imageItem);
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

    // 打开文件对话框，允许多选
    filePaths = QFileDialog::getOpenFileNames(
                this,       // 父窗口
                "选择文件", // 对话框标题
                "",         // 初始目录
                filter      // 文件过滤器
                );
    if (setting.getopenReverse()){
        QStringList tmp=filePaths;
        for(int i=0;i<tmp.length();i++){
            filePaths[i]=tmp[tmp.length()-i-1];
        }
    }
    // 检查是否选择了文件
    if (filePaths.isEmpty())
    {
        return; // 用户取消了选择
    }
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
        filePaths.move(nowIndex, filePaths.length()-1);
    UpdateQListWidget();
    if (nowIndex != 0)
        ui->listImageFilesWidget->setCurrentRow(nowIndex - 1);
    else
        ui->listImageFilesWidget->setCurrentRow(filePaths.length()-1);
    ui->listImageFilesWidget->setFocus();
}

void MainWindow::downImagePosition()
{
    if (ui->listImageFilesWidget->selectedItems().isEmpty())
        return;
    int nowIndex = ui->listImageFilesWidget->currentRow();
    if (nowIndex <= filePaths.length()-2)
        filePaths.move(nowIndex, nowIndex + 1);
    else
        filePaths.move(nowIndex, 0);
    UpdateQListWidget();
    if (nowIndex <= filePaths.length()-2)
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
void MainWindow::autoSplicing(){

}

void MainWindow::pushButton_verticalSplicing(){
    if(setting.getPaddingType()==)
}

void MainWindow::pushButton_horizontalSplicing(){

}
