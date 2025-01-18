/*
    SPDX-FileCopyrightText: 2021 Volker Krause <vkrause@kde.org>
    SPDX-FileCopyrightText: 2019 Jonah Brüchert <jbb@kaidan.im>
    SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "QrCodeContent.h"

#include <Prison/MeCard>

#include <QRegularExpression>
#include <QUrlQuery>

QrCodeContent::QrCodeContent() = default;

QrCodeContent::QrCodeContent(const QByteArray &content, Prison::Format::BarcodeFormat format)
    : m_content(content)
    , m_format(format)
{
}

QrCodeContent::QrCodeContent(const QString &content, Prison::Format::BarcodeFormat format)
    : m_content(content)
    , m_format(format)
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

static constexpr const char base45Table[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ $%*+-./:";

static bool isBase45(const QString &text)
{
    return std::all_of(text.begin(), text.end(), [](QChar c) {
        return c.row() == 0 && std::find(std::begin(base45Table), std::end(base45Table), c.cell()) != std::end(base45Table);
    });
}

static bool isHealtCertificate(const QString &text)
{
    return
        // EU DGC, NL COVID-19 CoronaCheck
        (text.size() > 400 && (text.startsWith(QLatin1String("HC1:")) || text.startsWith(QLatin1String("NL2:"))) && isBase45(text)) ||
        // SMART Health Cards (SHC)
        (text.size() > 1000 && text.startsWith(QLatin1String("shc:/")) && std::all_of(text.begin() + 10, text.end(), [](QChar c) {
            return c.row() == 0 && c.cell() >= '0' && c.cell() <= '9';
        }))
    ;
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

static bool isTransportTicket(const QString &text)
{
    // IATA BCBP
    if (text.size() >= 47 && text[0] == QLatin1Char('M') && text[1].digitValue() >= 1 && text[1].digitValue() <= 4
        && std::all_of(text.begin(), text.end(), [](QChar c) { return c.row() == 0; })
        && std::all_of(text.begin() + 30, text.begin() + 36, [](QChar c) { return c.isLetter() && c.isUpper(); })) {
        return true;
    }

    return false;
}

static bool isWifiSetting(const QString &text)
{
    auto p = Prison::MeCard::parse(text);
    return p && p->header().compare(QLatin1String("wifi"), Qt::CaseInsensitive) == 0;
}

// https://en.wikipedia.org/wiki/Global_Trade_Item_Number
// https://en.wikipedia.org/wiki/International_Standard_Book_Number
// https://en.wikipedia.org/wiki/International_Article_Number
// https://en.wikipedia.org/wiki/List_of_GS1_country_codes
static bool isGlobalTradeItemNumber(const QString &text)
{
    if (text.size() != 13 || std::any_of(text.begin(), text.end(), [](auto c) { return c.row() != 0 || !c.isDigit(); })) {
        return false;
    }

    int checkSum = 0;
    for (int i = 0; i < text.size() - 1; ++i) {
        checkSum += text[i].digitValue() * (i % 2 == 1 ? 3 : 1);
    }
    return text.back().digitValue() == ((10 - (checkSum % 10)) % 10);
}

QrCodeContent::ContentType QrCodeContent::contentType() const
{
    if (m_content.userType() == QMetaType::QByteArray) {
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
    else if (m_format == Prison::Format::EAN13 && isGlobalTradeItemNumber(text)) {
        if (text.startsWith(QLatin1String("978"))) {
            return ContentType::ISBN;
        }
        if (text.startsWith(QLatin1String("950")) || text.startsWith(QLatin1String("979"))) { // ISSN / ISMN
            return ContentType::Text;
        }
        return ContentType::EAN;
    } else if (isTransportTicket(text)) {
        return ContentType::TransportTicket;
    } else if (isWifiSetting(text)) {
        return ContentType::WifiSetting;
    }

    return ContentType::Text;
}

bool QrCodeContent::isPlainText() const
{
    return m_content.userType() == QMetaType::QString;
}

QString QrCodeContent::text() const
{
    if (m_content.userType() == QMetaType::QString) {
        return m_content.toString();
    }
    return {};
}

QByteArray QrCodeContent::binaryContent() const
{
    if (m_content.userType() == QMetaType::QByteArray) {
        return m_content.toByteArray();
    }
    return {};
}

#include "moc_QrCodeContent.cpp"
