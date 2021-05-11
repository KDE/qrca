/*
    SPDX-FileCopyrightText: 2021 Volker Krause <vkrause@kde.org>
    SPDX-FileCopyrightText: 2019 Jonah Brüchert <jbb@kaidan.im>
    SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "QrCodeContent.h"

#include <QRegularExpression>
#include <QUrlQuery>

QrCodeContent::QrCodeContent() = default;

QrCodeContent::QrCodeContent(const QByteArray &content)
    : m_content(content)
{
}

QrCodeContent::QrCodeContent(const QString &content)
    : m_content(content)
{
}

QrCodeContent::~QrCodeContent() = default;

static bool isUrl(const QString &text)
{
    QRegularExpression exp(QStringLiteral("(?:https?|ftp)://\\S+"));

    return exp.match(text).hasMatch();
}

static bool isVCard(const QString &text)
{
    return (text.startsWith(QLatin1String("BEGIN:VCARD")) && text.trimmed().endsWith(QLatin1String("END:VCARD")));
}

static bool isOtpToken(const QString &text)
{
    if (text.startsWith(QLatin1String("otpauth"))) {
        QUrl uri(text);
        if (uri.isValid() && (uri.host() == QLatin1String("totp") || uri.host() == QLatin1String("hotp"))) {
            QUrlQuery query(uri.query());
            return query.hasQueryItem(QStringLiteral("secret"));
        }
    }

    return false;
}

QrCodeContent::ContentType QrCodeContent::contentType() const
{
    if (m_content.type() == QVariant::ByteArray)
        return ContentType::Binary;

    const auto text = m_content.toString();
    if (isUrl(text))
        return ContentType::Url;
    else if (isVCard(text))
        return ContentType::VCard;
    else if (isOtpToken(text))
        return ContentType::OtpToken;

    return ContentType::Text;
}

QString QrCodeContent::text() const
{
    if (m_content.type() == QVariant::String) {
        return m_content.toString();
    }
    return {};
}

QByteArray QrCodeContent::binaryContent() const
{
    if (m_content.type() == QVariant::ByteArray) {
        return m_content.toByteArray();
    }
    return {};
}
