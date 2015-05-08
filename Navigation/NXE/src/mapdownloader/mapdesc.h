/*
 * mapdesc.h
 *
 *  Created on: Apr 2, 2015
 *      Author: jlr02
 */

#ifndef MAPDESC_H_
#define MAPDESC_H_

#include <string>
#include <vector>
#include <cstdint>
#include <boost/optional.hpp>
#include <boost/property_tree/ptree.hpp>

struct MapData {
	std::string name;
	std::string lon1;
	std::string lat1;
	std::string lon2;
	std::string lat2;
    // It has to be fixed size
    std::uint64_t size;
    std::uint8_t level;
    std::string continent;
};

struct MapInfo {
    const std::string name;
    const std::uint64_t size;
    const std::string continent;
};

class MapDesc {
public:
	MapDesc();
	virtual ~MapDesc();

    void setDataFilePath(const std::string &datafile);
    boost::optional<MapData> getMapData(const std::string& name);
    std::vector<MapInfo> availableMaps() const;

private:
    std::string m_datafile;
    boost::property_tree::ptree tree;
    std::vector<MapData> mapData;
};

#endif /* MAPDESC_H_ */
