#include <QGraphicsPixmapItem>
#include <QPainter>

// 前向声明
class SplicingLine;

class HorizontalMovablePixmapItem : public QGraphicsPixmapItem
{
public:
    HorizontalMovablePixmapItem(const QPixmap &pixmap, QGraphicsItem *parent = nullptr);

    // 设置相邻的拼接线
    void setLeftSplicingLine(SplicingLine *line) { leftSplicingLine = line; }
    void setRightSplicingLine(SplicingLine *line) { rightSplicingLine = line; }

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    SplicingLine *leftSplicingLine = nullptr;  // 左侧的拼接线
    SplicingLine *rightSplicingLine = nullptr; // 右侧的拼接线
};
