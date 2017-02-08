#include "timelinemodel.h"
#include "animationscene.h"
#include "rectangle.h"
#include <QPixmap>
#include <QPropertyAnimation>

TimelineModel::TimelineModel()
{
    QVariant rootData = "Root";
    QVariant data;

    m_rootItem = new TreeItem(rootData, data);
}

void TimelineModel::addItemToAnimate(QGraphicsItem *item)
{
    beginInsertRows(QModelIndex(), m_rootItem->childCount() - 1, m_rootItem->childCount() - 1);
    TreeItem *treeItem = new TreeItem(getItemTypeName(item), qVariantFromValue((void *) item), m_rootItem, 1);
    m_rootItem->appendChild(treeItem);
    endInsertRows();
}

void TimelineModel::addProperty(QString text, QModelIndex index)
{
    QVariant v = index.data(Qt::UserRole);
    ResizeableItem *item = (ResizeableItem *) v.value<void *>();
    if(item)
    {
        TreeItem *parent = static_cast<TreeItem*>(index.internalPointer());
        beginInsertRows(index, parent->childCount() - 1, parent->childCount() - 1);
        const QByteArray propName(text.toLatin1());
        QPropertyAnimation *anim = new QPropertyAnimation();
        anim->setTargetObject(item);
        anim->setPropertyName(propName);
        TreeItem *treeItem = new TreeItem(text, qVariantFromValue((void *) anim), parent, 2);
        parent->appendChild(treeItem);
        endInsertRows();
    }
}

int TimelineModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<TreeItem*>(parent.internalPointer())->columnCount();
    else
        return m_rootItem->columnCount();
}

QVariant TimelineModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if(role == Qt::DecorationRole)
        return QVariant();//QPixmap(":/images/rect.png");

    if (role == Qt::DisplayRole)
    {
        TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
        return item->data(0);
    }

    if (role == Qt::UserRole)
    {
        TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
        return item->data(1);
    }

    if (role == Qt::UserRole + 1)
    {
        TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
        return item->data(2);
    }

    return QVariant();
}

Qt::ItemFlags TimelineModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return QAbstractItemModel::flags(index);
}

QVariant TimelineModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return m_rootItem->data(section);

    return QVariant();
}

QModelIndex TimelineModel::index(int row, int column, const QModelIndex &parent)
            const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    TreeItem *parentItem;

    if (!parent.isValid())
        parentItem = m_rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    TreeItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex TimelineModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    TreeItem *childItem = static_cast<TreeItem*>(index.internalPointer());
    TreeItem *parentItem = childItem->parentItem();

    if (parentItem == m_rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int TimelineModel::rowCount(const QModelIndex &parent) const
{
    TreeItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = m_rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    return parentItem->childCount();
}
