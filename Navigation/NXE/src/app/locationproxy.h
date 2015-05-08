#ifndef LOCATIONPROXY_H
#define LOCATIONPROXY_H

#include <QObject>
#include "position.h"

class LocationProxy : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString itemText READ itemText CONSTANT)
    Q_PROPERTY(bool favorite READ favorite WRITE setFavorite NOTIFY favoriteChanged)
    Q_PROPERTY(QString description READ description WRITE setDescription NOTIFY descriptionChanged)
    Q_PROPERTY(bool bolded READ bolded WRITE setBolded NOTIFY boldedChanged)
    Q_PROPERTY(int xPosition READ xPosition CONSTANT)
    Q_PROPERTY(int yPosition READ yPosition CONSTANT)
public:
    explicit LocationProxy(QString itemText, bool fav, QString desc, bool bolded, QObject *parent = 0);

    QString itemText() const {return _itemText;}

    bool favorite() const {return _favorite;}
    void setFavorite( bool bFav);

    QString description() const {return _description;}
    void setDescription(const QString& desc);

    bool bolded() const {return _bolded;}
    void setBolded(bool b);

    int xPosition() const;
    int yPosition() const;
    void setPosition(const std::pair<int,int>& p) { _position = p;}

signals:

    void favoriteChanged();
    void descriptionChanged();
    void boldedChanged();

public slots:
private:
    QString _itemText;
    bool _favorite;
    QString _description;
    bool _bolded;
    std::pair<int,int> _position;
};

#endif // LOCATIONPROXY_H
