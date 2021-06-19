// SPDX-FileCopyrightText: 2017 Atul Sharma <atulsharma406@gmail.com>
// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

#pragma once

#include <QVariant>
#include <knotification.h>

class NotificationManager : public QObject
{
    Q_OBJECT
public:
    explicit NotificationManager(QObject *parent = nullptr);
    ~NotificationManager();

    //! @argument url: the valid url returned after sharing the image
    Q_INVOKABLE void showNotification(const QString &url);

private:
    KNotification *m_sharingSuccess;
};
