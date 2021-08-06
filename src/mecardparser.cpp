/*
    SPDX-FileCopyrightText: 2021 Volker Krause <vkrause@kde.org>
    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "mecardparser.h"

#include <QDebug>

MeCardParser::MeCardParser() = default;
MeCardParser::~MeCardParser() = default;

bool MeCardParser::Element::operator<(QStringView other) const
{
    return key < other;
}

bool MeCardParser::parse(const QString &data)
{
    m_header = {};
    m_elements.clear();
    m_data = data;

    const auto idx = data.indexOf(QLatin1Char(':'));
    if (idx <= 0 || idx >= data.size() - 1) {
        return false;
    }
    m_header = QStringView(data).left(idx);

    auto remaining = QStringView(data).mid(idx + 1);
    while (remaining.size() > 0) {
        const auto keyIdx = remaining.indexOf(QLatin1Char(':'));
        if (keyIdx <= 0 || keyIdx + 2 >= remaining.size()) {
            break;
        }

        QString value;
        auto elemIdx = keyIdx + 1;
        bool inQuote = false;
        for (; elemIdx < remaining.size() - 1; ++elemIdx) {
            auto c = remaining.at(elemIdx);
            if (elemIdx == (keyIdx + 1) && c == QLatin1Char('"')) { // leading quote
                inQuote = true;
                continue;
            }
            if (inQuote && c == QLatin1Char('"') && remaining.at(elemIdx + 1) == QLatin1Char(';')) { // trailing quote
                break;
            }
            if (c == QLatin1Char(';')) { // end of element
                break;
            }
            if (c == QLatin1Char('\\')) { // quoted character
                ++elemIdx;
                c = remaining.at(elemIdx);
            }
            value.push_back(c);
        }

        const auto key = remaining.left(keyIdx);
        auto it = std::lower_bound(m_elements.begin(), m_elements.end(), key);
        if (it == m_elements.end()) {
            m_elements.push_back(Element());
            it = std::prev(m_elements.end());
        } else if ((*it).key != key) {
            it = m_elements.insert(it, Element());
        }
        (*it).key = key;
        (*it).values.push_back(value);

        remaining = remaining.mid(elemIdx + 1);
    }

    return !m_elements.empty();
}

QStringView MeCardParser::header() const
{
    return m_header;
}

QString MeCardParser::value(QStringView key) const
{
    const auto it = std::lower_bound(m_elements.begin(), m_elements.end(), key);
    if (it != m_elements.end() && (*it).key == key && (*it).values.size() == 1) {
        return (*it).values.at(0);
    }
    return {};
}

QStringList MeCardParser::values(QStringView key) const
{
    const auto it = std::lower_bound(m_elements.begin(), m_elements.end(), key);
    if (it != m_elements.end() && (*it).key == key) {
        return (*it).values;
    }
    return {};
}
