/*
 *  SPDX-FileCopyrightText: 2019 Jonah Brüchert <jbb@kaidan.im>
 *  SPDX-FileCopyrightText: 2019 Simon Schmeisser <s.schmeisser@gmx.net>
 *  SPDX-FileCopyrightText: 2020 Nicolas Fella <nicolas.fella@gmx.de>
 *  SPDX-FileCopyrightText: 2025 Kai Uwe Broulik <kde@broulik.de>
 *
 *  SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef QRCA_H
#define QRCA_H

#include <Prison/ScanResult>
#include <QImage>
#include <QObject>
#include <QUrl>
#include <QVariant>

class QrCodeContent;

class Qrca : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString encodeText READ encodeText WRITE setEncodeText NOTIFY encodeTextChanged)

    /**
     * Hide most UI elements and quit after connecting to the added Wifi network.
     */
    Q_PROPERTY(bool wifiMode READ wifiMode WRITE setWifiMode NOTIFY wifiModeChanged)
    Q_PROPERTY(bool connectingToWifi READ connectingToWifi NOTIFY connectingToWifiChanged)

public:
    Qrca();

    Q_INVOKABLE QString newQrCodeSaveLocation() noexcept;
    static Q_INVOKABLE void saveVCard(const QString &text) noexcept;
    static Q_INVOKABLE QString getVCardName(const QString &text) noexcept;
    Q_INVOKABLE void copyToClipboard(const QrCodeContent &content) noexcept;
    Q_INVOKABLE void copyImageToClipboard(const QImage &image);
    Q_INVOKABLE bool hasApplication(const QString &appId) const;
    Q_INVOKABLE QString applicationIconName(const QString &appId) const;
    Q_INVOKABLE void openInApplication(const QrCodeContent &content, const QString &appId);
    Q_INVOKABLE QString wifiName(const QString &wifiSetting) const;
    Q_INVOKABLE bool canConnectToWifi() const;
    Q_INVOKABLE void connectToWifi(const QString &wifiCode);
    Q_INVOKABLE QrCodeContent resultContent(const Prison::ScanResult &result);
    Q_INVOKABLE Prison::ScanResult scanImage(const QUrl &imagePath);

    QString encodeText() const noexcept;
    void setEncodeText(const QString &encodeText) noexcept;

    bool wifiMode() const noexcept;
    void setWifiMode(bool wifiMode) noexcept;
    Q_SIGNAL void wifiModeChanged();

    bool connectingToWifi() const noexcept;
    Q_SIGNAL void connectingToWifiChanged(bool connectingToWifi);

signals:
    /**
     * Show passive notification
     */
    void passiveNotificationRequested(QString text);

    void encodeTextChanged();

    void wifiConnected();
    void wifiConnectionFailed(const QString &errorMsg);

private:
    void setConnectingToWifi(bool connectingToWifi) noexcept;
    QString m_encodeText;

    bool m_wifiMode = false;
    bool m_connectingToWifi = false;
};

#endif // QRCA_H
