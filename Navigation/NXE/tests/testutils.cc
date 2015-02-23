#include "testutils.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/lexical_cast.hpp>
#include "settingtags.h"
#include "jsonmessage.h"

namespace bpt = boost::property_tree;

std::string TestUtils::moveByMessage(int x, int y)
{
    NXE::JSONMessage msg {1, CallType::moveBy };
    return NXE::JSONUtils::serialize(msg);
}


void TestUtils::createNXEConfFile()
{
    // create simple json config file
    bpt::ptree config;
    config.put(SettingsTags::Navit::Path::name(), "/home/bata/builds/navit-qt5/navit");
    config.put(SettingsTags::Navit::AutoStart::name(), true);
    bpt::write_json("nxe.conf", config);
}


std::string TestUtils::zoomByMessage(int factor)
{
    std::string data = boost::lexical_cast<std::string>(factor);
    NXE::JSONMessage msg {1, CallType::zoomBy, 0,data };
    return NXE::JSONUtils::serialize(msg);
}
