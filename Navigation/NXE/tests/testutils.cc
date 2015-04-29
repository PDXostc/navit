#include "testutils.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem/operations.hpp>
#include "settingtags.h"

namespace bpt = boost::property_tree;
const std::string navitPath{ NAVIT_PATH };

extern bool runNavit;

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

void TestUtils::removeNXEConfFile()
{
    boost::filesystem::remove("nxe.conf");
}
