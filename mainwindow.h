#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <opencv2/opencv.hpp>
#include <QSettings>
#include <imagepro.hpp>
#include <QWheelEvent>
#include <QApplication>

QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    void UpdateQListWidget();
    void ClearDisplay();
    void ClearResult();
    void UnlockPostionButton();
    void LockPostionButton();
    void ReFreshResultWidget();
private slots:
    void openFilesBtnPress();
    void upImagePosition();
    void downImagePosition();
    void deleteImagePosition();
    void on_setting_action_triggered();
    void on_pushButton_horizontalSplicing_clicked();
    void on_pushButton_verticalSplicing_clicked();
    void on_pushButton_save_clicked();
    void on_pushButton_auto_clicked();

    void on_pushButton_LayerUp_clicked();

    void on_pushButton_LayerDown_clicked();

    void on_pushButton_copy_clicked();

    void on_pushButton_add_clicked();

    void on_pushButton_editSelect_clicked();

    void on_pushButton_editResult_clicked();

protected:
    void wheelEvent(QWheelEvent *event) override; // 重写滚轮事件
};
#endif // MAINWINDOW_H
