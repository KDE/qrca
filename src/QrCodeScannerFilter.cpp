/*
 *  SPDX-FileCopyrightText: 2016-2019 Kaidan developers and contributors (see the LICENSE file of Kaidan for a full list of copyright authors)
 *
 *  SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "QrCodeScannerFilter.h"

#include <QCamera>
#include <QCameraViewfinderSettings>
#include <QDebug>
#include <QtConcurrent/QtConcurrent>

QrCodeScannerFilter::QrCodeScannerFilter(QObject *parent)
    : QAbstractVideoFilter(parent)
    , m_decoder(new QrCodeDecoder(this))
{
    connect(m_decoder, &QrCodeDecoder::decodingFailed, this, &QrCodeScannerFilter::scanningFailed);
    connect(m_decoder, &QrCodeDecoder::decodingSucceeded, this, &QrCodeScannerFilter::scanningSucceeded);
}

QrCodeScannerFilter::~QrCodeScannerFilter()
{
    if (!m_processThread.isFinished()) {
        m_processThread.cancel();
        m_processThread.waitForFinished();
    }
}

QrCodeDecoder *QrCodeScannerFilter::decoder()
{
    return m_decoder;
}

QVideoFilterRunnable *QrCodeScannerFilter::createFilterRunnable()
{
    return new QrCodeScannerFilterRunnable(this);
}

void QrCodeScannerFilter::setCameraDefaultVideoFormat(QObject *qmlCamera)
{
    QCamera *camera = qvariant_cast<QCamera *>(qmlCamera->property("mediaObject"));
    if (camera) {
        QCameraViewfinderSettings settings = camera->viewfinderSettings();
        settings.setPixelFormat(QVideoFrame::Format_RGB24);
        camera->setViewfinderSettings(settings);
    } else {
        qWarning() << "Could not set pixel format of QML camera";
    }
}

QrCodeScannerFilterRunnable::QrCodeScannerFilterRunnable(QrCodeScannerFilter *filter)
    : QObject(nullptr)
    , m_filter(filter)
{
}

QVideoFrame QrCodeScannerFilterRunnable::run(QVideoFrame *input, const QVideoSurfaceFormat &, RunFlags)
{
    // Only one frame is processed at a time.
    if (input == nullptr || !input->isValid() || !m_filter->m_processThread.isFinished()) {
        return *input;
    }

    // Copy the data to be filtered.
    m_filter->m_frame.setData(*input);

    // Run a separate thread for processing the data.
    m_filter->m_processThread = QtConcurrent::run(this, &QrCodeScannerFilterRunnable::processVideoFrameProbed, m_filter->m_frame, m_filter);
    return *input;
}

void QrCodeScannerFilterRunnable::processVideoFrameProbed(QrCodeVideoFrame videoFrame, QrCodeScannerFilter *filter)
{
    // Return if the frame is empty.
    if (videoFrame.data().isEmpty())
        return;

    // Create an image from the frame.
    const QImage *image = videoFrame.toGrayscaleImage();

    // Return if conversion from the frame to the image failed.
    if (image->isNull()) {
        // dirty hack: write QVideoFrame::PixelFormat as string to format using QDebug
        //             QMetaEnum::valueToKey() did not work
        QString format;
        QDebug(&format).nospace() << videoFrame.pixelFormat();

        qDebug() << "QrCodeScannerFilterRunnable error: Cannot create image file to process.";
        qDebug() << "Maybe it was a format conversion problem.";
        qDebug() << "VideoFrame format:" << format;
        qDebug() << "Image corresponding format:" << QVideoFrame::imageFormatFromPixelFormat(videoFrame.pixelFormat());

        emit filter->unsupportedFormatReceived(format);
        return;
    }

    // Decode the image.
    m_filter->decoder()->decodeImage(*image);

    delete image;
}
