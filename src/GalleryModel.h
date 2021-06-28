#pragma once

#include <QAbstractListModel>

class GalleryModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit GalleryModel(QObject *parent = nullptr);

    QHash<int, QByteArray> roleNames() const override;
    QVariant data(const QModelIndex &index, int role) const override;
    int rowCount(const QModelIndex &index) const override;

    enum Role {
        Title = Qt::UserRole + 1,
        Type,
        Data
    };
};

