#include "MovablePixmapItem.h"
#include "SplicingLine.h"
#include <QGraphicsScene>
#include <QDebug>
MovablePixmapItem::MovablePixmapItem(const QPixmap &pixmap, Move_Type moveType, QGraphicsItem *parent)
    : QGraphicsPixmapItem(pixmap, parent)
{
    setFlag(QGraphicsItem::ItemIsMovable, false);

    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
    this->moveType = moveType;
}

QVariant MovablePixmapItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionChange && scene() && getMoveType() == MV_V)
    {
        // 只允许垂直移动，保持X坐标不变
        QPointF newPos = value.toPointF();
        QPointF oldPos = pos();
        qreal deltaY = newPos.y() - oldPos.y();
        newPos.setX(pos().x()); // 保持原始X坐标不变

        qreal topY = sceneBoundingRect().top();
        qreal bottomY = sceneBoundingRect().bottom();
        // 上移，更新下方所有元素位置
        if (deltaY < 0)
        {
            if (topSplicingLine && !bottomSplicingLine)
            {
                qDebug() << "最下面图片上移" << endl;
                qreal lineTY = topSplicingLine->line().y1();
                if (!(lineTY <= bottomY + deltaY))
                {
                    newPos.setY(oldPos.y());
                    return newPos;
                }
            }
            else if (bottomSplicingLine && !topSplicingLine)
            {
                qDebug() << "最上面图片上移" << endl;
                qreal lineBY = bottomSplicingLine->line().y1();
                if (!(bottomY + deltaY >= lineBY))
                {
                    newPos.setY(oldPos.y());
                    return newPos;
                }
            }
            else if (topSplicingLine && bottomSplicingLine)
            {
                qDebug() << "中间图片上移" << endl;
                // 如果是该元素的上拼接线
                if (topSplicingLine->isHighlighted())
                {
                    // 获取当前元素的底部Y坐标
                    qreal currentBottomY = sceneBoundingRect().bottom();

                    if (currentBottomY + deltaY <= topSplicingLine->line().y1())
                    {
                        return oldPos;
                    }

                    // 移动当前元素下方的所有元素作为整体
                    moveElementsBelow(currentBottomY, deltaY);
                }
                // 如果是该元素的下拼接线
                else if (bottomSplicingLine->isHighlighted())
                {
                    // 获取当前元素的底部Y坐标
                    qreal currentBottomY = sceneBoundingRect().bottom();
                    // 如果下拼接线在当前元素底部之下
                    if (bottomSplicingLine->line().y1() >= currentBottomY + deltaY)
                    {
                        return oldPos;
                    }

                    if (currentBottomY + deltaY <= topSplicingLine->line().y1())
                    {
                        return oldPos;
                    }

                    // 移动当前元素下方的所有元素作为整体
                    moveElementsBelow(currentBottomY, deltaY);
                }
            }
        }
        // 下移，更新上方所有元素位置
        else if (deltaY > 0)
        {
            if (topSplicingLine && !bottomSplicingLine)
            {
                qDebug() << "最下面图片下移" << endl;
                qreal lineTY = topSplicingLine->line().y1();
                if (!(topY + deltaY <= lineTY))
                {
                    newPos.setY(oldPos.y());
                    return newPos;
                }
            }
            else if (bottomSplicingLine && !topSplicingLine)
            {
                qDebug() << "最上面图片下移" << endl;
                qreal lineBY = bottomSplicingLine->line().y1();
                if (!(topY + deltaY <= lineBY))
                {
                    newPos.setY(oldPos.y());
                    return newPos;
                }
            }
            else if (topSplicingLine && bottomSplicingLine)
            {
                qDebug() << "中间图片下移" << endl;
                if (topSplicingLine->isHighlighted())
                {
                    qreal currentTopY = sceneBoundingRect().top();
                    if (currentTopY + deltaY >= topSplicingLine->line().y1())
                    {
                        return oldPos;
                    }

                    // 获取当前元素的底部Y坐标
                    qreal currentBottomY = sceneBoundingRect().bottom();

                    // 移动当前元素下方的所有元素作为整体
                    moveElementsBelow(currentBottomY, deltaY);
                }
                else if (bottomSplicingLine->isHighlighted())
                {
                    qreal currentTopY = sceneBoundingRect().top();
                    if (currentTopY + deltaY >= topSplicingLine->line().y1())
                    {
                        return oldPos;
                    }

                    // 获取当前元素的底部Y坐标
                    qreal currentBottomY = sceneBoundingRect().bottom();

                    // 移动当前元素下方的所有元素作为整体
                    moveElementsBelow(currentBottomY, deltaY);
                }
            }
        }
        return newPos;
    }
    else if (change == ItemPositionChange && scene() && getMoveType() == MV_H)
    {
        // 只允许水平移动，保持Y坐标不变
        QPointF newPos = value.toPointF();
        QPointF oldPos = pos();
        qreal deltaX = newPos.x() - oldPos.x();
        newPos.setY(pos().y()); // 保持原始Y坐标不变

        qreal rightX = sceneBoundingRect().right();
        qreal leftX = sceneBoundingRect().left();

        // 左移
        if (deltaX < 0)
        {
            if (rightSplicingLine && leftSplicingLine)
            {
                // qreal lineRX = rightSplicingLine->line().x1();
                // qreal lineLX = leftSplicingLine->line().x1();
            }
            else if (!rightSplicingLine && leftSplicingLine)
            {
                qreal lineLX = leftSplicingLine->line().x1();
                qDebug() << "最右边图片左移" << rightX << "  " << lineLX << endl;
                if (!(rightX + deltaX >= lineLX))
                {
                    newPos.setX(oldPos.x());
                    return newPos;
                }
            }
            else if (rightSplicingLine && !leftSplicingLine)
            {
                qreal lineRX = rightSplicingLine->line().x1();
                qDebug() << "最左边图片左移" << rightX << "  " << lineRX << endl;
                if (!(rightX + deltaX >= lineRX))
                {
                    newPos.setX(oldPos.x());
                    return newPos;
                }
            }
        }
        // 右移
        else if (deltaX > 0)
        {
            if (rightSplicingLine && leftSplicingLine)
            {
                // qreal lineRX = rightSplicingLine->line().x1();
                // qreal lineLX = leftSplicingLine->line().x1();
            }
            else if (!rightSplicingLine && leftSplicingLine)
            {
                qreal lineLX = leftSplicingLine->line().x1();
                qDebug() << "最右边图片右移" << rightX << "  " << lineLX << endl;
                if (!(leftX + deltaX <= lineLX))
                {
                    newPos.setX(oldPos.x());
                    return newPos;
                }
            }
            else if (rightSplicingLine && !leftSplicingLine)
            {
                qreal lineRX = rightSplicingLine->line().x1();
                qDebug() << "最左边图片右移" << rightX << "  " << lineRX << endl;
                if (!(rightX + deltaX >= lineRX))
                {
                    newPos.setX(oldPos.x());
                    return newPos;
                }
            }
        }
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
}

QPainterPath MovablePixmapItem::shape() const
{
    QPainterPath path;

    // 计算可点击区域：只有拼接线之间的部分
    QRectF itemRect = this->boundingRect();
    QRectF clickableRect = itemRect;

    if (moveType == MV_V)
    {
        // 垂直移动模式：限制上下边界
        if (topSplicingLine)
        {
            qreal lineY = topSplicingLine->line().y1();
            qreal localTopY = lineY - this->pos().y();
            clickableRect.setTop(qMax(itemRect.top(), localTopY));
        }

        if (bottomSplicingLine)
        {
            qreal lineY = bottomSplicingLine->line().y1();
            qreal localBottomY = lineY - this->pos().y();
            clickableRect.setBottom(qMin(itemRect.bottom(), localBottomY));
        }
    }
    else if (moveType == MV_H)
    {
        // 水平移动模式：限制左右边界
        if (leftSplicingLine)
        {
            qreal lineX = leftSplicingLine->line().x1();
            qreal localLeftX = lineX - this->pos().x();
            clickableRect.setLeft(qMax(itemRect.left(), localLeftX));
        }

        if (rightSplicingLine)
        {
            qreal lineX = rightSplicingLine->line().x1();
            qreal localRightX = lineX - this->pos().x();
            clickableRect.setRight(qMin(itemRect.right(), localRightX));
        }
    }

    // 确保可点击区域有效
    if (clickableRect.isValid() && !clickableRect.isEmpty())
    {
        path.addRect(clickableRect);
    }
    else
    {
        // 如果计算出的区域无效，返回空路径（不可点击）
        return QPainterPath();
    }

    return path;
};

// 整体移动currentTopY以上的
void MovablePixmapItem::moveElementsAbove(qreal currentTopY, qreal deltaY)
{
    // 遍历场景中的所有项目，找到位于当前元素上方的元素
    QList<QGraphicsItem *> allItems = this->scene()->items();

    for (QGraphicsItem *item : allItems)
    {
        // 处理SplicingLine类型的项目
        SplicingLine *splicingLine = dynamic_cast<SplicingLine *>(item);
        if (splicingLine)
        {
            // 检查拼接线是否位于当前元素的上方
            qreal lineY = splicingLine->line().y1();
            if (lineY < currentTopY)
            {
                // 移动拼接线
                QLineF currentLine = splicingLine->line();
                currentLine.setP1(QPointF(currentLine.x1(), currentLine.y1() + deltaY));
                currentLine.setP2(QPointF(currentLine.x2(), currentLine.y2() + deltaY));
                splicingLine->setLine(currentLine);
            }
        }

        // 处理MovablePixmapItem类型且不是当前元素本身的项目
        MovablePixmapItem *pixmapItem = dynamic_cast<MovablePixmapItem *>(item);
        if (pixmapItem && pixmapItem != this)
        {
            // 检查该元素是否位于当前元素的上方
            qreal itemBottomY = pixmapItem->sceneBoundingRect().bottom();
            if (itemBottomY < currentTopY)
            {
                // 移动该元素（作为整体移动）
                QPointF itemPos = pixmapItem->pos();
                itemPos.setY(itemPos.y() + deltaY);
                pixmapItem->setFlag(QGraphicsItem::ItemSendsGeometryChanges, false);
                pixmapItem->setPos(itemPos);
                pixmapItem->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
            }
        }
    }
}

// 整体移动currentBottomY以下的
void MovablePixmapItem::moveElementsBelow(qreal currentBottomY, qreal deltaY)
{

    // 遍历场景中的所有项目，找到位于当前元素下方的元素
    QList<QGraphicsItem *> allItems = this->scene()->items();

    for (QGraphicsItem *item : allItems)
    {
        // 处理SplicingLine类型的项目
        SplicingLine *splicingLine = dynamic_cast<SplicingLine *>(item);
        if (splicingLine)
        {
            // 检查拼接线是否位于当前元素的下方
            qreal lineY = splicingLine->line().y1();
            if (lineY > currentBottomY)
            {
                // 移动拼接线
                QLineF currentLine = splicingLine->line();
                currentLine.setP1(QPointF(currentLine.x1(), currentLine.y1() + deltaY));
                currentLine.setP2(QPointF(currentLine.x2(), currentLine.y2() + deltaY));
                splicingLine->setLine(currentLine);
            }
        }

        // 处理MovablePixmapItem类型且不是当前元素本身的项目
        MovablePixmapItem *pixmapItem = dynamic_cast<MovablePixmapItem *>(item);
        if (pixmapItem && pixmapItem != this)
        {
            // 检查该元素是否位于当前元素的下方
            qreal itemTopY = pixmapItem->sceneBoundingRect().top();
            if (itemTopY > currentBottomY)
            {
                // 移动该元素（作为整体移动）
                QPointF itemPos = pixmapItem->pos();
                itemPos.setY(itemPos.y() + deltaY);
                pixmapItem->setFlag(QGraphicsItem::ItemSendsGeometryChanges, false);
                pixmapItem->setPos(itemPos);
                pixmapItem->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
            }
        }
    }
}
