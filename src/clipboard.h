// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

#pragma once

#include <QObject>

class QClipboard;
class QImage;

/**
 * Clipboard proxy
 */
class Clipboard : public QObject
{
    Q_OBJECT
public:
    explicit Clipboard(QObject *parent = nullptr);
    Q_INVOKABLE void saveText(const QString &message);

private:
    QClipboard *m_clipboard;
};
