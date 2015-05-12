#include "locationproxy.h"
#include "alog.h"

LocationProxy::LocationProxy(LocationType locType, QString itemText, bool fav, QString desc, bool bolded, QObject* parent)
    : QObject(parent)
    , _locType(locType)
    , _itemText(itemText)
    , _favorite(fav)
    , _description(desc)
    , _bolded(bolded)
{
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
    auto p = new LocationProxy{ rhs->_locType,rhs->itemText(),
        rhs->favorite(),
        rhs->description(),
        rhs->bolded() };

    p->_locType = rhs->_locType;
}
