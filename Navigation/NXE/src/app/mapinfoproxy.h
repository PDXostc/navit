#ifndef MAPINFOPROXY_H
#define MAPINFOPROXY_H

#include <QObject>
#include "imapdownloader.h"

class MapInfoProxy : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString mapName READ mapName CONSTANT)
    Q_PROPERTY(bool downloaded READ downloaded NOTIFY downloadedChanged)
    Q_PROPERTY(quint64 mapSize READ mapSize CONSTANT)
    Q_PROPERTY(QString continent READ continent CONSTANT)
public:
    explicit MapInfoProxy(const NXE::MapInfo& mi, QObject *parent = 0);
    ~MapInfoProxy();

    QString mapName() const;
    bool downloaded() { return info.downloaded;}
    quint64 mapSize() {return info.size;}
    QString continent() const;

signals:
    void downloadedChanged();

private:
    const NXE::MapInfo info;
};

#endif // MAPINFOPROXY_H
