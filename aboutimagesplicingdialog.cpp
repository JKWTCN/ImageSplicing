#include "aboutimagesplicingdialog.h"
#include "ui_aboutimagesplicingdialog.h"

AboutImageSplicingDialog::AboutImageSplicingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutImageSplicingDialog)
{
    ui->setupUi(this);
}

AboutImageSplicingDialog::~AboutImageSplicingDialog()
{
    delete ui;
}
