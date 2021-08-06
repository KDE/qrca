/*
    SPDX-FileCopyrightText: 2021 Volker Krause <vkrause@kde.org>
    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef MECARDPARSER_H
#define MECARDPARSER_H

#include <QString>
#include <QStringList>
#include <vector>

/** Parser for the MeCard format.
 *  This was originally used for a more compact vCard representation, but today
 *  is mostly relevant for Wifi configuration QR codes.
 *  @see https://en.wikipedia.org/wiki/MeCard_(QR_code)
 *  @see https://github.com/zxing/zxing/wiki/Barcode-Contents#wi-fi-network-config-android-ios-11
 */
class MeCardParser
{
public:
    explicit MeCardParser();
    ~MeCardParser();

    bool parse(const QString &data);

    QStringView header() const;
    QString value(QStringView key) const;
    QStringList values(QStringView key) const;

private:
    QString m_data;
    QStringView m_header;
    struct Element {
        QStringView key;
        QStringList values;
        bool operator<(QStringView other) const;
    };
    std::vector<Element> m_elements;
};

#endif // MECARDPARSER_H
