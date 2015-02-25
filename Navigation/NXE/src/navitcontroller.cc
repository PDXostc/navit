#include "navitcontroller.h"
#include "log.h"

#include <functional>
#include <map>
#include <typeindex>
#include <thread>
#include <chrono>

#include <boost/lexical_cast.hpp>
#include <boost/fusion/algorithm/iteration/for_each.hpp>
#include <boost/fusion/include/for_each.hpp>

namespace NXE {
const std::uint16_t timeout = 2;

struct NavitControllerPrivate {
    NavitController* q;
    std::thread m_retriggerThread;
    bool m_isRunning = false;
    map_type m{ boost::fusion::make_pair<MoveByMessage>("moveBy"),
                boost::fusion::make_pair<ZoomByMessage>("zoomBy"),
                boost::fusion::make_pair<ZoomMessage>("zoom") };

    map_cb_type cb{
        boost::fusion::make_pair<MoveByMessage>([](const std::string& data) {}),
        boost::fusion::make_pair<ZoomByMessage>([](const std::string& data) {}),
        boost::fusion::make_pair<ZoomMessage>([](const std::string& data) {}),
    };

    template <typename T>
    void handleMessage(const std::string& data)
    {
        auto fn = boost::fusion::at_key<T>(cb);
        fn(data);
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
    fun(NXE::NavitControllerPrivate* d, const std::string& data)
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
    const std::string& _data;
};

NavitController::NavitController()
    : d_ptr(new NavitControllerPrivate)
{
    d_ptr->q = this;
}

NavitController::~NavitController()
{
}

void NavitController::tryStart()
{
    nDebug() << "Trying to start IPC Navit controller";
    if (!start()) {
        nInfo() << "Unable to start IPC yet, will try to do that in " << timeout << " seconds";
        d_ptr->m_retriggerThread = std::thread([this]() {
            std::chrono::milliseconds dura(timeout * 1000);
            std::this_thread::sleep_for(dura);
            if(!start()) {
                nFatal() << "Unable to start IPC, we won't be able to continue";
            } else {
                d_ptr->m_isRunning = true;
            }
        });
    } else {
        d_ptr->m_isRunning = true;
    }
}

void NavitController::handleMessage(JSONMessage msg)
{
    nDebug() << "Handling message " << msg.call;
    bool bCalled = false;
    try {
        const std::string& val = msg.data.get_value_or("");
        boost::fusion::for_each(d_ptr->m, make_filter([msg, &bCalled](const std::string& str) -> bool {
            if (str == msg.call)  {
                bCalled = true;
                return true;
            }
            return false;
                                                      },
                                                      fun(d_ptr.get(), val)));

        if (!bCalled) {
            nFatal() << "Unable to call " << msg.call;
        }
    }
    catch (const std::exception &ex)
    {
        nFatal() << "Unable to call IPC. Error= " << ex.what();
    }
}

} // namespace NXE
