#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
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
private slots:
    void openFilesBtnPress();
    void upImagePosition();
    void downImagePosition();
    void deleteImagePosition();
    void on_setting_action_triggered();
    void autoSplicing();
    void pushButton_verticalSplicing();
    void pushButton_horizontalSplicing();
};
#endif // MAINWINDOW_H
