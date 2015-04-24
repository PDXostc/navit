#ifndef TESTUTILS_H
#define TESTUTILS_H

#include <string>
#include <chrono>
#include "log.h"
#include "jsonmessage.h"

namespace TestUtils {


std::string moveByMessage(int x, int y);
std::string zoomByMessage(int factor);
std::string zoomMessage();
std::string renderMessage();
std::string changeOrientationMessage(int orientation);
std::string orientationMessage();
std::string positionMessage();
std::string downloadMessage(const std::string &country);
std::string cancelDownloadMessage(const std::string &country);
std::string availableMessages();
std::string setDestinationMessage(double lon, double lat, const std::string& description);
std::string setPositionMessage(double lon, double lat);
std::string clearDestinationMessage();
std::string addWaypointMessage(double lon, double lat);

void createNXEConfFile();

struct Timer {
    typedef std::chrono::high_resolution_clock clock_t;
    clock_t::time_point t1{ clock_t::now() };

    ~Timer()
    {
        auto t2 = clock_t::now();
        std::chrono::duration<double, std::milli> diff = t2 - t1;
        nDebug() << diff.count();
    }
};
}

#endif // TESTUTILS_H
