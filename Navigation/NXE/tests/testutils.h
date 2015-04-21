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

void createNXEConfFile();
void removeNXEConfFile();

} //TestUtils

#endif // TESTUTILS_H
