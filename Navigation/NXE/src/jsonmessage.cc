#include "jsonmessage.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using namespace NXE;
namespace bpt = boost::property_tree;

std::string JSONUtils::serialize(NXE::JSONMessage json)
{
    bpt::ptree tree;
    tree.put("id",json.id);
    std::string buff;
    bpt::write_json(buff, tree);
    return buff;
}


JSONMessage JSONUtils::deserialize(std::string buff)
{
    bpt::ptree tree;
    bpt::read_json(buff, tree);
    return JSONMessage {tree.get<int>("id"),
                        tree.get<int>("errorCode"),
                        tree.get<std::string>("data")};
}
