#ifndef LOCATIONPROXY_H
#define LOCATIONPROXY_H

#include <QObject>
#include "position.h"
#include "inavitipc.h"
#include <QtCore/QUuid>

enum class LocationType {
    Country = 0,
    City,
    Street,
    Address
};

class LocationProxy : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString itemText READ itemText CONSTANT)
    Q_PROPERTY(bool favorite READ favorite WRITE setFavorite NOTIFY favoriteChanged)
    Q_PROPERTY(QString description READ description CONSTANT)
    Q_PROPERTY(bool bolded READ bolded WRITE setBolded NOTIFY boldedChanged)
    Q_PROPERTY(int searchId READ searchId CONSTANT)
    Q_PROPERTY(double longitude READ longitude CONSTANT)
    Q_PROPERTY(double latitude READ latitude CONSTANT)
    Q_PROPERTY(QUuid itemId READ id CONSTANT)
public:
    LocationProxy(
        QString itemText, bool fav, QString desc, bool bolded, int searchID = -1, QObject* parent = 0);

    LocationProxy(const NXE::SearchResult& searchResult);

    QString itemText() const { return _itemText; }

    bool favorite() const { return _favorite; }
    void setFavorite(bool bFav);

    QString description() const { return _description; }

    bool bolded() const { return _bolded; }
    void setBolded(bool b);

    int searchId() const { return _searchId; }

    static LocationProxy* clone(LocationProxy* rhs);

    QUuid id() const {return _id;}

    double longitude() {return _coords.longitude;}
    double latitude() {return _coords.latitude;}

    void setPosition(const NXE::Position& p) {_coords = p;}

signals:

    void favoriteChanged();
    void descriptionChanged();
    void boldedChanged();

private:
    LocationType _locType;
    QString _itemText;
    bool _favorite;
    QString _description;
    bool _bolded;
    NXE::Position _coords;
    int _searchId;
    QUuid _id;
};

#endif // LOCATIONPROXY_H
