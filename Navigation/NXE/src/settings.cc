#include "settings.h"
#include "log.h"

#include <list>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <iostream>

namespace bfs = boost::filesystem;
namespace bpt = boost::property_tree;

namespace {
const std::list<std::string> settingPaths{
    "/usr/share/nxe/nxe.conf",
    "/etc/nxe/nxe.conf",
    "$HOME/.nxe/nxe.conf"
};

//! This will return first path in settingPaths that does exists and is readable
std::string getConfigPath()
{
    auto check = [](const bfs::path& path) -> bool {
        std::cout << "Checking " << path << std::endl;
        if (bfs::exists(path) ) {
            bfs::file_status st = bfs::status(path);

            if (st.permissions() & bfs::owner_write) {
                return true;
            }
        }

        return false;
    };

    bfs::path configPath = bfs::current_path();
    configPath /= bfs::path("nxe.conf");

    if (check(configPath)) {
        std::cout << "nxe.conf found in current dir [" << configPath.string() << " ] so it will be used" << std::endl;
        return configPath.string();
    }

    for (std::string str : settingPaths) {
        std::cout << "Checking path=" << str << std::endl;
        if (check(str)) {
            configPath = str;
            std::cout << "File exists and will be used, path= " << str << std::endl;
            break;
        }
    }

    return configPath.string();
}
}

namespace NXE {

Settings::Settings()
    : m_configPath(getConfigPath())
{
    std::cout << "Reading config file from" << m_configPath << std::endl;
    bpt::read_json(m_configPath, m_tree);
    std::cout << "Settings are ready" << std::endl;
}

Settings::~Settings()
{
}

} // namespace NXE
