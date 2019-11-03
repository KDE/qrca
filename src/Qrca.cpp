#include <QDebug>
#include <QImage>
#include <QRegularExpression>
#include <QStandardPaths>
#include <QDateTime>
#include <QDir>
#include <QCryptographicHash>
#include <QUrlQuery>

#include <KLocalizedString>
#include <KContacts/Addressee>
#include <KContacts/VCardConverter>

#include <ZXing/BarcodeFormat.h>
#include <ZXing/MultiFormatWriter.h>
#include <ZXing/BitMatrix.h>

#include "Qrca.h"


Qrca::Qrca()
{
}

bool Qrca::isUrl(const QString &text) {
	QRegularExpression exp("(?:https?|ftp)://\\S+");

	return exp.match(text).hasMatch();
}

bool Qrca::isVCard(const QString &text) {
	return (text.startsWith("BEGIN:VCARD") && text.trimmed().endsWith("END:VCARD"));
}

bool Qrca::isOtpToken(const QString &text)
{
	if (text.startsWith("otpauth")) {
		QUrl uri(text);
		if (uri.isValid() && (uri.host() == "totp" || uri.host() == "hotp")) {
			QUrlQuery query(uri.query());
			return query.hasQueryItem("secret");
		}
	}

	return false;
}

Qrca::ContentType Qrca::identifyContentType(const QString &text)
{
	if (isUrl(text))
		return ContentType::Url;
	else if (isVCard(text))
		return ContentType::VCard;
	else if (isOtpToken(text))
		return ContentType::OtpToken;

	return ContentType::Text;
}

void Qrca::saveVCard(const QString &text) {
	QString path = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation)
	               + ("/kpeoplevcard");

	QCryptographicHash hash(QCryptographicHash::Sha1);
	hash.addData(getVCardName(text).toUtf8());

	QFile file(path + "/" + hash.result().toHex() + ".vcf");

	if (!file.open(QFile::WriteOnly)) {
		qWarning() << "Couldn't save vCard: Couldn't open file for writing.";
	    return;
	}
	file.write(text.toUtf8(), text.toUtf8().length());
	file.close();
}

QString Qrca::getVCardName(const QString &text) {
	KContacts::VCardConverter converter;
	KContacts::Addressee adressee = converter.parseVCard(text.toUtf8());

	return adressee.realName();
}

QImage Qrca::encode(const QString &text, const int &width) {
	try {
		ZXing::MultiFormatWriter writer(ZXing::BarcodeFormat::QR_CODE);

		auto matrix = writer.encode(text.toStdWString(), width, width);

		QImage image(width, width, QImage::Format_ARGB32);

		for (int y = 0; y < matrix.height(); ++y) {
			for (int x = 0; x < matrix.width(); ++x) {
				image.setPixel(y, x, matrix.get(x, y) ? qRgb(0,0,0) : qRgb(255,255,255));
			}
		}

		return image;

	}
	catch (std::invalid_argument e) {
		passiveNotificationRequested(i18n("Generating the QR-Code failed: ") + QString(e.what()));
	}

	return(QImage());
}

QString Qrca::save(const QImage &image) {
	const QString directory = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation)
	                   + "/qrcodes/";

	const QString path = directory + QDateTime::currentDateTime().toString(Qt::ISODate) + ".png";

	QDir dir = QDir(directory);
	if (!dir.exists()) {
		dir.mkpath(directory);
	}

	qDebug() << "Saving image to" << path;
	image.save(path, "PNG", -1);

	return path;
}
