/*
 *  Copyright (C) 2019 Jonah Brüchert <jbb@kaidan.im
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

#include <QImage>
#include <QtTest/QTest>

#include "Qrca.h"
#include "QrCodeContent.h"

constexpr auto VCARD =
    "BEGIN:VCARD\n"
    "VERSION:3.0\n"
    "NAME:Alice\n"
    "TEL;TYPE=CELL:+12 3456 789\n"
    "UID:H3ll0-W0RLD-1M-A-VC4RD\n"
    "END:VCARD";

class QrcaTest : public QObject
{
    Q_OBJECT

private slots:
    void isUrl()
    {
        auto type = QrCodeContent(QStringLiteral("https://kde.org")).contentType();
        QCOMPARE(type, QrCodeContent::Url);
    }

    void isVCard()
    {
        auto type = QrCodeContent(QString::fromUtf8(VCARD)).contentType();
        QCOMPARE(type, QrCodeContent::VCard);
    }

    void isOtpToken()
    {
        auto type = QrCodeContent(QStringLiteral(
            "otpauth://totp/ACME%20Co:john.doe@email.com?"
            "secret=HXDMVJECJJWSRB3HWIZR4IFUGFTMXBOZ&"
            "issuer=ACME%20Co&algorithm=SHA1&digits=6&period=30")).contentType();
        QCOMPARE(type, QrCodeContent::OtpToken);
    }

    void isText()
    {
        auto type = QrCodeContent(QStringLiteral("Hello World")).contentType();
        QCOMPARE(type, QrCodeContent::Text);
    }

    void isBinary()
    {
        auto type = QrCodeContent(QByteArray("hello\0world")).contentType();
        QCOMPARE(type, QrCodeContent::Binary);
    }

    void VCardName()
    {
        QCOMPARE(Qrca::getVCardName(VCARD), "Alice");
    }
};

QTEST_GUILESS_MAIN(QrcaTest)

#include "qrcatest.moc"
