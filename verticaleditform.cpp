#include "verticaleditform.h"
#include "ui_verticaleditform.h"

VerticalEditForm::VerticalEditForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VerticalEditForm)
{
    ui->setupUi(this);
}

VerticalEditForm::~VerticalEditForm()
{
    delete ui;
}
