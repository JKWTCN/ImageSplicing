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
    MovablePixmapItem(const QPixmap &pixmap, Move_Type moveType, QGraphicsItem *parent = nullptr);

    // 设置相邻的拼接线
    void setTopSplicingLine(SplicingLine *line) { topSplicingLine = line; }
    void setBottomSplicingLine(SplicingLine *line) { bottomSplicingLine = line; }
    // 设置相邻的拼接线
    void setLeftSplicingLine(SplicingLine *line) { leftSplicingLine = line; }
    void setRightSplicingLine(SplicingLine *line) { rightSplicingLine = line; }
    Move_Type getMoveType() { return this->moveType; }

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    SplicingLine *topSplicingLine = nullptr;    // 上方的拼接线
    SplicingLine *bottomSplicingLine = nullptr; // 下方的拼接线
    SplicingLine *leftSplicingLine = nullptr;   // 左侧的拼接线
    SplicingLine *rightSplicingLine = nullptr;  // 右侧的拼接线
    Move_Type moveType;
};
#endif
