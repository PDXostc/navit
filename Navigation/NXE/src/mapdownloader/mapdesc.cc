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
