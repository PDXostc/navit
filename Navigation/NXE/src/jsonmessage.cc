#include "jsonmessage.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <iostream>

using namespace NXE;
namespace bpt = boost::property_tree;

std::string JSONUtils::serialize(NXE::JSONMessage json)
{
    bpt::ptree tree;
    tree.put("id",json.id);
    tree.put("call",json.call);

    if (json.errorCode) {
        tree.put("errorCode", json.errorCode.value());
    }

    if (json.data) {
        tree.put("data", json.data.value());
    }

    std::stringstream buff;
    bpt::write_json(buff, tree);
    return buff.str();
}


JSONMessage JSONUtils::deserialize(std::string buff)
{
    bpt::ptree tree;
    std::stringstream stream;
    stream << buff;
    std::istringstream is(stream.str());
    bpt::read_json(is, tree);
    boost::optional<int> errorCode = tree.get_optional<int>("errorCode");
    boost::optional<std::string> call = tree.get_optional<std::string>("call");
    boost::optional<std::string> data = tree.get_optional<std::string>("data");
    return JSONMessage {tree.get<int>("id"),
                        tree.get<std::string>("call"),
                        errorCode,
                        data};
}
