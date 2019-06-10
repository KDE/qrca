#include <QDebug>
#include <QImage>
#include <QRegularExpression>
#include <QStandardPaths>
#include <QDateTime>
#include <QDir>
#include <KLocalizedString>

#include <ZXing/BarcodeFormat.h>
#include <ZXing/MultiFormatWriter.h>
#include <ZXing/BitMatrix.h>

#include "QrSkanner.h"


QrSkanner::QrSkanner()
{
}

bool QrSkanner::isUrl(const QString &text) {
	QRegularExpression exp("(?:https?|ftp)://\\S+");

	return exp.match(text).hasMatch();
}

QImage QrSkanner::encode(const QString &text, const int &width) {
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

QString QrSkanner::save(const QImage &image) {
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
