/*
 *  Copyright (C) 2019 Jonah Brüchert <jbb@kaidan.im>
 *
 *  Qrca is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Qrca is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this project.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QClipboard>
#include <QCryptographicHash>
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QGuiApplication>
#include <QImage>
#include <QRegularExpression>
#include <QStandardPaths>
#include <QUrlQuery>

#include <KContacts/Addressee>
#include <KContacts/VCardConverter>
#include <KLocalizedString>

#include <ZXing/BarcodeFormat.h>
#include <ZXing/BitMatrix.h>
#include <ZXing/MultiFormatWriter.h>

#include "Qrca.h"

Qrca::Qrca() = default;

bool Qrca::isUrl(const QString &text)
{
    QRegularExpression exp(QStringLiteral("(?:https?|ftp)://\\S+"));

    return exp.match(text).hasMatch();
}

bool Qrca::isVCard(const QString &text)
{
    return (text.startsWith(QLatin1String("BEGIN:VCARD")) && text.trimmed().endsWith(QLatin1String("END:VCARD")));
}

bool Qrca::isOtpToken(const QString &text)
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

Qrca::ContentType Qrca::identifyContentType(const QString &text) noexcept
{
    if (isUrl(text))
        return ContentType::Url;
    else if (isVCard(text))
        return ContentType::VCard;
    else if (isOtpToken(text))
        return ContentType::OtpToken;

    return ContentType::Text;
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

void Qrca::copyToClipboard(const QString &text) noexcept
{
    QClipboard *clipboard = QGuiApplication::clipboard();
    clipboard->setText(text);
}
