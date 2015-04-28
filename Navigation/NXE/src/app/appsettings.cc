#include "appsettings.h"
#include "alog.h"

#include <boost/property_tree/json_parser.hpp>
#include <boost/filesystem/operations.hpp>

namespace bfs = boost::filesystem;

namespace {
std::string getConfigPath()
{
    const bfs::path homePath { getenv("HOME") };
    if (!bfs::exists(homePath)) {
        throw std::runtime_error("HOME does not exists");
    }

    const bfs::path defPath = homePath / bfs::path{".nxe"};
    const bfs::path defFilePath = defPath / "user.conf";

    if (!bfs::exists(defPath)) {
        if(!bfs::create_directories(defPath)) {
            throw std::runtime_error("Unable to create ~/.nxe directory");
        }
    }
    return defFilePath.string();
}
}

AppSettings::AppSettings()
    : m_configPath(getConfigPath())
{
    if (bfs::exists(m_configPath)) {
        boost::property_tree::read_json(m_configPath, m_tree);
    } else {
        // create default config file
        aDebug() << "Creating default app settings under= " << m_configPath;
        set<Tags::EnablePoi>(true);
        set<Tags::Orientation>("North-up");

        save();
    }
}

AppSettings::~AppSettings()
{
    save();
}

void AppSettings::save()
{
    boost::property_tree::write_json(m_configPath, m_tree);
}

