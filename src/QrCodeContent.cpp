/*
    SPDX-FileCopyrightText: 2021 Volker Krause <vkrause@kde.org>
    SPDX-FileCopyrightText: 2019 Jonah Brüchert <jbb@kaidan.im>
    SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "QrCodeContent.h"
#include <qrcastore.h>

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

bool QrCodeContent::saveCode()
{
    if (!m_content.canConvert<QString>()) {
        return false;
    }

    auto codes = QrcaStorage::self()->codes();
    codes.append(m_content.toString());
    QrcaStorage::self()->setCodes(codes);
    QrcaStorage::self()->save();

    return true;
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

static constexpr const char base45Table[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ $%*+-./:";

static bool isBase45(const QString &text)
{
    return std::all_of(text.begin(), text.end(), [](QChar c) {
        return c.row() == 0 && std::find(std::begin(base45Table), std::end(base45Table), c.cell()) != std::end(base45Table);
    });
}

static bool isHealtCertificate(const QString &text)
{
    // EU DGC
    return text.size() > 400 && text.startsWith(QLatin1String("HC1:")) && isBase45(text);
}

static bool isTransportTicket(const QByteArray &data)
{
    // UIC 918.3 train ticket containers
    if (data.size() > 100 && (data.startsWith("#UT") || data.startsWith("OTI"))) {
        return true;
    }

    // VDV eTicket
    if (data.size() >= 352 && data.startsWith((char)0x9E) && data.contains("VDV")) {
        return true;
    }

    return false;
}

QrCodeContent::ContentType QrCodeContent::contentType() const
{
    if (m_content.type() == QVariant::ByteArray) {
        const auto data = m_content.toByteArray();

        // Indian vaccination certificates
        if (data.startsWith(QByteArray::fromHex("504B0304")) && data.contains("certificate.json")) {
            return HealthCertificate;
        }

        if (isTransportTicket(data)) {
            return ContentType::TransportTicket;
        }

        return ContentType::Binary;
    }

    const auto text = m_content.toString();
    if (isUrl(text))
        return ContentType::Url;
    else if (isVCard(text))
        return ContentType::VCard;
    else if (isOtpToken(text))
        return ContentType::OtpToken;
    else if (isHealtCertificate(text))
        return ContentType::HealthCertificate;

    return ContentType::Text;
}

bool QrCodeContent::isPlainText() const
{
    return m_content.type() == QVariant::String;
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
