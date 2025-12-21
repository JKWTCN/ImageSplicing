#include "aboutdialog.h"
#include "ui_aboutdialog.h"
#include "version.h"

AboutDialog::AboutDialog(QWidget *parent) : QDialog(parent),
                                            ui(new Ui::AboutDialog)
{
    ui->setupUi(this);

    // 设置动态版本号
    ui->versionLabel->setText(QString("版本 %1.%2.%3").arg(APP_VERSION_MAJOR).arg(APP_VERSION_MINOR).arg(APP_VERSION_PATCH));
}

AboutDialog::~AboutDialog()
{
    delete ui;
}
