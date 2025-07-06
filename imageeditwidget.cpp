#include "imageeditwidget.h"
#include "ui_imageeditwidget.h"

ImageEditWidget::ImageEditWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ImageEditWidget)
{
    ui->setupUi(this);
}

ImageEditWidget::~ImageEditWidget()
{
    delete ui;
}
