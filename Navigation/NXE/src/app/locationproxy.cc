#include "locationproxy.h"
#include "alog.h"

#include <QtCore/QDir>
#include <QtCore/QFile>

LocationProxy::LocationProxy(QString itemText, bool fav, QString desc, bool bolded, int searchId, int distance, const QString &uuid, QObject* parent)
    : QObject(parent)
    , _itemText(itemText)
    , _favorite(fav)
    , _description(desc)
    , _bolded(bolded)
    , _searchId(searchId)
    , _distance(distance)
    , _id(uuid.isEmpty() ? QUuid::createUuid() : uuid)
{
}

LocationProxy::LocationProxy(const NXE::SearchResult& searchResult)
    : _itemText("")
    , _favorite(false)
    , _description()
    , _bolded(false)
    , _coords(NXE::Position{searchResult.position.first, searchResult.position.second})
    , _searchId(searchResult.searchId)
    , _id(QUuid::createUuid())
{
    if (!searchResult.house.name.empty()) {
        _itemText = QString("%1 %2")
                .arg(QString::fromStdString(searchResult.street.name))
                .arg(QString::fromStdString(searchResult.house.name));

        _description = QString("%1, %2")
                .arg(QString::fromStdString(searchResult.city.name))
                .arg(QString::fromStdString(searchResult.country.name));
    }
    else if (!searchResult.street.name.empty()) {
        _itemText = QString::fromStdString(searchResult.street.name);
        _description = QString("%1, %2").arg(QString::fromStdString(searchResult.city.name)).arg(QString::fromStdString(searchResult.country.name));
    }
    else if (!searchResult.city.name.empty()) {
        _itemText = QString("%1")
                        .arg(QString::fromStdString(searchResult.city.name))
                        .arg(QString::fromStdString(searchResult.city.postal));
    }
    else if (!searchResult.country.name.empty()) {
        _itemText = QString::fromStdString(searchResult.country.name);
    }
    else {
        throw std::runtime_error("What is this?");
    }
}

void LocationProxy::setFavorite(bool bFav)
{
    _favorite = bFav;
    aTrace() << "Location " << _itemText.toStdString() << " favorite= " << (_favorite ? " fav " : " not fav ");
    if (_id.isNull()) {
        aError() << "Unable to store empty favorite";
    }

    emit favoriteChanged();
}

void LocationProxy::setBolded(bool b)
{
}

LocationProxy* LocationProxy::clone(LocationProxy* rhs)
{
    auto p = new LocationProxy{ rhs->itemText(),
        rhs->favorite(),
        rhs->description(),
        rhs->bolded(),
        rhs->searchId(),
        rhs->distance()};

    p->_searchId = rhs->_searchId;
    p->_id = rhs->_id;
    p->_coords = rhs->_coords;
    return p;
}

void LocationProxy::setDistance(int newDistance)
{
    _distance = newDistance;
    emit distanceChanged();
}

void LocationProxy::setEta(int newEta)
{
    aTrace() << "Setting eta to " << newEta;
    _eta = newEta;
    emit etaChanged();
}
