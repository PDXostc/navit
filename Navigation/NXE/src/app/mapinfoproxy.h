#ifndef MAPINFOPROXY_H
#define MAPINFOPROXY_H

#include <QObject>
#include "imapdownloader.h"

class MapInfoProxy : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString mapName READ mapName CONSTANT)
    Q_PROPERTY(bool downloaded READ downloaded)
    Q_PROPERTY(quint64 size READ size)
public:
    explicit MapInfoProxy(const NXE::MapInfo& mi, QObject *parent = 0);
    ~MapInfoProxy();

    QString mapName() const;
    bool downloaded() { return info.downloaded;}
    quint64 size() {return info.size;}

private:
    const NXE::MapInfo info;
};

#endif // MAPINFOPROXY_H
