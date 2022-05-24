/*
    SPDX-FileCopyrightText: 2021 Volker Krause <vkrause@kde.org>
    SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef QRCODECONTENT_H
#define QRCODECONTENT_H

#include <QVariant>

#include <Prison/Format>

/** Result of a barcode scan.
 *  Can contain either text or binary data.
 */
class QrCodeContent
{
    Q_GADGET
    Q_PROPERTY(ContentType contentType READ contentType)
    Q_PROPERTY(QString text READ text)
    Q_PROPERTY(bool isPlainText READ isPlainText)
public:
    QrCodeContent();
    explicit QrCodeContent(const QByteArray &content, Prison::Format::BarcodeFormat format);
    explicit QrCodeContent(const QString &content, Prison::Format::BarcodeFormat format);
    ~QrCodeContent();

    enum ContentType {
        Url,
        VCard,
        OtpToken,
        TransportTicket,
        HealthCertificate,
        Text,
        Binary,
        ISBN,
        EAN,
        WifiSetting,
    };
    Q_ENUM(ContentType)
    ContentType contentType() const;

    /** @c true if the content consists only of plain text, which might also be the case for contentType() != Text. */
    bool isPlainText() const;
    QString text() const;
    QByteArray binaryContent() const;

private:
    QVariant m_content;
    Prison::Format::BarcodeFormat m_format;
};

Q_DECLARE_METATYPE(QrCodeContent)

#endif
