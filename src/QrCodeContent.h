/*
    SPDX-FileCopyrightText: 2021 Volker Krause <vkrause@kde.org>
    SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef QRCODECONTENT_H
#define QRCODECONTENT_H

#include <QVariant>

/** Result of a barcode scan.
 *  Can contain either text or binary data.
 */
class QrCodeContent
{
    Q_GADGET
    Q_PROPERTY(ContentType contentType READ contentType)
    Q_PROPERTY(QString text READ text)
public:
    QrCodeContent();
    explicit QrCodeContent(const QByteArray &content);
    explicit QrCodeContent(const QString &content);
    ~QrCodeContent();

    enum ContentType { Url, VCard, OtpToken, Text, Binary };
    Q_ENUM(ContentType)
    ContentType contentType() const;

    QString text() const;
    QByteArray binaryContent() const;

private:
    QVariant m_content;
};

Q_DECLARE_METATYPE(QrCodeContent)

#endif
