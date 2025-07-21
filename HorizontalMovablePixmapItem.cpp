#include "HorizontalMovablePixmapItem.h"
#include "SplicingLine.h"

// 绘制区域
QRectF area;

HorizontalMovablePixmapItem::HorizontalMovablePixmapItem(const QPixmap &pixmap, QGraphicsItem *parent)
    : QGraphicsPixmapItem(pixmap, parent)
{
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
    area = this->boundingRect();
}

QVariant HorizontalMovablePixmapItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionChange && scene())
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
        area = this->boundingRect();
        return newPos;
    }
    return QGraphicsPixmapItem::itemChange(change, value);
}

void HorizontalMovablePixmapItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->save();

    // 计算裁剪区域：只显示拼接线之间的部分
    QRectF itemRect = this->boundingRect();
    QPointF itemPos = this->pos();

    // 创建裁剪矩形
    QRectF clipRect = itemRect;

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

    // 应用裁剪
    painter->setClipRect(clipRect);

    QGraphicsPixmapItem::paint(painter, option, widget);
    painter->restore();
}
