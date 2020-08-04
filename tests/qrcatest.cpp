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

#include <QtTest/QTest>
#include <QImage>

#include "Qrca.h"

constexpr auto VCARD = "BEGIN:VCARD\n" \
                       "VERSION:3.0\n" \
                       "NAME:Alice\n"\
                       "TEL;TYPE=CELL:+12 3456 789\n" \
                       "UID:H3ll0-W0RLD-1M-A-VC4RD\n" \
                       "END:VCARD";

class QrcaTest : public QObject
{
	Q_OBJECT

private slots:
	void isUrl() {
		Qrca::ContentType type = Qrca::identifyContentType("https://kde.org");
		QCOMPARE(type, Qrca::Url);
	}

	void isVCard() {
		Qrca::ContentType type = Qrca::identifyContentType(VCARD);
		QCOMPARE(type, Qrca::VCard);
	}

	void isOtpToken() {
		Qrca::ContentType type = Qrca::identifyContentType("otpauth://totp/ACME%20Co:john.doe@email.com?"
		                                                   "secret=HXDMVJECJJWSRB3HWIZR4IFUGFTMXBOZ&"
		                                                   "issuer=ACME%20Co&algorithm=SHA1&digits=6&period=30");
		QCOMPARE(type, Qrca::OtpToken);
	}

	void isText() {
		Qrca::ContentType type = Qrca::identifyContentType("Hello World");
		QCOMPARE(type, Qrca::Text);
	}

	void VCardName() {
		QCOMPARE(Qrca::getVCardName(VCARD), "Alice");
	}
};

QTEST_GUILESS_MAIN(QrcaTest);

#include "qrcatest.moc"
