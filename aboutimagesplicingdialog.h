#ifndef ABOUTIMAGESPLICINGDIALOG_H
#define ABOUTIMAGESPLICINGDIALOG_H

#include <QDialog>

namespace Ui {
class AboutImageSplicingDialog;
}

class AboutImageSplicingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AboutImageSplicingDialog(QWidget *parent = nullptr);
    ~AboutImageSplicingDialog();

private:
    Ui::AboutImageSplicingDialog *ui;
};

#endif // ABOUTIMAGESPLICINGDIALOG_H
