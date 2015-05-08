#ifndef MD_MAPDOWNLOADERDBUSSERVER_H
#define MD_MAPDOWNLOADERDBUSSERVER_H

#include "mapdownloaderadaptor.h"
#include <memory>
#include <string>

namespace md {

struct MapDownloaderDBusServerPrivate;
class MapDownloaderDBusServer : public org::nxe::mapdownloader_adaptor,
                                public DBus::IntrospectableAdaptor,
                                public DBus::ObjectAdaptor

                                {
public:
    MapDownloaderDBusServer(DBus::Connection& connection);
    ~MapDownloaderDBusServer();

    virtual bool setOutputDirectory(const std::string& path) override;
    bool download(const std::string& mapName) override;
    virtual void reportProgress(const bool& enable) override;
    virtual void cancel(const std::string &mapName) override;
    virtual std::vector< DBus::Struct< std::string, uint64_t, bool, std::string> > maps() override;
private:
    std::unique_ptr<MapDownloaderDBusServerPrivate> d;
};

} // namespace md

#endif // MD_MAPDOWNLOADERDBUSSERVER_H
