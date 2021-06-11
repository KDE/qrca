/*
 *  SPDX-FileCopyrightText: 2016-2019 Kaidan developers and contributors (see the LICENSE file of Kaidan for a full list of copyright authors)
 *
 *  SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "config-qrca.h"

#include "QrCodeContent.h"
#include "QrCodeDecoder.h"
// Qt
#include <QColor>
#include <QImage>
// ZXing-cpp
#if ZXING_VERSION < QT_VERSION_CHECK(1, 1, 0)
#include <ZXing/BarcodeFormat.h>
#include <ZXing/DecodeHints.h>
#include <ZXing/GenericLuminanceSource.h>
#include <ZXing/HybridBinarizer.h>
#include <ZXing/LuminanceSource.h>
#include <ZXing/MultiFormatReader.h>
#include <ZXing/Result.h>
#else
#include <ZXing/ReadBarcode.h>
#endif
#include <ZXing/TextUtfEncoding.h>

using namespace ZXing;

QrCodeDecoder::QrCodeDecoder(QObject *parent)
    : QObject(parent)
{
}

void QrCodeDecoder::decodeImage(const QImage &image)
{
    // options for decoding
    DecodeHints decodeHints;

#if ZXING_VERSION < QT_VERSION_CHECK(1, 1, 0)
    // Advise the decoder to also decode rotated QR codes.
    decodeHints.setTryRotate(true);

    // Advise the decoder to only decode QR codes.
    std::vector<BarcodeFormat> allowedFormats;
    allowedFormats.emplace_back(BarcodeFormat::QR_CODE);
    allowedFormats.emplace_back(BarcodeFormat::AZTEC);
    decodeHints.setPossibleFormats(allowedFormats);

    MultiFormatReader reader(decodeHints);

    // Create an image source to be decoded later.
    GenericLuminanceSource source(image.width(), image.height(), image.bits(), image.width(), 1, 0, 1, 2);

    // Create an image source specific for decoding black data on white background.
    HybridBinarizer binImage(std::shared_ptr<LuminanceSource>(&source, [](void *) {}));

    // Decode the specific image source.
    const auto result = reader.read(binImage);
#else
#if ZXING_VERSION < QT_VERSION_CHECK(1, 2, 0)
    // emulate BarcodeFormat::Any, which was only introduced in 1.2.0
    decodeHints.setFormats(static_cast<BarcodeFormat>(static_cast<int>(BarcodeFormat::_max) - 1));
#else
    decodeHints.setFormats(BarcodeFormat::Any);
#endif

    const auto result = ReadBarcode({image.bits(), image.width(), image.height(), ZXing::ImageFormat::Lum, image.bytesPerLine()}, decodeHints);
#endif

    // If a QR code could be found and decoded, emit a signal with the decoded string.
    // Otherwise, emit a signal for failed decoding.
    if (result.isValid()) {
        const auto hasWideChars = std::any_of(result.text().begin(), result.text().end(), [](auto c) { return c > 255; });
        const auto hasControlChars = std::any_of(result.text().begin(), result.text().end(), [](auto c) { return c < 20; });
        if (hasWideChars || !hasControlChars) {
            emit decodingSucceeded(QrCodeContent(QString::fromStdString(TextUtfEncoding::ToUtf8(result.text()))));
        } else {
            QByteArray b;
            b.resize(result.text().size());
            std::copy(result.text().begin(), result.text().end(), b.begin());
            emit decodingSucceeded(QrCodeContent(b));
        }
    } else {
        emit decodingFailed();
    }
}
