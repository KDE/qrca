/*
 *  SPDX-FileCopyrightText: 2019 Jonah Brüchert <jbb@kaidan.im>
 *  SPDX-FileCopyrightText: 2019 Simon Schmeisser <s.schmeisser@gmx.net>
 *  SPDX-FileCopyrightText: 2020 Nicolas Fella <nicolas.fella@gmx.de>
 *
 *  SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <QClipboard>
#include <QCryptographicHash>
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QGuiApplication>
#include <QImage>
#include <QMimeData>
#include <QStandardPaths>

#include <KContacts/Addressee>
#include <KContacts/VCardConverter>
#include <KLocalizedString>

#include <ZXing/BarcodeFormat.h>
#include <ZXing/BitMatrix.h>
#include <ZXing/MultiFormatWriter.h>

#include "Qrca.h"
#include "QrCodeContent.h"

Qrca::Qrca() = default;

KAboutData Qrca::aboutData() const noexcept
{
    return m_aboutData;
}

void Qrca::setAboutData(const KAboutData &aboutData) noexcept
{
    m_aboutData = aboutData;

    Q_EMIT aboutDataChanged();
}

QString Qrca::encodeText() const noexcept
{
    return m_encodeText;
}

void Qrca::setEncodeText(const QString &encodeText) noexcept
{
    m_encodeText = encodeText;

    Q_EMIT encodeTextChanged();
}

void Qrca::saveVCard(const QString &text) noexcept
{
    QString path = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + QStringLiteral("/kpeoplevcard");

    QCryptographicHash hash(QCryptographicHash::Sha1);
    hash.addData(getVCardName(text).toUtf8());

    QFile file(path + QStringLiteral("/") + QString::fromLatin1(hash.result().toHex()) + QStringLiteral(".vcf"));

    if (!file.open(QFile::WriteOnly)) {
        qWarning() << "Couldn't save vCard: Couldn't open file for writing.";
        return;
    }
    file.write(text.toUtf8(), text.toUtf8().length());
    file.close();
}

QString Qrca::getVCardName(const QString &text) noexcept
{
    KContacts::VCardConverter converter;
    KContacts::Addressee adressee = converter.parseVCard(text.toUtf8());

    return adressee.realName();
}

QImage Qrca::encode(const QString &text, const int &width) noexcept
{
    try {
        const ZXing::MultiFormatWriter writer(ZXing::BarcodeFormat::QR_CODE);

        const auto matrix = writer.encode(text.toStdWString(), width, width);

        QImage image(width, width, QImage::Format_ARGB32);

        for (int y = 0; y < matrix.height(); ++y) {
            for (int x = 0; x < matrix.width(); ++x) {
                image.setPixel(y, x, matrix.get(x, y) ? qRgb(0, 0, 0) : qRgb(255, 255, 255));
            }
        }

        return image;
    } catch (const std::invalid_argument &e) {
        passiveNotificationRequested(i18n("Generating the QR-Code failed: ") + QString::fromLatin1(e.what()));
    };

    return {};
}

QUrl Qrca::save(const QImage &image) noexcept
{
    const QString directory = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation) + QStringLiteral("/qrcodes/");

    const QString path = directory + QDateTime::currentDateTime().toString(Qt::ISODate) + QStringLiteral(".png");

    const QDir dir(directory);
    if (!dir.exists()) {
        dir.mkpath(directory);
    }

    qDebug() << "Saving image to" << path;
    image.save(path, "PNG", -1);

    return QUrl::fromLocalFile(path);
}

void Qrca::copyToClipboard(const QrCodeContent &content) noexcept
{
    QClipboard *clipboard = QGuiApplication::clipboard();
    if (content.contentType() == QrCodeContent::Binary) {
        auto md = new QMimeData;
        md->setData(QStringLiteral("application/octet-stream"), content.binaryContent());
        clipboard->setMimeData(md);
    } else {
        clipboard->setText(content.text());
    }
}

