#include "VerticalMovablePixmapItem.h"
#include <QDebug>
// 上拼接线
qreal lineTopY;
// 下拼接线
qreal lineBottomY;
VerticalMovablePixmapItem::VerticalMovablePixmapItem(const QPixmap &pixmap, QGraphicsItem *parent)
    : QGraphicsPixmapItem(pixmap, parent)
{
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
    QRectF itemRect = this->boundingRect();
    QPolygonF scenePolygon = this->mapToScene(itemRect);
    lineTopY = scenePolygon.boundingRect().top();       // 上边
    lineBottomY = scenePolygon.boundingRect().bottom(); // 下边
}

QVariant VerticalMovablePixmapItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionChange && scene())
    {
        // 只允许垂直移动，保持X坐标不变
        QPointF newPos = value.toPointF();
        // 上移，下边界线跟随
        if (newPos.y() < pos().y())
        {
            qDebug() <<"上移前" <<lineTopY<<lineBottomY;
            lineBottomY -= pos().y()-newPos.y() ;
            qDebug() <<"上移后" <<lineTopY<<lineBottomY;
        }
        // 下移，上边界线跟随
        else if (newPos.y() > pos().y())
        {
            qDebug() <<"下移前" <<lineTopY<<lineBottomY;
            lineTopY += newPos.y()-pos().y() ;
            qDebug() <<"下移后" <<lineTopY<<lineBottomY;
        }
        newPos.setX(pos().x()); // 保持原始X坐标不变

        update();

        return newPos;
    }
    return QGraphicsPixmapItem::itemChange(change, value);
}
void VerticalMovablePixmapItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->save();
    //    painter->setClipRect(area); // 只绘制指定区域
    painter->setPen(QPen(Qt::blue, 1));
    painter->drawLine(QPointF(-1000, lineTopY), QPointF(1000, lineTopY)); // 绘制上拼接线
    painter->setPen(QPen(Qt::red, 1));
    painter->drawLine(QPointF(-1000, lineBottomY), QPointF(1000, lineBottomY)); // 绘制下拼接线
    QGraphicsPixmapItem::paint(painter, option, widget);
    painter->restore();
}
