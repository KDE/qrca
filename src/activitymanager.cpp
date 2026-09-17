/*
 * SPDX-FileCopyrightText: 2026 Kai Uwe Broulik <kde@broulik.de>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "activitymanager.h"

#include "config-qrca.h"

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusPendingCallWatcher>
#include <QDBusPendingReply>

using namespace Qt::StringLiterals;

static constexpr QLatin1String g_activityManagerService{"org.kde.ActivityManager"};
static constexpr QLatin1String g_activitiesPath{"/ActivityManager/Activities"};

static constexpr QLatin1String g_activitiesInterface{"org.kde.ActivityManager.Activities"};

static constexpr QLatin1String g_activityFeaturesPath{"/ActivityManager/Features"};
static constexpr QLatin1String g_activityFeaturesInterface{"org.kde.ActivityManager.Features"};

ActivityManager::ActivityManager(QObject *parent)
    : QObject(parent)
{
    auto c = QDBusConnection::sessionBus();
    c.connect(g_activityManagerService, g_activitiesPath, g_activitiesInterface, u"CurrentActivityChanged"_s, this, SLOT(currentActivityChanged(QString)));

    QDBusMessage message = QDBusMessage::createMethodCall(g_activityManagerService, g_activitiesPath, g_activitiesInterface, u"CurrentActivity"_s);
    auto call = c.asyncCall(message);
    auto *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [this, watcher] {
        watcher->deleteLater();

        QDBusPendingReply<QString> reply = *watcher;
        if (reply.isError()) {
            qWarning() << "Failed to get current activity:" << reply.error().message();
            // Also emit on failure so HistoryModel always loads.
            currentActivityChanged(QString());
        } else {
            currentActivityChanged(reply.value());
        }
    });
}

ActivityManager::~ActivityManager() = default;

QString ActivityManager::currentActivityId() const
{
    return m_currentActivityId;
}

bool ActivityManager::currentActivityIsOffTheRecord() const
{
    return m_activityOffTheRecord;
}

void ActivityManager::currentActivityChanged(const QString &activityId)
{
    m_currentActivityId = activityId;
    Q_EMIT currentActivityIdChanged(activityId);
    checkActivityOffTheRecord();
}

void ActivityManager::checkActivityOffTheRecord()
{
    // Unfortunately, there seems to be no signal when the user changes this setting,
    // ActivityChanged is not emitted when changing a feature.
    QDBusMessage message = QDBusMessage::createMethodCall(g_activityManagerService, g_activityFeaturesPath, g_activityFeaturesInterface, u"GetValue"_s);
    message.setArguments({u"org.kde.ActivityManager.Resources.Scoring/isOTR/%1"_s.arg(m_currentActivityId)});

    auto call = QDBusConnection::sessionBus().asyncCall(message);
    auto *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [this, watcher] {
        watcher->deleteLater();

        QDBusPendingReply<QDBusVariant> reply = *watcher;
        if (reply.isError()) {
            qWarning() << "Failed to check whether actvity is off the record:" << reply.error().message();
        } else {
            m_activityOffTheRecord = reply.value().variant().toBool();
        }
    });
}

#include "moc_activitymanager.cpp"
