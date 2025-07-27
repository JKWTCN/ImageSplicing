#ifndef SPLICINGLINE_H
#define SPLICINGLINE_H

#include <QGraphicsLineItem>
#include <QPen>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include "MovablePixmapItem.h"
enum class SplicingLineOrientation
{
    Horizontal,
    Vertical
};

class SplicingLine : public QGraphicsLineItem
{
public:
    SplicingLine(qreal x1, qreal y1, qreal x2, qreal y2,
                 SplicingLineOrientation orientation,
                 QGraphicsItem *parent = nullptr);

    // 设置是否被选中/点击状态
    void setHighlighted(bool highlighted);
    bool isHighlighted() const { return m_highlighted; }

    // 设置拼接线的扩展长度（超出图片的部分）
    void setExtensionLength(qreal length) { m_extensionLength = length; }
    qreal extensionLength() const { return m_extensionLength; }

    // 设置和获取初始中心点坐标
    void setInitialPos(const QPointF &center) { m_initialCenter = center; }
    QPointF getInitialCenter() const { return m_initialCenter; }

    // 设置线条粗细
    void setLineWidth(qreal normalWidth, qreal extensionWidth);

    // 获取和设置拼接线两端图片
    MovablePixmapItem *getLastItem() { return lastItem; }
    MovablePixmapItem *getNextItem() { return nextItem; }
    void setLastItem(MovablePixmapItem *item);
    void setNextItem(MovablePixmapItem *item);

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    QRectF boundingRect() const override;
    QPainterPath shape() const override;

private:
    void updatePen();
    SplicingLineOrientation m_orientation;
    bool m_highlighted;
    qreal m_extensionLength;
    qreal m_normalWidth;
    qreal m_extensionWidth;

    QColor m_normalColor;
    QColor m_highlightColor;
    QColor m_extensionColor;
    // 拼接线对应的两端图片
    MovablePixmapItem *lastItem = nullptr;
    MovablePixmapItem *nextItem = nullptr;
    // 初始中心点坐标
    QPointF m_initialCenter;
};

#endif // SPLICINGLINE_H
