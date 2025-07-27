#include "SplicingLine.h"
#include <QGraphicsScene>
#include <QDebug>
#include <QtMath>

SplicingLine::SplicingLine(qreal x1, qreal y1, qreal x2, qreal y2,
                           SplicingLineOrientation orientation,
                           QGraphicsItem *parent)
    : QGraphicsLineItem(x1, y1, x2, y2, parent), m_orientation(orientation), m_highlighted(false), m_extensionLength(20.0) // 默认超出20像素
      ,
      m_normalWidth(2.0) // 普通线条宽度
      ,
      m_extensionWidth(8.0) // 超出部分宽度
      ,
      m_normalColor(Qt::blue), m_highlightColor(Qt::red), m_extensionColor(Qt::darkBlue)
{
    setFlags(QGraphicsItem::ItemIsSelectable);
    setAcceptHoverEvents(true);
    // 初始化中心点坐标
    QLineF lineF(x1, y1, x2, y2);
    setInitialPos(lineF.center());
    qDebug() << "SplicingLine:初始中心点:" << m_initialCenter;
    updatePen();
}

void SplicingLine::setHighlighted(bool highlighted)
{
    if (m_highlighted != highlighted)
    {
        lastItem->setFlag(QGraphicsItem::ItemIsMovable, highlighted);
        nextItem->setFlag(QGraphicsItem::ItemIsMovable, highlighted);
        m_highlighted = highlighted;
        updatePen();
        update();
    }
}

void SplicingLine::setLineWidth(qreal normalWidth, qreal extensionWidth)
{
    m_normalWidth = normalWidth;
    m_extensionWidth = extensionWidth * 5;
    updatePen();
    update();
}

void SplicingLine::updatePen()
{
    QPen newPen;
    if (m_highlighted)
    {
        newPen.setColor(m_highlightColor);
        newPen.setWidth(m_normalWidth + 1);
    }
    else
    {
        newPen.setColor(m_normalColor);
        newPen.setWidth(m_normalWidth);
    }
    newPen.setCapStyle(Qt::RoundCap);
    setPen(newPen);
}

void SplicingLine::setLastItem(MovablePixmapItem *item)
{
    lastItem = item;
}

void SplicingLine::setNextItem(MovablePixmapItem *item)
{
    nextItem = item;
}

void SplicingLine::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    QLineF currentLine = line();

    // 绘制主线条部分
    QPen mainPen = pen();
    painter->setPen(mainPen);
    painter->drawLine(currentLine);

    // 绘制超出部分（更粗的线条）
    QPen extensionPen;
    extensionPen.setColor(m_highlighted ? m_highlightColor : m_extensionColor);
    extensionPen.setWidth(m_extensionWidth);
    extensionPen.setCapStyle(Qt::RoundCap);

    // 绘制两端的圆形扩展
    QPointF p1 = currentLine.p1();
    QPointF p2 = currentLine.p2();

    // 计算线条的方向向量
    QPointF direction = p2 - p1;
    qreal length = qSqrt(direction.x() * direction.x() + direction.y() * direction.y());
    if (length > 0)
    {
        direction = direction / length; // 单位向量
    }

    // 在线条两端延长 m_extensionLength 的距离放置圆形
    QPointF extendedP1 = p1 - direction * m_extensionLength;
    QPointF extendedP2 = p2 + direction * m_extensionLength;

    // 设置画刷填充圆形
    QBrush extensionBrush(m_highlighted ? m_highlightColor : m_extensionColor);
    painter->setBrush(extensionBrush);
    painter->setPen(extensionPen);

    // 圆形半径使用扩展宽度的一半
    qreal circleRadius = m_extensionWidth / 2.0;

    // 绘制第一个端点的圆形（在延长位置）
    QRectF circle1(extendedP1.x() - circleRadius, extendedP1.y() - circleRadius,
                   circleRadius * 2, circleRadius * 2);
    painter->drawEllipse(circle1);

    // 绘制第二个端点的圆形（在延长位置）
    QRectF circle2(extendedP2.x() - circleRadius, extendedP2.y() - circleRadius,
                   circleRadius * 2, circleRadius * 2);
    painter->drawEllipse(circle2);
}

void SplicingLine::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        setHighlighted(!m_highlighted);
        qDebug() << "拼接线被点击，当前状态：" << (m_highlighted ? "高亮" : "普通") << endl;
    }
    QGraphicsLineItem::mousePressEvent(event);
}

QRectF SplicingLine::boundingRect() const
{
    QRectF rect = QGraphicsLineItem::boundingRect();

    // 扩展边界矩形以包含延长位置的圆形
    qreal circleRadius = m_extensionWidth / 2.0;
    qreal totalExtension = m_extensionLength + circleRadius;
    rect.adjust(-totalExtension, -totalExtension, totalExtension, totalExtension);

    return rect;
}

QPainterPath SplicingLine::shape() const
{
    QPainterPath path;
    QLineF currentLine = line();

    // 创建一个包含主线条和圆形扩展部分的形状
    qreal halfWidth = m_normalWidth / 2.0;

    if (m_orientation == SplicingLineOrientation::Horizontal)
    {
        // 水平线的形状
        QRectF mainRect(currentLine.p1().x(), currentLine.p1().y() - halfWidth,
                        currentLine.length(), m_normalWidth);
        path.addRect(mainRect);
    }
    else
    {
        // 垂直线的形状
        QRectF mainRect(currentLine.p1().x() - halfWidth, currentLine.p1().y(),
                        m_normalWidth, currentLine.length());
        path.addRect(mainRect);
    }

    // 添加两端的圆形区域（在延长位置）
    QPointF p1 = currentLine.p1();
    QPointF p2 = currentLine.p2();

    // 计算线条的方向向量
    QPointF direction = p2 - p1;
    qreal length = qSqrt(direction.x() * direction.x() + direction.y() * direction.y());
    if (length > 0)
    {
        direction = direction / length; // 单位向量
    }

    // 在线条两端延长 m_extensionLength 的距离放置圆形
    QPointF extendedP1 = p1 - direction * m_extensionLength;
    QPointF extendedP2 = p2 + direction * m_extensionLength;

    qreal circleRadius = m_extensionWidth / 2.0;

    // 第一个端点的圆形（在延长位置）
    QRectF circle1(extendedP1.x() - circleRadius, extendedP1.y() - circleRadius,
                   circleRadius * 2, circleRadius * 2);
    path.addEllipse(circle1);

    // 第二个端点的圆形（在延长位置）
    QRectF circle2(extendedP2.x() - circleRadius, extendedP2.y() - circleRadius,
                   circleRadius * 2, circleRadius * 2);
    path.addEllipse(circle2);

    return path;
}
