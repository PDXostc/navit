#include "nxe_instance.h"
#include "inavitprocess.h"
#include "settings.h"
#include "settingtags.h"
#include "log.h"
#include "inavitipc.h"
#include "igpsprovider.h"
#include "imapdownloader.h"
#include "ispeech.h"

#include <sstream>
#include <string>
#include <iostream>
#include <iomanip>
#include <thread>

using namespace std;

namespace NXE {

struct NXEInstancePrivate {

    NXEInstancePrivate(DI::Injector& ifaces)
        : navitProcess(NXE::get<std::shared_ptr<INavitProcess> >(ifaces))
        , ipc(NXE::get<std::shared_ptr<INavitIPC> >(ifaces))
        , gps(NXE::get<std::shared_ptr<IGPSProvider> >(ifaces))
        , mapDownloaderIPC(NXE::get<std::shared_ptr<IMapDownloader> >(ifaces))
        , speech(NXE::get<std::shared_ptr<ISpeech> >(ifaces))
        , geometry(std::make_pair(0, 0))
    {
        ipc->initializedSignal().connect([this]() {
            nDebug() << "Navit has initialized";
            if (geometry != std::make_pair(0,0)) {
                ipc->resize(geometry.first, geometry.second);
            }
        });
    }

    std::shared_ptr<INavitProcess> navitProcess;
    std::shared_ptr<INavitIPC> ipc;
    std::shared_ptr<IGPSProvider> gps;
    std::shared_ptr<IMapDownloader> mapDownloaderIPC;
    std::shared_ptr<ISpeech> speech;
    std::pair<int, int> geometry;
    Settings settings;
    bool initialized{ false };
    bool mute{ false };
    std::thread distanceThread;
    bool distanceThreadShouldRun{ false };
    bool distanceThreadRunning {false};

    void setOrientation(int newOrientation)
    {
        ipc->setOrientation(newOrientation);
    }

    void toogleAudio(bool volumeOn)
    {
        mute = !volumeOn;
        nDebug() << "Turning audio " << (mute ? "off" : "on");
    }

    void resize(int w, int h)
    {
        using SettingsTags::Navit::ExternalNavit;
        const bool external = settings.get<ExternalNavit>();
        if (external)
            return;
        if (!initialized) {
            geometry = std::make_pair(w, h);
        }
        else {
            // TODO: This does not yet send an message since this would
            // block current thread.
            // ipc->resize(w,h);
        }

        if (w == 0 && h == 0) {
            nInfo() << "Force resize";
            ipc->resize(geometry.first, geometry.second);
        }
    }

    void getDistanceThread()
    {
        distanceThreadRunning = true;
        nDebug() << "Starting distance thread";
        std::chrono::milliseconds dura_3s(3000);
        std::this_thread::sleep_for(dura_3s);

        while (distanceThreadShouldRun) {
            nTrace() << "Getting navigation informations";
            std::chrono::milliseconds dura_1s(1000);
            std::this_thread::sleep_for(dura_1s);

            ipc->distance();
            ipc->eta();
            ipc->currentStreet();
        }

        nTrace() << "Finishing getDistanceThread()";
        distanceThreadRunning = false;
    }
};

NXEInstance::NXEInstance(DI::Injector& impls)
    : d(new NXEInstancePrivate{ impls })
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
        if (d->initialized) {
            d->ipc->quit();
        }
        d->navitProcess->stop();
    }

    // stop destination thread
    d->distanceThreadShouldRun = false;
    if (d->distanceThread.joinable()) {
        d->distanceThread.join();
    }
}

void NXEInstance::startNavit()
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
        d->ipc->speechSignal().connect([this](const std::string& string) {
            nDebug() << "Saying " << string << " speech pointer = " << static_cast<void*>(d->speech.get());
            if (d->speech && !(d->mute)) {
                d->speech->say(string);
            }
        });
    }
    d->initialized = true;
}

void NXEInstance::restartNavit()
{
    stopNavit();
    startNavit();

    d->ipc->restart();
}

void NXEInstance::stopNavit()
{
    d->ipc->quit();
    d->navitProcess->stop();
    d->initialized = false;
}

void NXEInstance::setWaylandSocketName(const std::string& socketName)
{
    d->navitProcess->setSocketName(socketName);
}

void NXEInstance::setAudioMute(bool mute)
{
    d->mute = mute;
}

void NXEInstance::setMapDownloaderListener(const MapDownloaderListener& listener)
{
    d->mapDownloaderIPC->setListener(listener);
}

void NXEInstance::setPositionUpdateListener(const IGPSProvider::PositionUpdateCb& listener)
{
    d->gps->addPostionUpdate(listener);
}

void NXEInstance::resize(int w, int h)
{
    d->resize(w, h);
}

void NXEInstance::startNavigation(double lat, double lon, const string& description)
{
    assert(d && d->ipc);
    d->ipc->setDestination(lat, lon, description);

    if (d->distanceThreadRunning) {
        return;
    }

    d->distanceThreadShouldRun = true;
    d->distanceThread =std::thread { std::bind(&NXEInstancePrivate::getDistanceThread, d.get()) };
}

void NXEInstance::cancelNavigation()
{
    nDebug() << "Canceling navigation";
    assert(d && d->ipc);
    d->distanceThreadShouldRun = false;
    if (d->distanceThread.joinable()) {
        d->distanceThread.join();
    }
    d->ipc->clearDestination();
}

INavitIPC::PointClickedSignalType& NXEInstance::pointClickedSignal()
{
    return d->ipc->pointClickedSignal();
}

INavitIPC* NXEInstance::ipc() const
{
    assert(d && d->ipc);
    return d->ipc.get();
}

IMapDownloader* NXEInstance::mapDownloader() const
{
    assert(d && d->mapDownloaderIPC);
    return d->mapDownloaderIPC.get();
}

IGPSProvider* NXEInstance::gps() const
{
    assert(d && d->gps);
    return d->gps.get();
}

} // namespace NXE
