#ifndef IMAGEEDITWIDGET_H
#define IMAGEEDITWIDGET_H

#include <QWidget>

namespace Ui {
class ImageEditWidget;
}

class ImageEditWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ImageEditWidget(QWidget *parent = nullptr);
    ~ImageEditWidget();

private:
    Ui::ImageEditWidget *ui;
};

#endif // IMAGEEDITWIDGET_H
