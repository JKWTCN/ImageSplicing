#include "HorizontalMovablePixmapItem.h"

//绘制区域
QRectF area;

HorizontalMovablePixmapItem::HorizontalMovablePixmapItem(const QPixmap &pixmap, QGraphicsItem *parent)
    : QGraphicsPixmapItem(pixmap, parent)
{
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);


    //    QRectF rect = this->boundingRect();
    // 获取四个角的本地坐标
    //    QPointF topLeftLocal = rect.topLeft();
    //    QPointF topRightLocal = rect.topRight();
    //    QPointF bottomLeftLocal = rect.bottomLeft();
    //        QPointF bottomRightLocal = rect.bottomRight();

    // 转换为场景坐标
    //    QPointF topLeftScene = this->mapToScene(topLeftLocal);
    //    QPointF topRightScene = this->mapToScene(topRightLocal);
    //    QPointF bottomLeftScene = this->mapToScene(bottomLeftLocal);
    //    QPointF bottomRightScene = this->mapToScene(bottomRightLocal);
    area=this->boundingRect();
}

QVariant HorizontalMovablePixmapItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionChange && scene())
    {
        // 只允许水平移动，保持Y坐标不变
        QPointF newPos = value.toPointF();
        newPos.setY(pos().y()); // 保持原始Y坐标不变
        area=this->boundingRect();
        return newPos;
    }
    return QGraphicsPixmapItem::itemChange(change, value);
}

void HorizontalMovablePixmapItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->save();
    //    painter->setClipRect(area); // 只绘制指定区域
    QGraphicsPixmapItem::paint(painter, option, widget);
    painter->restore();
}
