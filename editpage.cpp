#include "editpage.h"
#include "ui_editpage.h"

EditPage::EditPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EditPage)
{
    ui->setupUi(this);
}

EditPage::~EditPage()
{
    delete ui;
}
