// SPDX-FileCopyrightText: (C) 2017 Atul Sharma <atulsharma406@gmail.com>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "notificationmanager.h"
#include <klocalizedstring.h>

NotificationManager::NotificationManager(QObject *parent)
    : QObject(parent)
{
    m_sharingSuccess = new KNotification(QStringLiteral("sharingSuccess"), KNotification::Persistent, this);
}

NotificationManager::~NotificationManager()
{
}

void NotificationManager::showNotification(const QString &url)
{
    m_sharingSuccess->setText(i18n("Shared url for image is <a href='%1'>%1</a>"));
    m_sharingSuccess->sendEvent();
}
