/*
 * SPDX-FileCopyrightText: 2026 Kai Uwe Broulik <kde@broulik.de>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#pragma once

#include <QObject>
#include <QUrl>

#include <Prison/ScanResult>

class Importer : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool busy READ busy NOTIFY busyChanged)

public:
    explicit Importer(QObject *parent = nullptr);
    ~Importer() override;

    bool busy() const;
    Q_SIGNAL void busyChanged(bool busy);

    Q_INVOKABLE void load(const QUrl &url);
    Q_INVOKABLE void load(const QByteArray &data);

Q_SIGNALS:
    void canceled();
    void failed(int errorCode, const QString &errorString);
    void finished(const QImage &image, const Prison::ScanResult &result);

private:
    void setBusy(bool busy);

    void scanImage(const QImage &image);

    bool m_busy = false;
};
