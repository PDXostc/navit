#ifndef MAPDOWNLOADERDBUS_H
#define MAPDOWNLOADERDBUS_H

#include "imapdownloader.h"
#include <fruit/macro.h>
#include <memory>

namespace NXE {
struct MapDownloaderDBusPrivate;
class MapDownloaderDBus : public IMapDownloader
{
public:
    INJECT(MapDownloaderDBus());
    ~MapDownloaderDBus();

    virtual void start() override;
    virtual std::vector<std::string> availableMaps() override;
    virtual bool download(const std::string &region) override;
    virtual void setListener(const MapDownloaderListener & listener) override;
    virtual void cancel(const std::string& region) override;
    virtual bool setOutputDirectory(const std::string &outDirectory) override;

private:
    std::unique_ptr<MapDownloaderDBusPrivate> d;
};
} // NXE


#endif // MAPDOWNLOADERDBUS_H
