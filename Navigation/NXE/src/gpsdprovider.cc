#include "gpsdprovider.h"
#include "log.h"

#include <thread>
#include <libgpsmm.h>

namespace NXE {

struct GPSDProviderPrivate {

    void threadRoutine () {
        nTrace() << "Starting gpsd polling";
    }

    std::thread m_gpsdThread;
    gpsmm m_gpsd {"localhost", DEFAULT_GPSD_PORT};
};

GPSDProvider::GPSDProvider()
    : d(new GPSDProviderPrivate)
{
    d->m_gpsdThread = std::thread(std::bind(&GPSDProviderPrivate::threadRoutine, d.get()));
}

GPSDProvider::~GPSDProvider()
{
}

Position GPSDProvider::position()
{
    return Position {};
}

} // namespace NXE

