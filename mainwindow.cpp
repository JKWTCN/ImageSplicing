#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <iostream>
#include <ShlObj.h>
#include <windows.h>
#include <Commdlg.h>
#include <QFileDialog>
#include <QStringList>
#include <tools.h>
using namespace std;

QStringList filePaths ;//当前打开图片存储路径

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
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
    ui->listImageFilesWidget->setSpacing(10);//间距
    ui->listImageFilesWidget->setResizeMode(QListView::Adjust);//适应布局调整
    ui->listImageFilesWidget->setMovement(QListView::Static);//不能移动
    if( ui->checkBoxOpenReverse->checkState()){
        for(int i=filePaths.length()-1;i>=0;i--){
            QListWidgetItem *imageItem = new QListWidgetItem;
            imageItem->setIcon(QIcon(filePaths[i]));
            imageItem->setText(QString::fromStdString(tools::GetFileName(filePaths[i].toStdString())));
            imageItem->setSizeHint(QSize(120, 100));
            ui->listImageFilesWidget->addItem(imageItem);
        }
    }
    else
        for(int i=0;i<filePaths.length();i++){
            QListWidgetItem *imageItem = new QListWidgetItem;
            imageItem->setIcon(QIcon(filePaths[i]));
            imageItem->setText(QString::fromStdString(tools::GetFileName(filePaths[i].toStdString())));
            imageItem->setSizeHint(QSize(120, 100));
            ui->listImageFilesWidget->addItem(imageItem);
        }
}



void MainWindow::openFilesBtnPress() {
    // 设置文件过滤器
    QString filter = "All Image Files (*.jpg *.jpeg *.png *.bmp *.gif *.tif *.tiff *.ico);;"
                     "JPEG Files (*.jpg *.jpeg);;"
                     "PNG Files (*.png);;"
                     "Bitmap Files (*.bmp);;"
                     "GIF Files (*.gif);;"
                     "TIFF Files (*.tif *.tiff);;"
                     "Icon Files (*.ico);;"
                     "All Files (*.*)";

    // 打开文件对话框，允许多选
    filePaths = QFileDialog::getOpenFileNames(
                this,                               // 父窗口
                "选择文件", // 对话框标题
                "",                               // 初始目录
                filter                              // 文件过滤器
                );

    // 检查是否选择了文件
    if (filePaths.isEmpty()) {
        return; // 用户取消了选择
    }
    UpdateQListWidget();



}

void MainWindow::upImagePosition()
{
    if(ui->listImageFilesWidget->selectedItems().isEmpty())
        return;
    int nowIndex=ui->listImageFilesWidget->currentRow();
    if(nowIndex!=0)
        filePaths.move(nowIndex,nowIndex-1);
    else
        filePaths.move(nowIndex,filePaths.length());
    UpdateQListWidget();
    if(nowIndex!=0)
        ui->listImageFilesWidget->setCurrentRow (nowIndex-1);
    else
        ui->listImageFilesWidget->setCurrentRow (filePaths.length());
    ui->listImageFilesWidget->setFocus();
}

void MainWindow::downImagePosition()
{
    if(ui->listImageFilesWidget->selectedItems().isEmpty())
        return;
    int nowIndex=ui->listImageFilesWidget->currentRow();
    if(nowIndex!=filePaths.length())
        filePaths.move(nowIndex,nowIndex+1);
    else
        filePaths.move(nowIndex,0);
    UpdateQListWidget();
    if(nowIndex!=filePaths.length())
        ui->listImageFilesWidget->setCurrentRow (nowIndex+1);
    else
        ui->listImageFilesWidget->setCurrentRow (0);
    ui->listImageFilesWidget->setFocus();
}

void MainWindow::deleteImagePosition()
{
    if(ui->listImageFilesWidget->selectedItems().isEmpty())
        return;
    int nowIndex=ui->listImageFilesWidget->currentRow();
    filePaths.removeAt(nowIndex);
    UpdateQListWidget();
}
