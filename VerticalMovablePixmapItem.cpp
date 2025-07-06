#include "VerticalMovablePixmapItem.h"

VerticalMovablePixmapItem::VerticalMovablePixmapItem(const QPixmap &pixmap, QGraphicsItem *parent)
    : QGraphicsPixmapItem(pixmap, parent)
{
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
}

QVariant VerticalMovablePixmapItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionChange && scene())
    {
        // 只允许垂直移动，保持X坐标不变
        QPointF newPos = value.toPointF();
        newPos.setX(pos().x()); // 保持原始X坐标不变
        return newPos;
    }
    return QGraphicsPixmapItem::itemChange(change, value);
}
