#include "PoliticianPictureProxyModel.h"

#include <QtGlobal>

#include <QFileInfo>

#include "PoliticianModel.h"

PoliticianPictureProxyModel::PoliticianPictureProxyModel(
    QObject* parent, PoliticianModel* politicianModel)
    : QIdentityProxyModel(parent)
{
    setSourceModel(politicianModel);
}

QVariant PoliticianPictureProxyModel::data(
    const QModelIndex& index, int role) const
{
    if (role != Qt::DisplayRole)
        return QIdentityProxyModel::data(index, role);
    QFileInfo fileInfo(politicianModel()->data(
        index, PoliticianModel::FilePathRole).toString());
    if (!pixmapCache_.contains(fileInfo.absoluteFilePath()))
        return QPixmap();
    return pixmapCache_[fileInfo.absoluteFilePath()];
}

void PoliticianPictureProxyModel::setSourceModel(QAbstractItemModel* source)
{
    if (source)
        Q_ASSERT(qobject_cast<PoliticianModel*>(source));
    QIdentityProxyModel::setSourceModel(source);

    reloadCache();
    if (!source)
        return;

    connect(source, &QAbstractItemModel::modelReset,
        this, &PoliticianPictureProxyModel::reloadCache);
    connect(source, &QAbstractItemModel::rowsInserted,
        [this](const QModelIndex& /*parent*/, int first, int last) {
        partiallyReloadCache(index(first, 0), last - first + 1);
    });
    connect(source, &QAbstractItemModel::dataChanged,
        [this](const QModelIndex& topLeft, const QModelIndex& bottomRight) {
        auto count = bottomRight.row() - topLeft.row();
        if (count <= 0)
            return;
        partiallyReloadCache(topLeft, count);
    });
}

PoliticianModel* PoliticianPictureProxyModel::politicianModel() const
{
    return qobject_cast<PoliticianModel*>(sourceModel());
}

void PoliticianPictureProxyModel::partiallyReloadCache(
    const QModelIndex& startIndex, int count)
{
    if (!sourceModel() 
        || !politicianModel() 
        || count <= 0 
        || count + startIndex.row() > rowCount())
        return;
    const auto& politicianMod = *(politicianModel());
    for (auto i = 0; i < count; ++i)
    {
        auto filePath = politicianMod.data(
            index(startIndex.row() + i, 0), PoliticianModel::FilePathRole).
                toString();
        auto absFilePath = QFileInfo(filePath).absoluteFilePath();
        pixmapCache_[absFilePath] = QPixmap(absFilePath);
    }
}

void PoliticianPictureProxyModel::reloadCache()
{
    pixmapCache_.clear();
    if (!sourceModel() || !politicianModel())
        return;
    partiallyReloadCache(index(0, 0), rowCount());
}
