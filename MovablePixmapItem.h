#ifndef _MOVABLE_PIXMAP_ITEM_H
#define _MOVABLE_PIXMAP_ITEM_H
#include <QGraphicsPixmapItem>
#include <QPainter>
#include "enumerate.h"
// Forward declarations
class SplicingLine;

class MovablePixmapItem : public QGraphicsPixmapItem
{
public:
    MovablePixmapItem(const QPixmap &pixmap, Move_Type moveType, qreal initialX, qreal initialY, qreal initialZ, QGraphicsItem *parent = nullptr);

    // 设置相邻的拼接线
    void setTopSplicingLine(SplicingLine *line) { topSplicingLine = line; }
    void setBottomSplicingLine(SplicingLine *line) { bottomSplicingLine = line; }
    // 设置相邻的拼接线
    void setLeftSplicingLine(SplicingLine *line) { leftSplicingLine = line; }
    void setRightSplicingLine(SplicingLine *line) { rightSplicingLine = line; }

    // 获取相邻的拼接线
    SplicingLine *getTopSplicingLine() const { return topSplicingLine; }
    SplicingLine *getBottomSplicingLine() const { return bottomSplicingLine; }
    SplicingLine *getLeftSplicingLine() const { return leftSplicingLine; }
    SplicingLine *getRightSplicingLine() const { return rightSplicingLine; }

    Move_Type getMoveType() { return this->moveType; }
    // 设置和获取初始中心点坐标
    void setInitialPos(const QPointF &center) { initialCenter = center; }
    QPointF getInitialCenter() const { return initialCenter; }

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    QPainterPath shape() const override;

private:
    // 移动当前元素下方的所有元素（作为整体移动）
    void moveElementsBelow(qreal deltaY);
    // 移动当前元素上方的所有元素（作为整体移动）
    void moveElementsAbove(qreal deltaY);
    // 移动当前元素左侧的所有元素（作为整体移动）
    void moveElementsLeft(qreal deltaX);
    // 移动当前元素右侧的所有元素（作为整体移动）
    void moveElementsRight(qreal deltaX);
    SplicingLine *topSplicingLine = nullptr;    // 上方的拼接线
    SplicingLine *bottomSplicingLine = nullptr; // 下方的拼接线
    SplicingLine *leftSplicingLine = nullptr;   // 左侧的拼接线
    SplicingLine *rightSplicingLine = nullptr;  // 右侧的拼接线
    Move_Type moveType;
    QPointF initialCenter; // 初始中心点坐标
};
#endif
