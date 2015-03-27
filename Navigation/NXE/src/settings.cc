#include "settings.h"
#include "log.h"

#include <list>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/property_tree/json_parser.hpp>

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
        return configPath.string();
    }

    for (std::string str : settingPaths) {
        if (check(str)) {
            configPath = str;
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
    bpt::read_json(m_configPath, m_tree);
}

Settings::~Settings()
{
}

} // namespace NXE
