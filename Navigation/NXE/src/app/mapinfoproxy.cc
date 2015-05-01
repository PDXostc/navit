#include "mapinfoproxy.h"

MapInfoProxy::MapInfoProxy(const NXE::MapInfo &mi, QObject *parent)
    : QObject(parent)
    , info(mi)
{
}

MapInfoProxy::~MapInfoProxy()
{
}

QString MapInfoProxy::mapName() const
{
    return QString::fromStdString(info.name);
}

