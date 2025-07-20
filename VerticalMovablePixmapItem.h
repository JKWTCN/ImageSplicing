#include <QGraphicsPixmapItem>
#include <QPainter>
#include <QGraphicsScene>

class VerticalMovablePixmapItem : public QGraphicsPixmapItem
{
public:
    VerticalMovablePixmapItem(const QPixmap &pixmap, QGraphicsItem *parent = nullptr);

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
};
