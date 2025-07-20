#include <QObject>
#include <QGraphicsPixmapItem>
#include <QList>
#include <QMap>
#include <VerticalMovablePixmapItem.h>

class ItemManager : public QObject
{
    Q_OBJECT
public:
    // 添加需要管理的item
    void addItem(VerticalMovablePixmapItem *item)
    {
        if (!items.contains(item))
        {
            items.append(item);
            item->setFlag(QGraphicsItem::ItemIsMovable); // 允许移动
            lastPositions[item] = item->pos();           // 记录初始位置
        }
    }

    // 开始监控所有item的位置变化
    void startTracking()
    {
        foreach (VerticalMovablePixmapItem *item, items)
        {
            // 连接位置变化信号
//            connect(item, &QGraphicsPixmapItem::itemChange,
//                    this, &ItemManager::onItemMoved);
        }
    }

    // 停止监控
    void stopTracking()
    {
        foreach (VerticalMovablePixmapItem *item, items)
        {
//            disconnect(item, &VerticalMovablePixmapItem::itemChange,
//                       this, &ItemManager::onItemMoved);
        }
    }

private slots:
    // 当任一item移动时的处理函数
    void onItemMoved()
    {
        VerticalMovablePixmapItem *movedItem = qobject_cast<VerticalMovablePixmapItem *>(sender());
        if (!movedItem || isUpdating)
            return;

        isUpdating = true; // 防止递归调用

        // 计算移动的增量
        QPointF delta = movedItem->pos() - lastPositions.value(movedItem, QPointF());
        // todo 更新其他item的位置
        //        // 更新所有其他item的位置
        //        foreach (QGraphicsPixmapItem *item, items)
        //        {
        //            if (item != movedItem)
        //            {
        //                item->setPos(item->pos() + delta);
        //            }
        //            // 更新记录的位置
        //            lastPositions[item] = item->pos();
        //        }

        isUpdating = false;
    }

private:
    QList<VerticalMovablePixmapItem *> items;                 // 存储所有管理的item
    QMap<VerticalMovablePixmapItem *, QPointF> lastPositions; // 记录每个item的上次位置
    bool isUpdating = false;                                  // 防止的递归调用
};
