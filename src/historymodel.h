/*
 * SPDX-FileCopyrightText: 2026 Kai Uwe Broulik <kde@broulik.de>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#pragma once

#include <QAbstractListModel>

#include <KSharedConfig>

#include "QrCodeContent.h"

class HistoryModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)

public:
    explicit HistoryModel(QObject *parent = nullptr);
    ~HistoryModel() override;

    enum Roles {
        TextRole = Qt::DisplayRole,
        ContentRole = Qt::UserRole + 1,
    };

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void add(const QrCodeContent &content);
    Q_INVOKABLE void removeAt(int row);
    Q_INVOKABLE void clear();

Q_SIGNALS:
    void countChanged();

private:
    void writeConfig();

    QVector<QrCodeContent> m_entries;
    KSharedConfig::Ptr m_config;
};
