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
#include <boost/optional.hpp>
#include <boost/property_tree/ptree.hpp>

struct MapData {
	std::string name;
	std::string lon1;
	std::string lat1;
	std::string lon2;
	std::string lat2;
	long size;
	int  level;
};

class MapDesc {
public:
	MapDesc();
	virtual ~MapDesc();

    void setDataFilePath(const std::string &datafile);
    boost::optional<MapData> getMapData(const std::string& name);
    std::vector<std::string> availableMaps() const;

private:
    std::string m_datafile;
    boost::property_tree::ptree tree;
    std::vector<MapData> mapData;
};

#endif /* MAPDESC_H_ */
