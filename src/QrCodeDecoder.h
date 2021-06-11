/*
 *  SPDX-FileCopyrightText: 2016-2019 Kaidan developers and contributors (see the LICENSE file of Kaidan for a full list of copyright authors)
 *
 *  SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef QRCODEDECODER_H
#define QRCODEDECODER_H

#include <QObject>

class QrCodeContent;

/**
 * Decoder for QR codes. This is a backend for \c QrCodeScanner .
 */
class QrCodeDecoder : public QObject
{
    Q_OBJECT

public:
    /**
     * Instantiates a QR code decoder.
     *
     * @param parent parent object
     */
    explicit QrCodeDecoder(QObject *parent = nullptr);

signals:
    /**
     * Emitted when the decoding failed.
     */
    void decodingFailed();

    /**
     * Emitted when the decoding succeeded.
     *
     * @param tag barcode content from the QR code decoder
     */
    void decodingSucceeded(const QrCodeContent &tag);

public slots:
    /**
     * Tries to decode the QR code from the given image. When decoding has
     * finished @c decodingFinished() will be emitted. In case a QR code was found,
     * also @c tagFound() will be emitted.
     *
     * @param image image which may contain a QR code to decode to a string.
     *        It needs to be in grayscale format (one byte per pixel).
     */
    void decodeImage(const QImage &image);
};

#endif // QRCODEDECODER_H
