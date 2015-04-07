/*
 * mapdesc.h
 *
 *  Created on: Apr 2, 2015
 *      Author: jlr02
 */

#ifndef MAPDESC_H_
#define MAPDESC_H_

#include <string>

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

	bool getMapData(const std::string& name, const std::string& datafile, MapData& res);
};

#endif /* MAPDESC_H_ */
