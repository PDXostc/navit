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

    BOOST_FOREACH (ptree::value_type const& v, tree.get_child("Mapset")) {
        if (v.first == "Mapdata") {
            mapData.emplace_back(MapData {
                                     v.second.get<std::string>("Name"),
                                     v.second.get<std::string>("Lon1"),
                                     v.second.get<std::string>("Lat1"),
                                     v.second.get<std::string>("Lon2"),
                                     v.second.get<std::string>("Lat2"),
                                     v.second.get<long>("size"),
                                     v.second.get<int>("Level"),
                                 });
        }
    }
}

boost::optional<MapData> MapDesc::getMapData(const std::string& name)
{
    using boost::property_tree::ptree;
    MapData m;

    auto it = std::find_if(mapData.begin(), mapData.end(), [&name](const MapData &md) -> bool {
        return md.name == name;
    });

    auto bo = it != mapData.end() ? boost::optional<MapData> {} :
                                    boost::optional<MapData> {*it};
    return bo;
}

std::vector<std::string> MapDesc::availableMaps() const
{
    return std::vector<std::string>{};
}
