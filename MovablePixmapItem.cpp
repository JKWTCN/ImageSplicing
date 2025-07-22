#include "MovablePixmapItem.h"
#include "SplicingLine.h"

MovablePixmapItem::MovablePixmapItem(const QPixmap &pixmap, Move_Type moveType, QGraphicsItem *parent)
    : QGraphicsPixmapItem(pixmap, parent)
{
    setFlag(QGraphicsItem::ItemIsMovable,false);

    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
    this->moveType = moveType;
}

QVariant MovablePixmapItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionChange && scene() &&getMoveType() == MV_V)
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
    else if (change == ItemPositionChange && scene() && getMoveType() == MV_H)
    {
        // 只允许水平移动，保持Y坐标不变
        QPointF newPos = value.toPointF();
        QPointF oldPos = pos();
        qreal deltaX = newPos.x() - oldPos.x();

        // 左移，更新右侧拼接线位置
        if (deltaX < 0 && rightSplicingLine)
        {
            QLineF currentLine = rightSplicingLine->line();
            rightSplicingLine->setLine(currentLine.x1() + deltaX, currentLine.y1(),
                                       currentLine.x2() + deltaX, currentLine.y2());
        }
        // 右移，更新左侧拼接线位置
        else if (deltaX > 0 && leftSplicingLine)
        {
            QLineF currentLine = leftSplicingLine->line();
            leftSplicingLine->setLine(currentLine.x1() + deltaX, currentLine.y1(),
                                      currentLine.x2() + deltaX, currentLine.y2());
        }

        newPos.setY(pos().y()); // 保持原始Y坐标不变
        return newPos;
    }
    return QGraphicsPixmapItem::itemChange(change, value);
}
void MovablePixmapItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->save();

    // 计算裁剪区域：只显示拼接线之间的部分
    QRectF itemRect = this->boundingRect();
    QPointF itemPos = this->pos();

    // 创建裁剪矩形
    QRectF clipRect = itemRect;
    if (moveType == MV_V)
    {
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
    }
    else if (moveType == MV_H)
    {
        // 如果有左侧拼接线，限制左边界
        if (leftSplicingLine)
        {
            qreal lineX = leftSplicingLine->line().x1();
            qreal localLeftX = lineX - itemPos.x();
            clipRect.setLeft(qMax(itemRect.left(), localLeftX));
        }
        // 如果有右侧拼接线，限制右边界
        if (rightSplicingLine)
        {
            qreal lineX = rightSplicingLine->line().x1();
            qreal localRightX = lineX - itemPos.x();
            clipRect.setRight(qMin(itemRect.right(), localRightX));
        }
    }
    // 应用裁剪
    painter->setClipRect(clipRect);

    QGraphicsPixmapItem::paint(painter, option, widget);
    painter->restore();
};
