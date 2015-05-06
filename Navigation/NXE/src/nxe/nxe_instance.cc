#include "nxe_instance.h"
#include "inavitprocess.h"
#include "settings.h"
#include "settingtags.h"
#include "log.h"
#include "calls.h"
#include "inavitipc.h"
#include "igpsprovider.h"
#include "imapdownloader.h"
#include "ispeech.h"

#include <sstream>
#include <string>
#include <iostream>
#include <iomanip>
#include <fruit/injector.h>

using namespace std;
using namespace boost::fusion;

namespace NXE {

struct NXEInstancePrivate {

    NXEInstancePrivate(DI::Injector& ifaces, NXEInstance* qptr)
        : navitProcess(ifaces.get<std::shared_ptr<INavitProcess> >())
        , ipc(ifaces.get<std::shared_ptr<INavitIPC> >())
        , gps(ifaces.get<std::shared_ptr<IGPSProvider> >())
        , mapDownloaderIPC(ifaces.get<std::shared_ptr<IMapDownloader> >())
        , speech(ifaces.get<std::shared_ptr<ISpeech> >())
    {
    }

    std::shared_ptr<INavitProcess> navitProcess;
    std::shared_ptr<INavitIPC> ipc;
    std::shared_ptr<IGPSProvider> gps;
    std::shared_ptr<IMapDownloader> mapDownloaderIPC;
    std::shared_ptr<ISpeech> speech;
    Settings settings;
    bool initialized{ false };

    void moveBy(int x, int y)
    {
        ipc->moveBy(x, y);
    }

    void setOrientation(int newOrientation)
    {
        ipc->setOrientation(newOrientation);
    }

    void zoomBy(int factor)
    {
        ipc->zoomBy(factor);
    }

    int zoomMessage()
    {
        return ipc->zoom();
    }
};

NXEInstance::NXEInstance(DI::Injector& impls)
    : d(new NXEInstancePrivate{ impls, this })
    , fusion_list(
          make_pair<MoveByMessageTag>(bind(&NXEInstancePrivate::moveBy, d.get(), placeholders::_1, placeholders::_2)),
          make_pair<ZoomByMessageTag>(bind(&NXEInstancePrivate::zoomBy, d.get(), placeholders::_1)),
          make_pair<ZoomMessageTag>(bind(&NXEInstancePrivate::zoomMessage, d.get())),
          make_pair<SetOrientationMessageTag>(bind(&NXEInstancePrivate::setOrientation, d.get(), placeholders::_1)),
          make_pair<PositionMessageTag>(bind(&IGPSProvider::position, d->gps.get())),
          make_pair<RenderMessageTag>(bind(&INavitIPC::render, d->ipc.get())),
          make_pair<ExitMessageTag>(bind(&INavitIPC::quit, d->ipc.get())),
          make_pair<OrientationMessageTag>(bind(&INavitIPC::orientation, d->ipc.get())),
          make_pair<SetCenterMessageTag>(bind(&INavitIPC::setCenter, d->ipc.get(), placeholders::_1, placeholders::_2)),
          make_pair<DownloadMessageTag>(bind(&IMapDownloader::download, d->mapDownloaderIPC.get(), placeholders::_1)),
          make_pair<CancelDownloadMessageTag>(bind(&IMapDownloader::cancel, d->mapDownloaderIPC.get(), placeholders::_1)),
          make_pair<MapsMessageTag>(bind(&IMapDownloader::maps, d->mapDownloaderIPC.get())),
          make_pair<SetDestinationMessageTag>( [this](double lon, double lat, const char* desc) { d->ipc->setDestination(lon,lat,desc); }),
          make_pair<ClearDestinationMessageTag>( bind(&INavitIPC::clearDestination, d->ipc.get())),
          make_pair<SetPositionMessageTag>(bind(&INavitIPC::setPosition, d->ipc.get(), placeholders::_1, placeholders::_2)),
          make_pair<SetSchemeMessageTag>(bind(&INavitIPC::setScheme, d->ipc.get(), placeholders::_1)),
          make_pair<StartSearchTag>(bind(&INavitIPC::startSearch, d->ipc.get())),
          make_pair<SearchCountryLocationTag>(bind(&INavitIPC::searchCountry, d->ipc.get(), placeholders::_1)),
          make_pair<SearchCityLocationTag>(bind(&INavitIPC::searchCity, d->ipc.get(), placeholders::_1)),
          make_pair<AddWaypointMessageTag>(bind(&INavitIPC::addWaypoint, d->ipc.get(), placeholders::_1, placeholders::_2))
      )
{
    nDebug() << "Creating NXE instance. Settings path = " << d->settings.configPath();
    nTrace() << "Connecting to navitprocess signals";
}

NXEInstance::~NXEInstance()
{
    nTrace() << "~NXEInstance";
    using SettingsTags::Navit::ExternalNavit;
    const bool external = d->settings.get<ExternalNavit>();

    nTrace() << "Stopping controller. external navit=" << external;

    if (!external) {
        d->navitProcess->stop();
    }
}

void NXEInstance::Initialize()
{
    if (d->initialized) {
        nDebug() << "Already initilized";
        return;
    }
    using SettingsTags::Navit::AutoStart;
    using SettingsTags::Navit::ExternalNavit;

    nDebug() << "Initializing NXEInstance";
    bool bAutoRun = d->settings.get<AutoStart>();
    if (bAutoRun) {
        bool external = d->settings.get<ExternalNavit>();
        if (!external) {
            d->navitProcess->start();
        }
        else {
            nInfo() << "Navit external is set, won't run";
        }
        nDebug() << "Trying to start IPC Navit controller";
        d->ipc->speechSignal().connect([this] (const std::string& string) {
            nDebug() << "Saying " << string << " speech pointer = " << static_cast<void*>(d->speech.get());
            if (d->speech) {
                d->speech->say(string);
            }
        });
    }
    d->initialized = true;
}

void NXEInstance::setWaylandSocketName(const std::string& socketName)
{
    d->navitProcess->setSocketName(socketName);
}

void NXEInstance::setMapDownloaderListener(const MapDownloaderListener &listener)
{
    d->mapDownloaderIPC->setListener(listener);
}

void NXEInstance::setPositionUpdateListener(const IGPSProvider::PositionUpdateCb &listener)
{
    d->gps->addPostionUpdate(listener);
}

INavitIPC::PointClickedSignalType &NXEInstance::pointClickedSignal()
{
    return d->ipc->pointClickedSignal();
}

} // namespace NXE
