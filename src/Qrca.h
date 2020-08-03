/*
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

#ifndef QRCA_H
#define QRCA_H

#include <QObject>
#include <QImage>
#include <QVariant>
#include <QUrl>

class Qrca : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString encodeText READ encodeText WRITE setEncodeText NOTIFY encodeTextChanged)
	Q_PROPERTY(QVariant aboutData READ aboutData WRITE setAboutData NOTIFY aboutDataChanged)

public:
	enum ContentType {
		Url,
		VCard,
		OtpToken,
		Text
	};
	Q_ENUM(ContentType)

	Qrca();

	static Q_INVOKABLE ContentType identifyContentType(const QString &text) noexcept;

	Q_INVOKABLE QImage encode(const QString &text, const int &width) noexcept ;
	static Q_INVOKABLE QUrl save(const QImage &image) noexcept;
	static Q_INVOKABLE void saveVCard(const QString &text) noexcept;
	static Q_INVOKABLE QString getVCardName(const QString &text) noexcept;

	QString encodeText() const noexcept;
	void setEncodeText(const QString &encodeText) noexcept;

	QVariant aboutData() const noexcept;
	void setAboutData(const QVariant &aboutData) noexcept;

signals:
	/**
	 * Show passive notification
	 */
	void passiveNotificationRequested(QString text);

	void encodeTextChanged();
	void aboutDataChanged();

private:
	static bool isUrl(const QString &text);
	static bool isVCard(const QString &text);
	static bool isOtpToken(const QString &text);

	QString m_encodeText;
	QVariant m_aboutData;
};

#endif // QRCA_H
