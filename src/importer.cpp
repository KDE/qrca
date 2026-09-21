/*
 * SPDX-FileCopyrightText: 2026 Kai Uwe Broulik <kde@broulik.de>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "importer.h"

#include <QImage>
#include <QImageReader>
#include <QtConcurrentRun>

#include <KIO/StoredTransferJob>

#include <Prison/ImageScanner>

using namespace Qt::StringLiterals;

Importer::Importer(QObject *parent)
    : QObject(parent)
{
}

Importer::~Importer() = default;

bool Importer::busy() const
{
    return m_busy;
}

void Importer::setBusy(bool busy)
{
    if (m_busy != busy) {
        m_busy = busy;
        Q_EMIT busyChanged(busy);
    }
}

void Importer::load(const QUrl &url)
{
    if (m_busy) {
        return;
    }

    setBusy(true);

    auto *job = KIO::storedGet(url);
    connect(job, &KIO::StoredTransferJob::result, this, [this, job] {
        if (job->error()) {
            if (job->error() == KIO::ERR_USER_CANCELED) {
                Q_EMIT canceled();
            } else {
                Q_EMIT failed(job->error(), job->errorString());
            }
            setBusy(false);
            return;
        }

        load(job->data());
    });
}

void Importer::load(const QByteArray &data)
{
    setBusy(true);

    QtConcurrent::run([data] {
        return QImage::fromData(data);
    }).then(this, [this](const QImage &image) {
        if (image.isNull()) {
            Q_EMIT failed(KIO::ERR_NO_CONTENT, QString());
            setBusy(false);
        } else {
            scanImage(image);
        }
    });
}

void Importer::scanImage(const QImage &image)
{
    Q_ASSERT(m_busy);

    QtConcurrent::run([image]() {
        return Prison::ImageScanner::scan(image);
    }).then(this, [this, image](const Prison::ScanResult &result) {
        Q_EMIT finished(image, result);
        setBusy(false);
    });
}

#include "moc_importer.cpp"
