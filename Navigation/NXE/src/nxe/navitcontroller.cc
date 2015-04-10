#include "navitcontroller.h"
#include "inavitipc.h"
#include "log.h"
#include "calls.h"
#include "igpsprovider.h"
#include "imapdownloader.h"

#include <functional>
#include <map>
#include <typeindex>
#include <thread>
#include <chrono>

#include <fruit/injector.h>

#include <boost/lexical_cast.hpp>
#include <boost/fusion/algorithm/iteration/for_each.hpp>
#include <boost/fusion/include/for_each.hpp>
#include <boost/signals2/signal.hpp>

namespace bpt = boost::property_tree;

namespace NXE {

struct NavitControllerPrivate {
    std::shared_ptr<INavitIPC> ipc;
    std::shared_ptr<IGPSProvider> gps;
    std::shared_ptr<IMapDownloader> mapDownloaderIPC;
    std::map<std::string, std::uint32_t> mapDownloadIds;
    NavitController* q;
    std::thread m_retriggerThread;
    bool m_isRunning = false;
    boost::signals2::signal<void(const JSONMessage&)> successSignal;
    map_type m{
        boost::fusion::make_pair<MoveByMessage>("moveBy"),
        boost::fusion::make_pair<ZoomByMessage>("zoomBy"),
        boost::fusion::make_pair<ZoomMessage>("zoom"),
        boost::fusion::make_pair<PositionMessage>("position"),
        boost::fusion::make_pair<RenderMessage>("render"),
        boost::fusion::make_pair<ExitMessage>("exit"),
        boost::fusion::make_pair<SetOrientationMessage>("setOrientation"),
        boost::fusion::make_pair<OrientationMessage>("orientation"),
        boost::fusion::make_pair<SetCenterMessage>("setCenter"),
        boost::fusion::make_pair<DownloadMessage>("downloadMap"),
        boost::fusion::make_pair<CancelDownloadMessage>("cancelDownloadMap"),
        boost::fusion::make_pair<AvailableMapsMessage>("availableMaps"),
        boost::fusion::make_pair<SetDestinationMessage>("setDestination"),
        boost::fusion::make_pair<ClearDestinationMessage>("clearDestination"),
        boost::fusion::make_pair<SetPositionMessage>("setPosition")
    };

    map_cb_type cb{
        boost::fusion::make_pair<MoveByMessage>([this](const JSONMessage& message) {
            if (message.data.empty()) {
                // TODO: Change exception
                throw std::runtime_error("Unable to parse");
            }

            const int x = message.data.get<int>("x");
            const int y = message.data.get<int>("y");
            nDebug() << "IPC: Move by [x,y] = [" << x << ","  << y << "]";
            ipc->moveBy(x,y);

            // TODO: proper success signal
            JSONMessage response {message.id, message.call};
            successSignal(response);
        }),

        boost::fusion::make_pair<ZoomByMessage>([this](const JSONMessage& message) {
            int factor = message.data.get<int>("factor");
            ipc->zoomBy(factor);
            JSONMessage response {message.id, message.call};
            successSignal(response);
        }),

        boost::fusion::make_pair<ZoomMessage>([this](const JSONMessage& message) {
            int zoomValue = ipc->zoom();
            bpt::ptree values;
            values.put("zoom", zoomValue);
            JSONMessage response {message.id, message.call, "", values };
            successSignal(response);
        }),

        boost::fusion::make_pair<PositionMessage>([this](const JSONMessage& message) {
            Position pos = gps->position();
            bpt::ptree values;
            values.put("altitude", pos.altitude);
            values.put("longitude", pos.longitude);
            values.put("latitude", pos.latitude);
            JSONMessage response {message.id, message.call, "", values };
            successSignal(response);
        }),

        boost::fusion::make_pair<RenderMessage>([this](const JSONMessage& message) {
            ipc->render();
            JSONMessage response {message.id, message.call};
            successSignal(response);
        }),

        boost::fusion::make_pair<ExitMessage>([this](const JSONMessage& message) {
            ipc->stop();
            JSONMessage response {message.id, message.call};
            successSignal(response);
        }),

        boost::fusion::make_pair<SetOrientationMessage>([this](const JSONMessage& message) {
            int newOrientation = message.data.get<int>("orientation");
            ipc->setOrientation(newOrientation);
            JSONMessage response {message.id, message.call};
            successSignal(response);
        }),

        boost::fusion::make_pair<OrientationMessage>([this](const JSONMessage& message) {
            bpt::ptree p;
            p.put("orientation", ipc->orientation());
            JSONMessage response {message.id, message.call, "", p };
            successSignal(response);
        }),

        boost::fusion::make_pair<SetCenterMessage>([this](const JSONMessage& message) {

            const double longitude = message.data.get<double>("longitude");
            const double latitude = message.data.get<double>("latitude");

            ipc->setCenter(longitude, latitude);

            JSONMessage response {message.id, message.call};
            successSignal(response);
        }),

        boost::fusion::make_pair<DownloadMessage>([this](const JSONMessage& message) {
            // Janusz start download here
            const std::string region = message.data.get<std::string>("region");
            nTrace() << "Download message with region";
            bool ok = mapDownloaderIPC->download(region);
            if (ok) {
                successSignal(JSONMessage {message.id, message.call});
                mapDownloadIds[region] = message.id;
            } else {
                successSignal(JSONMessage {message.id, message.call, "An error happened"});
            }
        }),
        boost::fusion::make_pair<CancelDownloadMessage>([this](const JSONMessage& message) {
            // Janusz start download here
            const std::string region = message.data.get<std::string>("region");
            nTrace() << "Canceling download region= " << region;
            mapDownloaderIPC->cancel(region);
            successSignal(JSONMessage {message.id, message.call});
        }),

        boost::fusion::make_pair<AvailableMapsMessage>([this](const JSONMessage& message) {
            auto maps = mapDownloaderIPC->availableMaps();
            nDebug() << "Available map size=" << maps.size();
            bpt::ptree p;
            std::for_each(maps.begin(), maps.end(), [&p](const std::string &ctr) {
                p.add("country", ctr);
            });

            successSignal(JSONMessage {message.id, message.call, "", p});
        }),

        boost::fusion::make_pair<SetDestinationMessage>([this](const JSONMessage& message) {

            const double longitude = message.data.get<double>("longitude");
            const double latitude = message.data.get<double>("latitude");
            const std::string description = message.data.get<std::string>("description");

            ipc->setDestination(longitude, latitude, description);

            JSONMessage response {message.id, message.call};
            successSignal(response);
        }),

        boost::fusion::make_pair<ClearDestinationMessage>([this](const JSONMessage& message) {

            ipc->clearDestination();

            JSONMessage response {message.id, message.call};
            successSignal(response);
        }),

        boost::fusion::make_pair<SetPositionMessage>([this](const JSONMessage& message) {

            const double longitude = message.data.get<double>("longitude");
            const double latitude = message.data.get<double>("latitude");

            ipc->setPosition(longitude, latitude);

            JSONMessage response {message.id, message.call};
            successSignal(response);
        }),


    };

    // A listener for IMapDownloader, all functions are lambda
    MapDownloaderListener listener{
        [this](const std::string& mapName, std::uint64_t now, std::uint64_t total) // progress callback
        {
            nTrace() << "Map download " << mapName << " progress callback";
            auto it = mapDownloadIds.find(mapName);
            if (it == mapDownloadIds.end()) {
                nInfo() << "Progress callback for " << mapName << " received but on id found";
                return;
            }

            bpt::ptree p;
            p.put("mapName", mapName);
            p.put("now", now);
            p.put("total", total);
            successSignal(JSONMessage{it->second, "downloadMap", "", p});
        },
        [this](const std::string& mapName, const std::string& errorStr) // error callback
        {
            auto it = mapDownloadIds.find(mapName);
            if (it == mapDownloadIds.end()) {
                nInfo() << "Error callback for " << mapName << " received but on id found";
                return;
            }

            successSignal(JSONMessage{it->second, "downloadMap", errorStr});
        },
        [this](const std::string& mapName) // finished callback
        {
            auto it = mapDownloadIds.find(mapName);
            if (it == mapDownloadIds.end()) {
                nInfo() << "Finished callback for " << mapName << " received but on id found";
                return;
            }

            bpt::ptree p;
            p.put("mapName", mapName);
            p.put("finished", true);
            successSignal(JSONMessage{it->second, "downloadMap", "", p});
        },
    };

    template <typename T>
    void handleMessage(const JSONMessage& data)
    {
        auto fn = boost::fusion::at_key<T>(cb);
        fn(data);
    }

    void speechCallback(const std::string& data)
    {
        bpt::ptree tree;
        tree.put("text", data);
        JSONMessage speech{ 0, "speech", "", tree };
        successSignal(speech);
    }
};

template <class Pred, class Fun>
struct filter {
    Pred pred_;
    const Fun& fun_;

    filter(Pred p, const Fun& f)
        : pred_(p)
        , fun_(f)
    {
    }

    template <class Pair>
    void operator()(Pair& pair) const
    {
        if (pred_(pair.second))
            fun_(pair);
    }
};

template <class Pred, class Fun>
filter<Pred, Fun> make_filter(Pred p, const Fun& f)
{
    return filter<Pred, Fun>(p, f);
}

struct fun {
    fun(NXE::NavitControllerPrivate* d, const JSONMessage& data)
        : _d(d)
        , _data(data)
    {
    }

    template <class First, class Second>
    void operator()(boost::fusion::pair<First, Second>&) const
    {
        _d->handleMessage<First>(_data);
    }

    NXE::NavitControllerPrivate* _d;
    const JSONMessage& _data;
};

NavitController::NavitController(DI::Injector& ctx)
    : d(new NavitControllerPrivate)
{
    d->ipc = ctx.get<std::shared_ptr<INavitIPC> >();
    d->gps = ctx.get<std::shared_ptr<IGPSProvider> >();
    d->mapDownloaderIPC = ctx.get<std::shared_ptr<IMapDownloader> >();
    d->q = this;

}

NavitController::~NavitController()
{
}

void NavitController::positon()
{
    // TODO: Ask for LBS position
}

void NavitController::tryStart()
{
    nDebug() << "Trying to start IPC Navit controller";
    d->ipc->start();
    d->ipc->speechSignal().connect(std::bind(&NavitControllerPrivate::speechCallback, d.get(), std::placeholders::_1));
    d->ipc->initializedSignal().connect([]() {});

    d->mapDownloaderIPC->start();
    d->mapDownloaderIPC->setListener(d->listener);
}

void NavitController::handleMessage(const JSONMessage& msg)
{
    bool bCalled = false;
    nDebug() << "Handling message " << msg.call;
    boost::fusion::for_each(d->m, make_filter([msg, &bCalled](const std::string& str) -> bool {
        if (str == msg.call)  {
            bCalled = true;
            return true;
        }
        return false;
    }, fun(d.get(), msg)));

    if (!bCalled) {
        nFatal() << "Unable to call " << msg.call;
        throw std::runtime_error("No parser found");
    }
}

void NavitController::addListener(const NavitController::Callback_type& cb)
{
    d->successSignal.connect(cb);
}

} // namespace NXE
