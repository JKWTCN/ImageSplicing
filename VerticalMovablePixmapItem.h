#include <QGraphicsPixmapItem>
#include <QPainter>
#include <QGraphicsScene>

// 前向声明
class SplicingLine;

class VerticalMovablePixmapItem : public QGraphicsPixmapItem
{
public:
    VerticalMovablePixmapItem(const QPixmap &pixmap, QGraphicsItem *parent = nullptr);

    // 设置相邻的拼接线
    void setTopSplicingLine(SplicingLine *line) { topSplicingLine = line; }
    void setBottomSplicingLine(SplicingLine *line) { bottomSplicingLine = line; }

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    SplicingLine *topSplicingLine = nullptr;    // 上方的拼接线
    SplicingLine *bottomSplicingLine = nullptr; // 下方的拼接线
};
