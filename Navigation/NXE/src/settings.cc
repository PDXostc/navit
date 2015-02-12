#include "settings.h"
#include "log.h"

#include <list>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace bfs = boost::filesystem;
namespace bpt = boost::property_tree;

namespace {
const std::list<std::string> settingPaths {"/etc/nxe/nxe.conf", "$HOME/.nxe/nxe.conf"};

std::string getConfigPath()
{
    auto check = [](const bfs::path &path) -> bool{
        nDebug() << "Checking " << path;
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
        nDebug() << "nxe.conf found in current dir [" << configPath.string() << " ] so it will be used";
        return configPath.string();
    }

    for(std::string str: settingPaths) {
        nDebug() << "Checking path=" << str;
        if(check(str)) {
            configPath = str;
            nDebug() << "File exists and will be used, path= " << str;
            break;
        }
    }

    return configPath.string();
}

}

namespace NXE {

Settings::Settings():
    m_configPath(getConfigPath())
{
    bpt::read_json(m_configPath, m_tree);
    nDebug() << "Settings are ready";
}

Settings::~Settings()
{
}

} // namespace NXE

