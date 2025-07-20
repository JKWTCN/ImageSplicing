#include "VerticalMovablePixmapItem.h"
//上拼接线
qreal lineTopY;
//下拼接线
qreal lineBottomY;
VerticalMovablePixmapItem::VerticalMovablePixmapItem(const QPixmap &pixmap, QGraphicsItem *parent)
    : QGraphicsPixmapItem(pixmap, parent)
{
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
    QRectF itemRect = this->boundingRect();
    QPolygonF scenePolygon = this->mapToScene(itemRect);
    lineTopY = scenePolygon.boundingRect().top();  // 上边
    lineBottomY = scenePolygon.boundingRect().bottom();  // 下边
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
