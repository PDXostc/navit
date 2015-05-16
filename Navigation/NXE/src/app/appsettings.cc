#include "appsettings.h"
#include "alog.h"
#include "locationproxy.h"

#include <boost/property_tree/json_parser.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/range/iterator_range.hpp>

namespace bfs = boost::filesystem;

namespace {
std::pair<std::string, std::string> getConfigPath()
{
    const bfs::path homePath{ getenv("HOME") };
    if (!bfs::exists(homePath)) {
        throw std::runtime_error("HOME does not exists");
    }

    const bfs::path defPath = homePath / bfs::path{ ".NavIt" };
    const bfs::path favPaths = defPath / bfs::path{ "favorites" };
    const bfs::path defFilePath = defPath / "user.conf";

    if (!bfs::exists(defPath)) {
        if (!bfs::create_directories(defPath) || !bfs::create_directories(favPaths)) {
            throw std::runtime_error("Unable to create ~/.NavIt directory");
        }
    }
    if (!bfs::exists(favPaths)) {
        if (!bfs::create_directories(favPaths)) {
            throw std::runtime_error("Unable to create ~/.NavIt/favorites directory");
        }
    }
    return std::make_pair(defFilePath.string(), favPaths.string());
}
}

AppSettings::AppSettings()
    : m_configPath(getConfigPath().first)
    , m_favoritesPath(getConfigPath().second)
{
    if (bfs::exists(m_configPath)) {
        try {
            boost::property_tree::read_json(m_configPath, m_tree);
        } catch(const std::exception&) {
            // user.conf is not valid
            bfs::remove(m_configPath);
            createDefaults();
            boost::property_tree::read_json(m_configPath, m_tree);
        }
    }
    else {
        // create default config file
        createDefaults();
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

void AppSettings::addToFavorites(LocationProxy* proxy)
{
    using boost::property_tree::ptree;
    using boost::property_tree::write_json;
    const std::string name = "fav_" + std::string{ proxy->id().toByteArray().data() };
    bfs::path favPath{ bfs::path{ m_favoritesPath } / name };

    aInfo() << "Adding " << name << " to from favorites"
            << " under " << favPath.string();
    ptree favTree;
    favTree.add("id", proxy->id().toByteArray().data());
    favTree.add("itemText", proxy->itemText().toStdString());
    favTree.add("description", proxy->description().toStdString());
    favTree.add("longitude", proxy->longitude());
    favTree.add("latitude", proxy->latitude());
    write_json(favPath.string(), favTree);
}

void AppSettings::removeFromFavorites(const std::string& id)
{
    aInfo() << "Remove " << id << " from favorites";
}

void AppSettings::remove()
{
    using boost::property_tree::read_json;
    // remove ~/.NavIt/user.conf
    bfs::remove(m_configPath);
    bfs::remove_all(m_favoritesPath);

    getConfigPath();
    createDefaults();

    read_json(m_configPath, m_tree);
}

QList<LocationProxy*> AppSettings::favorites()
{
    aInfo() << "Reading favorites";
    bfs::directory_iterator dirIt{ m_favoritesPath };
    QList<LocationProxy*> favs;

    for (auto& entry : boost::make_iterator_range(dirIt, {})) {
        using boost::property_tree::ptree;
        using boost::property_tree::read_json;
        ptree entryTree;
        aInfo() << "Reading " << entry;
        read_json(entry.path().string(), entryTree);

        auto loc = new LocationProxy{
            QString::fromStdString(entryTree.get<std::string>("itemText")),
            true,
            QString::fromStdString(entryTree.get<std::string>("description")),
            false
        };
        loc->setPosition(NXE::Position{ entryTree.get<double>("longitude"), entryTree.get<double>("latitude") });
        favs.append(loc);
    }

    return favs;
}

void AppSettings::createDefaults()
{
    aDebug() << "Creating default app settings under= " << m_configPath;
    set<Tags::EnablePoi>(true);
    set<Tags::Orientation>(-1);
    set<Tags::Ftu>(true);
    set<Tags::Voice>(true);
    set<Tags::MapView>("2D");
    save();
}
