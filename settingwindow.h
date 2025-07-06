#ifndef SETTINGWINDOW_H
#define SETTINGWINDOW_H

#include <QDialog>
#include <QListView.h>
namespace Ui {
class SettingWindow;
}

class SettingWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SettingWindow(QWidget *parent = nullptr);
    ~SettingWindow();

private slots:
    void on_comboBox_splicingType_currentIndexChanged(int index);

    void on_comboBox_saveType_currentIndexChanged(int index);

private:
    Ui::SettingWindow *ui;
};

#endif // SETTINGWINDOW_H
