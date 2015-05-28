/*
 * mapdesc.cc
 *
 *  Created on: Apr 2, 2015
 */

#include "mapdesc.h"
#include "mdlog.h"

#include <iostream>
#include <boost/foreach.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/lexical_cast.hpp>
using namespace std;

MapDesc::MapDesc()
{
}

MapDesc::~MapDesc()
{
}

void MapDesc::setDataFilePath(const string& datafile)
{
    using boost::property_tree::ptree;
    m_datafile = datafile;
    boost::property_tree::read_xml(datafile, tree);

    std::string currentContinent;
    BOOST_FOREACH (ptree::value_type const& v, tree.get_child("Mapset")) {
        if (v.first == "Mapdata") {
            const int level = v.second.get<std::uint8_t>("Level");
            if (level == 0) {
                currentContinent = v.second.get<std::string>("Name");
            }
            mapData.emplace_back(MapData{
                v.second.get<std::string>("Name"),
                v.second.get<std::string>("Lon1"),
                v.second.get<std::string>("Lat1"),
                v.second.get<std::string>("Lon2"),
                v.second.get<std::string>("Lat2"),
                v.second.get<std::uint64_t>("Size"),
                v.second.get<std::uint8_t>("Level"),
                currentContinent });
        }
    }
}

boost::optional<MapData> MapDesc::getMapData(const std::string& name)
{
    using boost::property_tree::ptree;
    MapData m;

    auto it = std::find_if(mapData.begin(), mapData.end(), [&name](const MapData& md) -> bool {
        return md.name == name;
    });

    boost::optional<MapData> md;
    if (it != mapData.end()) {
        md = *it;
    }

    return md;
}

std::vector<MapInfo> MapDesc::availableMaps() const
{
    std::vector<MapInfo> ret{};
    std::for_each(mapData.begin(), mapData.end(), [&ret](const MapData& md) {
        ret.emplace_back(MapInfo {md.name, md.size, md.continent});
    });
    return ret;
}

std::vector<MapInfo> MapDesc::recommendedMaps(const double lon, const double lat) const
{
    struct localMapInfo {
         std::string name;
         std::uint64_t size;
         std::string continent;
         double distance;
    };

    std::vector<localMapInfo> temp{};
    std::for_each(mapData.begin(), mapData.end(), [&temp, &lon, &lat](const MapData& md) {
        double lon1, lon2, lat1, lat2;
        lon1 = boost::lexical_cast<double>(md.lon1);
        lon2 = boost::lexical_cast<double>(md.lon2);
        lat1 = boost::lexical_cast<double>(md.lat1);
        lat2 = boost::lexical_cast<double>(md.lat2);
        if(lon >= lon1 && lon <= lat2 && lat >= lat1 && lat <= lat2) {
            double distance = (lon - lon1) + (lon2 - lon) + (lat - lat1) + (lat2 - lat);
            temp.emplace_back(localMapInfo {md.name, md.size, md.continent, distance});
        }
    });
    std::sort(temp.begin(), temp.end(), [](localMapInfo a, localMapInfo b) { return (a.distance < b.distance);});
    std::vector<MapInfo> ret{};
    for(auto it = temp.begin(); it != temp.end(); it++) {
        ret.emplace_back(MapInfo{it->name, it->size, it->continent});
    }
    return ret;
}
