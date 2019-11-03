#ifndef QRCA_H
#define QRCA_H

#include <QObject>
#include <QImage>

class Qrca : public QObject
{
	Q_OBJECT

public:
	enum ContentType {
		Url,
		VCard,
		OtpToken,
		Text
	};
	Q_ENUM(ContentType)

	Qrca();

	static Q_INVOKABLE ContentType identifyContentType(const QString &text);

	Q_INVOKABLE QImage encode(const QString &text, const int &width);
	static Q_INVOKABLE QString save(const QImage &image);
	static Q_INVOKABLE void saveVCard(const QString &text);
	static Q_INVOKABLE QString getVCardName(const QString &text);

signals:
	/**
	 * Show passive notification
	 */
	void passiveNotificationRequested(QString text);

private:
	static bool isUrl(const QString &text);
	static bool isVCard(const QString &text);
	static bool isOtpToken(const QString &text);
};

#endif // QRCA_H
