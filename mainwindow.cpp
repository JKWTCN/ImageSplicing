#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <iostream>
#include <ShlObj.h>
#include <windows.h>
#include <Commdlg.h>



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

void MainWindow::openFilesBtnPress(){
    char szFilePath[1024] = {0};   // 所选择的文件最终的路径
    OPENFILENAME ofn= {};
    ofn.lStructSize = sizeof(ofn);
    //    ofn.hwndOwner = m_hWnd;
    ofn.lpstrFilter = "png(*.png)|jpg(*.jpg)";
    ofn.lpstrInitialDir = "./";//默认的文件路径
    ofn.lpstrFile = szFilePath;//存放文件的缓冲区
    ofn.nMaxFile = sizeof(szFilePath)/sizeof(*szFilePath);
    ofn.nFilterIndex = 0;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_EXPLORER|OFN_ALLOWMULTISELECT ;
    if (!GetOpenFileName(&ofn))
    {
        return ;
    }
    if (strcmp(szFilePath, "") == 0)
    {
        // 检验是否获取成功
        return ;
    }

}
