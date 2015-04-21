#include "testutils.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem/operations.hpp>
#include "settingtags.h"
#include "jsonmessage.h"

namespace bpt = boost::property_tree;
const std::string navitPath{ NAVIT_PATH };

extern bool runNavit;

std::string TestUtils::moveByMessage(int x, int y)
{
    bpt::ptree data;
    data.put("x", x);
    data.put("y", y);
    NXE::JSONMessage msg{ 1, "moveBy", "", data };
    return NXE::JSONUtils::serialize(msg);
}

void TestUtils::createNXEConfFile()
{
    // create simple json config file
    bpt::ptree config;
    config.put(SettingsTags::Navit::Path::name(), navitPath);
    config.put(SettingsTags::Navit::AutoStart::name(), true);
    config.put(SettingsTags::Navit::ExternalNavit::name(), !runNavit);
    // xcb is default QT_QPA_PLATFORM for X11
    config.put(SettingsTags::Navit::GraphicPlatform::name(), "xcb");
    config.put(SettingsTags::LogPath::name(), "/tmp/");
    bpt::write_json("nxe.conf", config);
}

std::string TestUtils::zoomByMessage(int factor)
{
    boost::property_tree::ptree p;
    p.put("factor", factor);
    NXE::JSONMessage msg{ 1, "zoomBy", "", p };
    return NXE::JSONUtils::serialize(msg);
}

std::string TestUtils::setDestinationMessage(double lon, double lat, const std::string& description)
{
    boost::property_tree::ptree p;
    p.put("longitude", lon);
    p.put("latitude", lat);
    p.put("description", description);

    NXE::JSONMessage msg{ 1, "setDestination", "", p };
    return NXE::JSONUtils::serialize(msg);
}

std::string TestUtils::setPositionMessage(double lon, double lat)
{
    boost::property_tree::ptree p;
    p.put("longitude", lon);
    p.put("latitude", lat);

    NXE::JSONMessage msg{ 1, "setPosition", "", p };
    return NXE::JSONUtils::serialize(msg);
}


std::string TestUtils::clearDestinationMessage()
{
    NXE::JSONMessage msg{ 1, "clearDestination" };
    return NXE::JSONUtils::serialize(msg);
}



std::string TestUtils::zoomMessage()
{
    NXE::JSONMessage msg{ 3, "zoom" };
    return NXE::JSONUtils::serialize(msg);
}

std::string TestUtils::renderMessage()
{
    NXE::JSONMessage msg{ 3, "render" };
    return NXE::JSONUtils::serialize(msg);
}

std::string TestUtils::changeOrientationMessage(int orientation)
{
    boost::property_tree::ptree p;
    p.put("orientation", orientation);
    NXE::JSONMessage msg{ 3, "setOrientation", "", p };
    return NXE::JSONUtils::serialize(msg);
}

std::string TestUtils::orientationMessage()
{
    NXE::JSONMessage msg{ 3, "orientation" };
    return NXE::JSONUtils::serialize(msg);
}

std::string TestUtils::positionMessage()
{
    NXE::JSONMessage msg{ 100, "position" };
    return NXE::JSONUtils::serialize(msg);
}


std::string TestUtils::downloadMessage(const std::string &country)
{
    boost::property_tree::ptree p;
    p.put("region", country);
    NXE::JSONMessage msg {401, "downloadMap", "", p};
    return NXE::JSONUtils::serialize(msg);
}


std::string TestUtils::availableMessages()
{
    NXE::JSONMessage msg{ 100, "availableMaps" };
    return NXE::JSONUtils::serialize(msg);
}


std::string TestUtils::cancelDownloadMessage(const std::string &country)
{
    boost::property_tree::ptree p;
    p.put("region", country);
    NXE::JSONMessage msg {401, "cancelDownloadMap", "", p};
    return NXE::JSONUtils::serialize(msg);
}


void TestUtils::removeNXEConfFile()
{
    boost::filesystem::remove("nxe.conf");
}
