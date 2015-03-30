#include "gpsdprovider.h"
#include "log.h"

#include <thread>
#include <libgpsmm.h>
#include <cstdint>

namespace {
const std::int32_t gpsTimeout = 50000;
}

namespace NXE {

struct GPSDProviderPrivate {

    void threadRoutine () {
        nTrace() << "Starting gpsd polling gpsd is open " << (m_gpsd.is_open() ? "true" : "false");

        gps_data_t *streamData { nullptr }, *newData {nullptr};
        streamData = m_gpsd.stream(WATCH_ENABLE | WATCH_JSON);

        if (streamData == nullptr) {
            nError() << "GPSD server is not working";
            // gpsd is not working, stop
        } else {

            while (m_bThreadRunning) {
                if ( m_gpsd.waiting(gpsTimeout) ) {
                    newData = m_gpsd.read();
                    if (newData && newData->status > 0) {
                        nDebug() << "[longitude, latitude, altitude] [ " <<
                                    newData->fix.longitude << " , " <<
                                    newData->fix.latitude  << " , " <<
                                    newData->fix.altitude  << " ]";
                        m_currentPosition.altitude = newData->fix.altitude;
                        m_currentPosition.longitude = newData->fix.longitude;
                        m_currentPosition.latitude = newData->fix.latitude;
                    }
                } else {
                }
            }

            nInfo() << "GPSD thread is exiting";
        }
    }

    std::thread m_gpsdThread;
    bool m_bThreadRunning {true};
    gpsmm m_gpsd {"localhost", DEFAULT_GPSD_PORT};

    Position m_currentPosition;
};

GPSDProvider::GPSDProvider()
    : d(new GPSDProviderPrivate)
{
    d->m_gpsdThread = std::thread(std::bind(&GPSDProviderPrivate::threadRoutine, d.get()));
}

GPSDProvider::~GPSDProvider()
{
    d->m_bThreadRunning = false;
    d->m_gpsdThread.join();
}

Position GPSDProvider::position() const
{
    return d->m_currentPosition;
}

} // namespace NXE

