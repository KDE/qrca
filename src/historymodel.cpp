/*
 * SPDX-FileCopyrightText: 2026 Kai Uwe Broulik <kde@broulik.de>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "historymodel.h"

#include "config-qrca.h"

#include <KConfigGroup>
#include <KLocalizedString>

#include <QMetaEnum>

#include <algorithm>

#if HAVE_QTDBUS
#include "activitymanager.h"
#endif

using namespace Qt::StringLiterals;

HistoryModel::HistoryModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_config(KSharedConfig::openStateConfig())
#if HAVE_QTDBUS
    , m_activityManager(new ActivityManager(this))
#endif
{
    connect(this, &QAbstractListModel::rowsInserted, this, &HistoryModel::countChanged);
    connect(this, &QAbstractListModel::rowsRemoved, this, &HistoryModel::countChanged);
    connect(this, &QAbstractListModel::modelReset, this, &HistoryModel::countChanged);

#if HAVE_QTDBUS
    // TODO Clear history of activities that got removed.
    connect(m_activityManager, &ActivityManager::currentActivityIdChanged, this, &HistoryModel::reload);
#else
    reload();
#endif
}

HistoryModel::~HistoryModel() = default;

void HistoryModel::reload()
{
    KConfigGroup historyGroup(m_config, u"History"_s);

    KConfigGroup group = historyGroup;
#if HAVE_QTDBUS
    if (!m_activityManager->currentActivityId().isEmpty()) {
        group = KConfigGroup(&historyGroup, m_activityManager->currentActivityId());
    }
#endif

    const auto formatEnum = QMetaEnum::fromType<Prison::Format::BarcodeFormat>();

    QStringList entryGroupNames = group.groupList();
    std::sort(entryGroupNames.begin(), entryGroupNames.end(), [](const auto &a, const auto &b) {
        return a.toInt() < b.toInt();
    });

    beginResetModel();
    m_entries.clear();
    m_entries.reserve(entryGroupNames.size());
    for (const auto &entryGroupName : entryGroupNames) {
        KConfigGroup entryGroup(&group, entryGroupName);

        Prison::Format::BarcodeFormat format = Prison::Format::QRCode;

        const QString formatString = entryGroup.readEntry(u"Format"_s);
        if (!formatString.isEmpty()) {
            bool ok;
            const int formatValue = formatEnum.keyToValue(formatString.toUtf8(), &ok);
            if (!ok) {
                qWarning() << "Unsupported or invalid barcode format:" << formatString;
                continue;
            }
            format = static_cast<Prison::Format::BarcodeFormat>(formatValue);
        }

        const QString text = entryGroup.readEntry(u"Text"_s, QString());
        if (!text.isNull()) {
            QrCodeContent content{text, format};
            m_entries.append(content);
        } else {
            QrCodeContent content{entryGroup.readEntry(u"BinaryContent"_s, QByteArray()), format};
            m_entries.append(content);
        }
    }
    endResetModel();
}

void HistoryModel::writeConfig()
{
    KConfigGroup historyGroup(m_config, u"History"_s);

    KConfigGroup group = historyGroup;
#if HAVE_QTDBUS
    if (!m_activityManager->currentActivityId().isEmpty()) {
        group = KConfigGroup(&historyGroup, m_activityManager->currentActivityId());
    }
#endif
    group.deleteGroup();

    const auto formatEnum = QMetaEnum::fromType<Prison::Format::BarcodeFormat>();

    for (qsizetype i = 0; i < m_entries.size(); ++i) {
        const auto &entry = m_entries.at(i);

        KConfigGroup entryGroup(&group, QString::number(i));
        if (entry.isPlainText()) {
            entryGroup.writeEntry(u"Text"_s, entry.text());
        } else {
            entryGroup.writeEntry(u"BinaryContent"_s, entry.binaryContent());
        }
        // Assume QRCode if not stated otherwise, don't write it out.
        if (entry.format() != Prison::Format::QRCode) {
            // Can KConfigGroup not serialize enums automatically?
            entryGroup.writeEntry(u"Format"_s, formatEnum.valueToKey(entry.format()));
        }
    }
}

void HistoryModel::add(const QrCodeContent &content)
{
    if (content.contentType() == QrCodeContent::OtpToken) {
        return;
    }

#if HAVE_QTDBUS
    if (m_activityManager->currentActivityIsOffTheRecord()) {
        return;
    }
#endif

    auto it = std::find_if(m_entries.begin(), m_entries.end(), [&content](const auto &candidate) {
        return candidate.text() == content.text() && candidate.binaryContent() == content.binaryContent();
    });
    if (it != m_entries.end()) {
        const auto index = std::distance(m_entries.begin(), it);

        // Nothing to do, already the first entry.
        if (index == 0) {
            return;
        }

        beginRemoveRows(QModelIndex(), index, index);
        m_entries.erase(it);
        endRemoveRows();
    }

    beginInsertRows(QModelIndex(), 0, 0);
    m_entries.prepend(content);
    endInsertRows();

    while (m_entries.size() > 100) {
        beginRemoveRows(QModelIndex(), m_entries.size() - 1, m_entries.size() - 1);
        m_entries.removeLast();
        endRemoveRows();
    }

    writeConfig();
}

void HistoryModel::removeAt(int row)
{
    if (row < 0 || row >= m_entries.size()) {
        return;
    }

    beginRemoveRows(QModelIndex(), row, row);
    m_entries.removeAt(row);
    endRemoveRows();

    writeConfig();
}

void HistoryModel::clear()
{
    if (m_entries.isEmpty()) {
        return;
    }

    beginResetModel();
    m_entries.clear();
    endResetModel();

    writeConfig();
}

QVariant HistoryModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_entries.size()) {
        return {};
    }

    const auto &entry = m_entries.at(index.row());

    switch (role) {
    case Qt::DisplayRole:
        return entry.displayText();
    case ContentRole:
        return QVariant::fromValue(entry);
    }

    return {};
}

int HistoryModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_entries.size();
}

QHash<int, QByteArray> HistoryModel::roleNames() const
{
    auto roles = QAbstractListModel::roleNames();
    // Default "display" conflicts with AbstractButton "display" property...
    roles[TextRole] = "text"_ba;
    roles[ContentRole] = "content"_ba;
    return roles;
}

#include "moc_historymodel.cpp"
