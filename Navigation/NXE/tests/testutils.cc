#include "testutils.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/lexical_cast.hpp>
#include "settingtags.h"
#include "jsonmessage.h"

namespace bpt = boost::property_tree;
const std::string navitPath { NAVIT_PATH };

std::string TestUtils::moveByMessage(int x, int y)
{
    NXE::JSONMessage msg {1, "moveBy" };
    return NXE::JSONUtils::serialize(msg);
}


void TestUtils::createNXEConfFile()
{
    // create simple json config file
    bpt::ptree config;
    config.put(SettingsTags::Navit::Path::name(), navitPath);
    config.put(SettingsTags::Navit::AutoStart::name(), true);
    config.put(SettingsTags::FileLog::name(), "/tmp/log.file");
    bpt::write_json("nxe.conf", config);
}


std::string TestUtils::zoomByMessage(int factor)
{
    boost::property_tree::ptree p;
    p.put("factor", factor);
    NXE::JSONMessage msg {1, "zoomBy", "", p};
    return NXE::JSONUtils::serialize(msg);
}


std::string TestUtils::zoomMessage()
{
    NXE::JSONMessage msg { 3, "zoom"};
    return NXE::JSONUtils::serialize(msg);
}

std::string TestUtils::renderMessage()
{
    NXE::JSONMessage msg { 3, "render"};
    return NXE::JSONUtils::serialize(msg);
}
