/*
 *  SPDX-FileCopyrightText: 2019 Jonah Brüchert <jbb@kaidan.im>
 *  SPDX-FileCopyrightText: 2019 Simon Schmeisser <s.schmeisser@gmx.net>
 *  SPDX-FileCopyrightText: 2020 Nicolas Fella <nicolas.fella@gmx.de>
 *
 *  SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef QRCA_H
#define QRCA_H

#include <KAboutData>
#include <QImage>
#include <QObject>
#include <QUrl>
#include <QVariant>

class QrCodeContent;

class Qrca : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString encodeText READ encodeText WRITE setEncodeText NOTIFY encodeTextChanged)
    Q_PROPERTY(KAboutData aboutData READ aboutData WRITE setAboutData NOTIFY aboutDataChanged)

public:
    Qrca();

    Q_INVOKABLE QImage encode(const QString &text, const int &width) noexcept;
    static Q_INVOKABLE QUrl save(const QImage &image) noexcept;
    static Q_INVOKABLE void saveVCard(const QString &text) noexcept;
    static Q_INVOKABLE QString getVCardName(const QString &text) noexcept;
    Q_INVOKABLE void copyToClipboard(const QrCodeContent &content) noexcept;
    Q_INVOKABLE bool hasApplication(const QString &appId) const;
    Q_INVOKABLE void openInApplication(const QrCodeContent &content, const QString &appId);
    Q_INVOKABLE QString wifiName(const QString &wifiSetting) const;
    Q_INVOKABLE bool canConnectToWifi() const;
    Q_INVOKABLE void connectToWifi(const QString &wifiCode);

    QString encodeText() const noexcept;
    void setEncodeText(const QString &encodeText) noexcept;

    KAboutData aboutData() const noexcept;
    void setAboutData(const KAboutData &aboutData) noexcept;

signals:
    /**
     * Show passive notification
     */
    void passiveNotificationRequested(QString text);

    void encodeTextChanged();
    void aboutDataChanged();

private:
    QString m_encodeText;
    KAboutData m_aboutData;
};

#endif // QRCA_H
