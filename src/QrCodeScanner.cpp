/*
 *  Kaidan - A user-friendly XMPP client for every device!
 *
 *  Copyright (C) 2016-2019 Kaidan developers and contributors
 *  (see the LICENSE file for a full list of copyright authors)
 *
 *  Kaidan is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  In addition, as a special exception, the author of Kaidan gives
 *  permission to link the code of its release with the OpenSSL
 *  project's "OpenSSL" library (or with modified versions of it that
 *  use the same license as the "OpenSSL" library), and distribute the
 *  linked executables. You must obey the GNU General Public License in
 *  all respects for all of the code used other than "OpenSSL". If you
 *  modify this file, you may extend this exception to your version of
 *  the file, but you are not obligated to do so.  If you do not wish to
 *  do so, delete this exception statement from your version.
 *
 *  Kaidan is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Kaidan.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "QrCodeScanner.h"

// C++
#include <vector>
// Qt
#include <QBuffer>
#include <QDebug>
#include <QFile>
#include <QImage>
#include <QTimer>
#include <QCamera>
#include <QCameraImageCapture>
// ZXing-cpp
#include <ZXing/DecodeHints.h>
#include <ZXing/BarcodeFormat.h>
#include <ZXing/GenericLuminanceSource.h>
#include <ZXing/HybridBinarizer.h>
#include <ZXing/MultiFormatReader.h>
#include <ZXing/Result.h>
#include <ZXing/TextUtfEncoding.h>

using namespace ZXing;

#define QR_CODE_RESCAN_INTERVAL 1000

QrCodeScanner::QrCodeScanner(QObject* parent)
        : QObject(parent), m_qmlCamera(nullptr)
{
	connect(this, &QrCodeScanner::qmlCameraChanged,
	        this, &QrCodeScanner::handleQmlCameraChanged);
}

void QrCodeScanner::processImage(const QImage& image)
{
	// convert to RGB24 (RGB/8-bit per channel) format
	std::vector<unsigned char> imageBytes;
	int pixels = image.width() * image.height();
	for (int i = 0; i < pixels; i++) {
		QColor color = image.pixelColor(i % image.width(), i / image.width());
		imageBytes.emplace_back(static_cast<unsigned char>(color.red()));
		imageBytes.emplace_back(static_cast<unsigned char>(color.green()));
		imageBytes.emplace_back(static_cast<unsigned char>(color.blue()));
	}

	DecodeHints decodeHints;
	decodeHints.setShouldTryRotate(true);

	MultiFormatReader reader(decodeHints);
	GenericLuminanceSource source(
	        image.width(),
	        image.height(),
	        imageBytes.data(),
	        image.width() * 3,
	        3,
	        0,
	        1,
	        2
	);

	HybridBinarizer binImage(std::shared_ptr<LuminanceSource>(&source, [](void*) {}));

	auto result = reader.read(binImage);

	if (!result.isValid()) {
		QTimer::singleShot(QR_CODE_RESCAN_INTERVAL, sender(), [this]() {
			emit scanningFailed();
		});
	} else {
		emit scanningSucceeded(
		        QString::fromStdString(TextUtfEncoding::ToUtf8(result.text()))
		);
	}
}

void QrCodeScanner::handleReadyForCaptureChanged(bool ready)
{
	if (ready) {
		startScanningLoop(static_cast<QCameraImageCapture*>(sender()));
		disconnect(
		        static_cast<QCameraImageCapture*>(sender()),
		        &QCameraImageCapture::readyForCaptureChanged,
		        this,
		        &QrCodeScanner::handleReadyForCaptureChanged
		);
	}
}

void QrCodeScanner::handleQmlCameraChanged()
{
	QCamera* camera = qvariant_cast<QCamera*>(m_qmlCamera->property("mediaObject"));
	auto* imageCapture = new QCameraImageCapture(camera, this);

	if (imageCapture->isReadyForCapture()) {
		startScanningLoop(imageCapture);
	} else {
		connect(imageCapture, &QCameraImageCapture::readyForCaptureChanged,
		        this, &QrCodeScanner::handleReadyForCaptureChanged);
	}
}

void QrCodeScanner::startScanningLoop(QCameraImageCapture* imageCapture)
{
	imageCapture->setCaptureDestination(QCameraImageCapture::CaptureToBuffer);
	imageCapture->setBufferFormat(QVideoFrame::Format_RGB24);

	imageCapture->capture();
	connect(imageCapture, &QCameraImageCapture::imageCaptured, this,
	        [this](int, const QImage &preview) {
		processImage(preview);
	});

	connect(this, &QrCodeScanner::scanningFailed, imageCapture, [imageCapture]() {
		imageCapture->capture();
	});
	connect(this, &QrCodeScanner::scanningSucceeded, imageCapture,
	        [imageCapture](const QString&) {
		imageCapture->capture();
	});
}

QObject* QrCodeScanner::qmlCamera() const
{
	return m_qmlCamera;
 }

void QrCodeScanner::setQmlCamera(QObject* qmlCamera)
{
	m_qmlCamera = qmlCamera;
	emit qmlCameraChanged();
}
