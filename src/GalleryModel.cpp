#include "GalleryModel.h"

#include <qrcastore.h>
#include <QrCodeContent.h>
#include <Qrca.h>

#include <KLocalizedString>

GalleryModel::GalleryModel(QObject *parent)
    : QAbstractListModel(parent)
{

}

QHash<int, QByteArray> GalleryModel::roleNames() const
{
    return {
        {Type, "type"},
        {Title, "title"},
        {Data, "data"}
    };
}

QVariant GalleryModel::data(const QModelIndex &index, int role) const
{
    auto data = QrcaStorage::self()->codes().at(index.row());
    auto code = QrCodeContent(data);

    switch (role) {
    case Type:
        return code.contentType();
    case Title:
        switch (code.contentType()) {
        case QrCodeContent::Binary:
            return i18n("<binary data>");
        case QrCodeContent::HealthCertificate:
            return i18n("Health Certificate");
        case QrCodeContent::OtpToken:
            return i18n("One Time Password");
        case QrCodeContent::Text:
            return code.text();
        case QrCodeContent::TransportTicket:
            return i18n("Transport Ticket");
        case QrCodeContent::VCard:
            return Qrca::getVCardName(code.text());
        case QrCodeContent::Url:
            return code.text();
        }
        break;
    case Data:
        return data;
    }

    return {};
}

int GalleryModel::rowCount(const QModelIndex &index) const
{
    return QrcaStorage::self()->codes().size();
}
