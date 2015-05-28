#ifndef MAPDOWNLOADERDBUS_H
#define MAPDOWNLOADERDBUS_H

#include "imapdownloader.h"
#include <memory>

namespace NXE {
class DBusController;
struct MapDownloaderDBusPrivate;
class MapDownloaderDBus : public IMapDownloader
{
public:
    MapDownloaderDBus(DBusController&);
    ~MapDownloaderDBus();

    virtual std::vector<MapInfo> maps() override;
    virtual std::vector<MapInfo> recommendedMaps(const double& lon, const double& lat) override;
    virtual bool download(const std::string &region) override;
    virtual void setListener(const MapDownloaderListener & listener) override;
    virtual void cancel(const std::string& region) override;
    virtual bool setOutputDirectory(const std::string &outDirectory) override;
    virtual void removeAllMaps() override;

private:
    std::unique_ptr<MapDownloaderDBusPrivate> d;
};
} // NXE


#endif // MAPDOWNLOADERDBUS_H
