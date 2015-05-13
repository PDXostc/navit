#include "locationproxy.h"
#include "alog.h"

LocationProxy::LocationProxy(LocationType locType, QString itemText, bool fav, QString desc, bool bolded, int searchId, QObject* parent)
    : QObject(parent)
    , _locType(locType)
    , _itemText(itemText)
    , _favorite(fav)
    , _description(desc)
    , _bolded(bolded)
    , _searchId(searchId)
    , _id(QUuid::createUuid())
{
}

LocationProxy::LocationProxy(const NXE::SearchResult& searchResult)
    : _locType(LocationType::Address)
    , _itemText("")
    , _favorite(false)
    , _description()
    , _bolded(false)
    , _position(searchResult.position)
    , _searchId(searchResult.searchId)
    , _id(QUuid::createUuid())
{
    if (!searchResult.house.name.empty()) {
        _locType = LocationType::Address;
        _itemText = QString("%1 %2")
                .arg(QString::fromStdString(searchResult.street.name))
                .arg(QString::fromStdString(searchResult.house.name));

        _description = QString("%1 (%2), %3")
                .arg(QString::fromStdString(searchResult.city.name))
                .arg(QString::fromStdString(searchResult.city.postal))
                .arg(QString::fromStdString(searchResult.country.name));
    }
    else if (!searchResult.street.name.empty()) {
        _locType = LocationType::Street;
        _itemText = QString::fromStdString(searchResult.street.name);
        _description = QString("%1, %2").arg(QString::fromStdString(searchResult.city.name)).arg(QString::fromStdString(searchResult.country.name));
    }
    else if (!searchResult.city.name.empty()) {
        _locType = LocationType::City;
        _itemText = QString("%1 (%2)")
                        .arg(QString::fromStdString(searchResult.city.name))
                        .arg(QString::fromStdString(searchResult.city.postal));
    }
    else if (!searchResult.country.name.empty()) {
        _locType = LocationType::Country;
        _itemText = QString::fromStdString(searchResult.country.name);
    }
    else {
        throw std::runtime_error("What is this?");
    }
    _position = searchResult.position;
}

void LocationProxy::setFavorite(bool bFav)
{
    _favorite = bFav;
    aTrace() << "Location " << _itemText.toStdString() << " favorite= " << (_favorite ? " fav " : " not fav ");
    emit favoriteChanged();
}

void LocationProxy::setDescription(const QString& desc)
{
}

void LocationProxy::setBolded(bool b)
{
}

int LocationProxy::xPosition() const
{
    return _position.first;
}

int LocationProxy::yPosition() const
{
    return _position.second;
}

LocationProxy* LocationProxy::clone(LocationProxy* rhs)
{
    auto p = new LocationProxy{ rhs->_locType, rhs->itemText(),
        rhs->favorite(),
        rhs->description(),
        rhs->bolded() };

    p->_locType = rhs->_locType;
    p->_searchId = rhs->_searchId;
    p->_id = rhs->_id;
    return p;
}
