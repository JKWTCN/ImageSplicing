#include <QGraphicsPixmapItem>
#include <QPainter>
class HorizontalMovablePixmapItem : public QGraphicsPixmapItem
{
public:
    HorizontalMovablePixmapItem(const QPixmap &pixmap, QGraphicsItem *parent = nullptr);

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
};
