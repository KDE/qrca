/*
    SPDX-FileCopyrightText: 2021 Volker Krause <vkrause@kde.org>
    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include <QtTest/QTest>

#include "mecardparser.h"

class MeCardParserTest : public QObject
{
    Q_OBJECT

private slots:
    void testWifiParse()
    {
        MeCardParser p;
        QVERIFY(p.parse(QStringLiteral("WIFI:S:myssid;T:WPA;P:xxx123;;")));
        QCOMPARE(p.header(), QLatin1String("WIFI"));
        QCOMPARE(p.value(u"S"), QLatin1String("myssid"));
        QCOMPARE(p.value(u"T"), QLatin1String("WPA"));
        QCOMPARE(p.value(u"P"), QLatin1String("xxx123"));

        QVERIFY(p.parse(QStringLiteral("MECARD:N:Doe,John;TEL:13035551212;EMAIL:john.doe@example.com;;")));
        QCOMPARE(p.header(), QLatin1String("MECARD"));
        QCOMPARE(p.value(u"N"), QLatin1String("Doe,John"));
        QCOMPARE(p.value(u"TEL"), QLatin1String("13035551212"));
        QCOMPARE(p.value(u"EMAIL"), QLatin1String("john.doe@example.com"));

        QVERIFY(p.parse(QStringLiteral("MECARD:N:Doe,John;TEL:13035551212;EMAIL:john.doe@example.com;EMAIL:null@kde.org;;")));
        QCOMPARE(p.header(), QLatin1String("MECARD"));
        QCOMPARE(p.value(u"N"), QLatin1String("Doe,John"));
        QCOMPARE(p.value(u"TEL"), QLatin1String("13035551212"));
        QCOMPARE(p.value(u"EMAIL"), QString());
        QCOMPARE(p.values(u"EMAIL"), QStringList({QLatin1String("john.doe@example.com"), QLatin1String("null@kde.org")}));

        QVERIFY(p.parse(QStringLiteral("WIFI:S:\\\"foo\\;bar\\\\baz\\\";P:\"ABCD\";;")));
        QCOMPARE(p.value(u"P"), QLatin1String("ABCD"));
        QCOMPARE(p.value(u"S"), QLatin1String("\"foo;bar\\baz\""));
    }

    void testInvalid_data()
    {
        QTest::addColumn<QString>("input");
        QTest::newRow("empty") << QString();
        QTest::newRow("no colon") << QStringLiteral("wifi");
        QTest::newRow("no content 1") << QStringLiteral("wifi:");
        QTest::newRow("no content 2") << QStringLiteral("wifi:;");
        QTest::newRow("no content 3") << QStringLiteral("wifi:;;");
        QTest::newRow("invalid element") << QStringLiteral("wifi:S:");
    }
    void testInvalid()
    {
        QFETCH(QString, input);
        MeCardParser p;
        QVERIFY(!p.parse(input));
    }
};

QTEST_APPLESS_MAIN(MeCardParserTest)

#include "mecardparsertest.moc"
