#ifndef NXE_NXEINSTANCE_H
#define NXE_NXEINSTANCE_H

#include "context.h"
#include "imapdownloader.h"
#include "igpsprovider.h"
#include "inavitipc.h"

#include <vector>
#include <functional>

namespace NXE {
class INavitProcess;
class INavitIPC;
class NavitController;
class IGPSProvider;
class IMapDownloader;

struct NXEInstancePrivate;
class NXEInstance {
public:
    NXEInstance() = delete;
    NXEInstance(DI::Injector& impls);
    ~NXEInstance();

    virtual void Initialize();

    void setWaylandSocketName(const std::string& socketName);
    void setAudioMute(bool mute);

    void setMapDownloaderListener(const MapDownloaderListener& listener);
    void setPositionUpdateListener(const NXE::IGPSProvider::PositionUpdateCb& listener);

    void resize(int w, int h);

    void startNavigation(double lat, double lon, const std::string& description);
    void cancelNavigation();

    INavitIPC::PointClickedSignalType& pointClickedSignal();

    INavitIPC* ipc() const;
    IMapDownloader* mapDownloader() const;
    IGPSProvider* gps() const;


private:
    std::unique_ptr<NXEInstancePrivate> d;
};

} // namespace NXE

#endif // NXE_NXEINSTANCE_H
