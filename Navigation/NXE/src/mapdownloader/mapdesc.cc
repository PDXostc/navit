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
#include <boost/property_tree/ptree.hpp>

using namespace std;

MapDesc::MapDesc() {
}

MapDesc::~MapDesc() {
}

bool MapDesc::getMapData(const std::string& name, const std::string& datafile, MapData& m) {

	using boost::property_tree::ptree;
	ptree pt;

	std::filebuf fb;

	if (!fb.open(datafile.c_str(),std::ios::in)) {
		return false;
	}

	std::istream is(&fb);

	read_xml(is, pt);

	BOOST_FOREACH( ptree::value_type const& v, pt.get_child("Mapset") ) {
		if( v.first == "Mapdata" ) {
			std::string n = v.second.get<std::string>("Name");
	        if (n == name) {
	        	m.name = v.second.get<std::string>("Name");
	        	m.lon1 = v.second.get<std::string>("Lon1");
	        	m.lat1 = v.second.get<std::string>("Lat1");
	        	m.lon2 = v.second.get<std::string>("Lon2");
	        	m.lat2 = v.second.get<std::string>("Lat2");
	        	m.size = v.second.get<long>("Size");
	        	m.level = v.second.get<int>("Level");

                mdDebug() << "Found map:" << m.name << ", lon1: " << m.lon1 << ", lat1: "
	        			<<  m.lat1 << ", lon2: " << m.lon2 << ", lat2: " <<  m.lat2
	        			<<  ", size: " << m.size << ", level: " << m.level;

	        	fb.close();
	        	return true;
	        }
		}
	}

	fb.close();
	return false;
}
