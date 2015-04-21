#ifndef TESTUTILS_H
#define TESTUTILS_H

#include <string>
#include <chrono>
#include "log.h"
#include "jsonmessage.h"

namespace TestUtils {

NXE::JSONMessage moveByMessage(int x, int y);
NXE::JSONMessage zoomByMessage(int factor);
NXE::JSONMessage zoomMessage();
NXE::JSONMessage renderMessage();
NXE::JSONMessage changeOrientationMessage(int orientation);
NXE::JSONMessage orientationMessage();
NXE::JSONMessage positionMessage();
NXE::JSONMessage downloadMessage(const std::string &country);
NXE::JSONMessage cancelDownloadMessage(const std::string &country);
NXE::JSONMessage availableMessages();
NXE::JSONMessage setDestinationMessage(double lon, double lat, const std::string& description);
NXE::JSONMessage setPositionMessage(double lon, double lat);
NXE::JSONMessage clearDestinationMessage();

void createNXEConfFile();
void removeNXEConfFile();

} //TestUtils

#endif // TESTUTILS_H
