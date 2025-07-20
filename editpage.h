#ifndef EDITPAGE_H
#define EDITPAGE_H

#include <QWidget>

namespace Ui {
class EditPage;
}

class EditPage : public QWidget
{
    Q_OBJECT

public:
    explicit EditPage(QWidget *parent = nullptr);
    ~EditPage();

private:
    Ui::EditPage *ui;
};

#endif // EDITPAGE_H
