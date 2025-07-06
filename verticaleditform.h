#ifndef VERTICALEDITFORM_H
#define VERTICALEDITFORM_H

#include <QWidget>

namespace Ui {
class VerticalEditForm;
}

class VerticalEditForm : public QWidget
{
    Q_OBJECT

public:
    explicit VerticalEditForm(QWidget *parent = nullptr);
    ~VerticalEditForm();

private:
    Ui::VerticalEditForm *ui;
};

#endif // VERTICALEDITFORM_H
