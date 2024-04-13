// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "clipboard.h"

#include <QClipboard>
#include <QGuiApplication>
#include <QMimeData>
#include <QRegularExpression>
#include <QUrl>

Clipboard::Clipboard(QObject *parent)
    : QObject(parent)
    , m_clipboard(QGuiApplication::clipboard())
{
}

void Clipboard::saveText(const QString &message)
{
    QRegularExpression re(QStringLiteral("<[^>]*>"));
    auto *mineData = new QMimeData; // ownership is transferred to clipboard
    mineData->setHtml(message);
    QString html = message;
    mineData->setText(html.replace(re, QString()));
    m_clipboard->setMimeData(mineData);
}

#include "moc_clipboard.cpp"
