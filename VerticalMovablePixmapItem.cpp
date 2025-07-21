#include "VerticalMovablePixmapItem.h"
#include "SplicingLine.h"
#include <QDebug>

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
        QPointF oldPos = pos();
        qreal deltaY = newPos.y() - oldPos.y();

        // 上移，更新下方拼接线位置
        if (deltaY < 0 && bottomSplicingLine)
        {
            QLineF currentLine = bottomSplicingLine->line();
            bottomSplicingLine->setLine(currentLine.x1(), currentLine.y1() + deltaY,
                                        currentLine.x2(), currentLine.y2() + deltaY);
        }
        // 下移，更新上方拼接线位置
        else if (deltaY > 0 && topSplicingLine)
        {
            QLineF currentLine = topSplicingLine->line();
            topSplicingLine->setLine(currentLine.x1(), currentLine.y1() + deltaY,
                                     currentLine.x2(), currentLine.y2() + deltaY);
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

    // 计算裁剪区域：只显示拼接线之间的部分
    QRectF itemRect = this->boundingRect();
    QPointF itemPos = this->pos();

    // 创建裁剪矩形
    QRectF clipRect = itemRect;

    // 如果有上方拼接线，限制顶部边界
    if (topSplicingLine)
    {
        qreal lineY = topSplicingLine->line().y1();
        qreal localTopY = lineY - itemPos.y();
        clipRect.setTop(qMax(itemRect.top(), localTopY));
    }

    // 如果有下方拼接线，限制底部边界
    if (bottomSplicingLine)
    {
        qreal lineY = bottomSplicingLine->line().y1();
        qreal localBottomY = lineY - itemPos.y();
        clipRect.setBottom(qMin(itemRect.bottom(), localBottomY));
    }

    // 应用裁剪
    painter->setClipRect(clipRect);

    QGraphicsPixmapItem::paint(painter, option, widget);
    painter->restore();
}
