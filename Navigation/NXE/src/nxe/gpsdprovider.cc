#include "gpsdprovider.h"
#include "log.h"

#include <thread>
#include <gps.h>
#include <libgpsmm.h>
#include <cstdint>

namespace {
const std::int32_t gpsTimeout = 50000;
}

namespace NXE {

struct GPSDProviderPrivate {

    void threadRoutine()
    {
        gps_open("localhost", DEFAULT_GPSD_PORT, &gps_data);

        int status = gps_stream(&gps_data, WATCH_ENABLE | WATCH_JSON, nullptr);

        if (status == -1) {
            nError() << "GSPD is not running, exiting";
        }
        else {

            while (m_bThreadRunning) {
                if (gps_waiting(&gps_data, 5000)) {
                    if (gps_read(&gps_data) != -1) {
                        m_currentPosition.altitude = gps_data.fix.altitude;
                        m_currentPosition.longitude = gps_data.fix.longitude;
                        m_currentPosition.latitude = gps_data.fix.latitude;
                        std::for_each(callbacks.begin(), callbacks.end(), [this](const GPSDProvider::PositionUpdateCb& cb) {
                            if(cb) {
                                cb(m_currentPosition);
                            }
                        });
                    }
                }
            }
        }
    }

    std::thread m_gpsdThread;
    bool m_bThreadRunning{ true };
    Position m_currentPosition;
    gps_data_t gps_data;
    std::vector<GPSDProvider::PositionUpdateCb> callbacks;
};

GPSDProvider::GPSDProvider()
    : d(new GPSDProviderPrivate)
{
    nTrace() << "GPSDProvider::GPSDProvider()";
    d->m_gpsdThread = std::thread{ std::bind(&GPSDProviderPrivate::threadRoutine, d.get()) };
}

GPSDProvider::~GPSDProvider()
{
    nTrace() << "~GPSDProvider";
    d->m_bThreadRunning = false;
    d->m_gpsdThread.join();
}

Position GPSDProvider::position() const
{
    return d->m_currentPosition;
}

void GPSDProvider::addPostionUpdate(const IGPSProvider::PositionUpdateCb &position)
{
    d->callbacks.push_back(position);
}

} // namespace NXE
