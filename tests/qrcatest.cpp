/*
 *  SPDX-FileCopyrightText: 2019 Jonah Brüchert <jbb@kaidan.im
 *  SPDX-FileCopyrightText: 2020 Nicolas Fella <nicolas.fella@gmx.de>
 *
 *  SPDX-License-Identifier: GPL-3.0-or-later
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
        auto type = QrCodeContent(QStringLiteral("https://kde.org"), ZXing::BarcodeFormat::QR_CODE).contentType();
        QCOMPARE(type, QrCodeContent::Url);
    }

    void isVCard()
    {
        auto type = QrCodeContent(QString::fromUtf8(VCARD), ZXing::BarcodeFormat::QR_CODE).contentType();
        QCOMPARE(type, QrCodeContent::VCard);
    }

    void isOtpToken()
    {
        auto type = QrCodeContent(QStringLiteral(
            "otpauth://totp/ACME%20Co:john.doe@email.com?"
            "secret=HXDMVJECJJWSRB3HWIZR4IFUGFTMXBOZ&"
            "issuer=ACME%20Co&algorithm=SHA1&digits=6&period=30"), ZXing::BarcodeFormat::QR_CODE).contentType();
        QCOMPARE(type, QrCodeContent::OtpToken);
    }

    void isText()
    {
        auto type = QrCodeContent(QStringLiteral("Hello World"), ZXing::BarcodeFormat::QR_CODE).contentType();
        QCOMPARE(type, QrCodeContent::Text);
    }

    void isBinary()
    {
        auto type = QrCodeContent(QByteArray("hello\0world"), ZXing::BarcodeFormat::QR_CODE).contentType();
        QCOMPARE(type, QrCodeContent::Binary);
    }

    void VCardName()
    {
        QCOMPARE(Qrca::getVCardName(VCARD), "Alice");
    }

    void isHealthCertificate()
    {
        const auto euDgc = QLatin1String(R"(HC1:6BF+70790T9WJWG.FKY*4GO0.O1CV2 O5 N2FBBRW1*70HS8WY04AC*WIFN0AHCD8KD97TK0F90KECTHGWJC0FDC:5AIA%G7X+AQB9746HS80:54IBQF60R6$A80X6S1BTYACG6M+9XG8KIAWNA91AY%67092L4WJCT3EHS8XJC$+DXJCCWENF6OF63W5NW6WF6%JC QE/IAYJC5LEW34U3ET7DXC9 QE-ED8%E.JCBECB1A-:8$96646AL60A60S6Q$D.UDRYA 96NF6L/5QW6307KQEPD09WEQDD+Q6TW6FA7C466KCN9E%961A6DL6FA7D46JPCT3E5JDLA7$Q6E464W5TG6..DX%DZJC6/DTZ9 QE5$CB$DA/D JC1/D3Z8WED1ECW.CCWE.Y92OAGY8MY9L+9MPCG/D5 C5IA5N9$PC5$CUZCY$5Y$527B+A4KZNQG5TKOWWD9FL%I8U$F7O2IBM85CWOC%LEZU4R/BXHDAHN 11$CA5MRI:AONFN7091K9FKIGIY%VWSSSU9%01FO2*FTPQ3C3F)");
        auto content = QrCodeContent(euDgc, ZXing::BarcodeFormat::QR_CODE);
        QCOMPARE(content.contentType(), QrCodeContent::HealthCertificate);
        QCOMPARE(content.isPlainText(), true);
    }

    void isTransportTicket()
    {
        const auto uic9183 = QByteArray::fromHex("2355543031303038303030303032302C02141AE7BBA505A4B31E0D0868FD709CE2A03BC71B62021469DCD3887C0357879A93FA86B3A4AAFBB2A1AA1F0000000030333734789C7D91BF4EC25014C691415D5C7C82262E50A99E7BFA9F0D6C2B0D14931608C84088686C4034A592E893B0B938CACAA283495FC041371FC673E58F98183BDC7EF73BBFFBDDD3D37AA764172C6000AA0C6080691B56B322E9A9B587E980080C99CA0CB06CCBE6A06B01029061329415EE142BE428CC0485685F715AD6A9DA5A0FC2450E1AF3B781AA6A6AE4A36F39D8ACD6CBDE5FB406F28AA638A605C0FB65AC7A1D5D750737D17938228BBA218A84CC458184B21426090D252631332086DB4082D731A0234C25A59693D960481B8D67395177D8BFB88DE28CD70D87D9FDA330BEA39AFE4FCD580B41C6FB68B885BC50F34F5C31708F8BA257132B9E98714A878EE33959A2A855542F93C76496BC44824FE241E827D3B37E47EAE40F08D0788BFCABF0FB66EA1B99A9F3692B34138D8F9E6A74B38C3FE772A3387916FCBDDFC1FC50A3CE2767CC7F4A3A4DCBFB24DE7A9B8C73EDD446AEFDBA18FDD3F6E7E6CE647CBF1BD2E66341F488E82D89E98AF802EAA2924B");
        auto content = QrCodeContent(uic9183, ZXing::BarcodeFormat::QR_CODE);
        QCOMPARE(content.contentType(), QrCodeContent::TransportTicket);
        QCOMPARE(content.isPlainText(), false);
    }

    void isGlobalTradeItemNumber()
    {
        auto content = QrCodeContent(QStringLiteral("9783827316608"), ZXing::BarcodeFormat::EAN13);
        QCOMPARE(content.contentType(), QrCodeContent::ISBN);
        content = QrCodeContent(QStringLiteral("4337256114837"), ZXing::BarcodeFormat::EAN13);
        QCOMPARE(content.contentType(), QrCodeContent::EAN);
        content = QrCodeContent(QStringLiteral("4388844146860"), ZXing::BarcodeFormat::EAN13);
        QCOMPARE(content.contentType(), QrCodeContent::EAN);
    }
};

QTEST_GUILESS_MAIN(QrcaTest)

#include "qrcatest.moc"
