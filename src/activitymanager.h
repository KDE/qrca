/*
 * SPDX-FileCopyrightText: 2026 Kai Uwe Broulik <kde@broulik.de>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#pragma once

#include <QObject>

class ActivityManager : public QObject
{
    Q_OBJECT

public:
    explicit ActivityManager(QObject *parent = nullptr);
    ~ActivityManager() override;

    QString currentActivityId() const;
    bool currentActivityIsOffTheRecord() const;

Q_SIGNALS:
    void currentActivityIdChanged(const QString &activityId);

private Q_SLOTS:
    void currentActivityChanged(const QString &activityId);

private:
    void checkActivityOffTheRecord();

    QString m_currentActivityId;
    bool m_activityOffTheRecord = false;
};
