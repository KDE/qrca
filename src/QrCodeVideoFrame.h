/*
 *  SPDX-FileCopyrightText: 2017 QZXing authors
 *  SPDX-FileCopyrightText: 2016-2019 Kaidan developers and contributors (see the LICENSE file of Kaidan for a full list of copyright authors)
 *
 *  SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef QRCODEVIDEOFRAME_H
#define QRCODEVIDEOFRAME_H

#include <QByteArray>
#include <QSize>
#include <QVideoFrame>
class QImage;

/**
 * video frame which may contain a QR code
 */
class QrCodeVideoFrame
{
public:
    /**
     * Instantiates an empty video frame
     */
    QrCodeVideoFrame()
        : m_size {0, 0}
        , m_pixelFormat {QVideoFrame::Format_Invalid}
    {
    }

    /**
     * Sets the frame.
     *
     * @param frame frame to be set
     */
    void setData(QVideoFrame &frame);

    /**
     * Converts a given image to a grayscale image.
     *
     * @return grayscale image
     */
    QImage *toGrayscaleImage();

    /**
     * @return content of the frame which may contain a QR code
     */
    QByteArray data() const;

    /**
     * @return size of the frame
     */
    QSize size() const;

    /**
     * @return format of the frame
     */
    QVideoFrame::PixelFormat pixelFormat() const;

private:
    QByteArray m_data;
    QSize m_size;
    QVideoFrame::PixelFormat m_pixelFormat;
};

#endif // QRCODEVIDEOFRAME_H
