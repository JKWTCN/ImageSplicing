#include "HorizontalMovablePixmapItem.h"

HorizontalMovablePixmapItem::HorizontalMovablePixmapItem(const QPixmap &pixmap, QGraphicsItem *parent)
    : QGraphicsPixmapItem(pixmap, parent)
{
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
}

QVariant HorizontalMovablePixmapItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionChange && scene())
    {
        // 只允许水平移动，保持Y坐标不变
        QPointF newPos = value.toPointF();
        newPos.setY(pos().y()); // 保持原始Y坐标不变
        return newPos;
    }
    return QGraphicsPixmapItem::itemChange(change, value);
}

void HorizontalMovablePixmapItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->save();
    painter->setClipRect(QRectF(50, 50, 100, 100)); // 只绘制指定区域
    QGraphicsPixmapItem::paint(painter, option, widget);
    painter->restore();
}
