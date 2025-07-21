#include "SplicingLine.h"
#include <QGraphicsScene>
#include <QDebug>

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
    updatePen();
}

void SplicingLine::setHighlighted(bool highlighted)
{
    if (m_highlighted != highlighted)
    {
        m_highlighted = highlighted;
        updatePen();
        update();
    }
}

void SplicingLine::setLineWidth(qreal normalWidth, qreal extensionWidth)
{
    m_normalWidth = normalWidth;
    m_extensionWidth = extensionWidth;
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

    if (m_orientation == SplicingLineOrientation::Horizontal)
    {
        // 水平拼接线，在两端垂直方向扩展
        QPointF p1 = currentLine.p1();
        QPointF p2 = currentLine.p2();

        // 左端扩展
        QLineF leftExtension(p1.x(), p1.y() - m_extensionLength,
                             p1.x(), p1.y() + m_extensionLength);
        painter->setPen(extensionPen);
        painter->drawLine(leftExtension);

        // 右端扩展
        QLineF rightExtension(p2.x(), p2.y() - m_extensionLength,
                              p2.x(), p2.y() + m_extensionLength);
        painter->drawLine(rightExtension);
    }
    else
    {
        // 垂直拼接线，在两端水平方向扩展
        QPointF p1 = currentLine.p1();
        QPointF p2 = currentLine.p2();

        // 上端扩展
        QLineF topExtension(p1.x() - m_extensionLength, p1.y(),
                            p1.x() + m_extensionLength, p1.y());
        painter->setPen(extensionPen);
        painter->drawLine(topExtension);

        // 下端扩展
        QLineF bottomExtension(p2.x() - m_extensionLength, p2.y(),
                               p2.x() + m_extensionLength, p2.y());
        painter->drawLine(bottomExtension);
    }
}

void SplicingLine::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        setHighlighted(!m_highlighted);
        qDebug() << "拼接线被点击，当前状态：" << (m_highlighted ? "高亮" : "普通");
    }
    QGraphicsLineItem::mousePressEvent(event);
}

QRectF SplicingLine::boundingRect() const
{
    QRectF rect = QGraphicsLineItem::boundingRect();

    // 扩展边界矩形以包含超出部分
    qreal extraWidth = m_extensionWidth / 2.0 + m_extensionLength;
    rect.adjust(-extraWidth, -extraWidth, extraWidth, extraWidth);

    return rect;
}

QPainterPath SplicingLine::shape() const
{
    QPainterPath path;
    QLineF currentLine = line();

    // 创建一个包含主线条和扩展部分的形状
    qreal halfWidth = qMax(m_normalWidth, m_extensionWidth) / 2.0;

    if (m_orientation == SplicingLineOrientation::Horizontal)
    {
        // 水平线的形状
        QRectF mainRect(currentLine.p1().x(), currentLine.p1().y() - halfWidth,
                        currentLine.length(), m_normalWidth);
        path.addRect(mainRect);

        // 添加扩展区域
        QRectF leftExt(currentLine.p1().x() - m_extensionWidth / 2,
                       currentLine.p1().y() - m_extensionLength,
                       m_extensionWidth, m_extensionLength * 2);
        path.addRect(leftExt);

        QRectF rightExt(currentLine.p2().x() - m_extensionWidth / 2,
                        currentLine.p2().y() - m_extensionLength,
                        m_extensionWidth, m_extensionLength * 2);
        path.addRect(rightExt);
    }
    else
    {
        // 垂直线的形状
        QRectF mainRect(currentLine.p1().x() - halfWidth, currentLine.p1().y(),
                        m_normalWidth, currentLine.length());
        path.addRect(mainRect);

        // 添加扩展区域
        QRectF topExt(currentLine.p1().x() - m_extensionLength,
                      currentLine.p1().y() - m_extensionWidth / 2,
                      m_extensionLength * 2, m_extensionWidth);
        path.addRect(topExt);

        QRectF bottomExt(currentLine.p2().x() - m_extensionLength,
                         currentLine.p2().y() - m_extensionWidth / 2,
                         m_extensionLength * 2, m_extensionWidth);
        path.addRect(bottomExt);
    }

    return path;
}
