#include "jsonmessage.h"
#include "log.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <iostream>

using namespace NXE;
namespace bpt = boost::property_tree;

namespace {
}

std::string JSONUtils::serialize(NXE::JSONMessage json)
{
    bpt::ptree tree;
    tree.put("id", json.id);
    tree.put("call", json.call);

    if (!json.error.empty()) {
        tree.put("error", json.error);
    }

    if (!json.data.empty()) {
        tree.put_child("data", json.data);
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
    boost::optional<std::string> error = tree.get_optional<std::string>("error");
    const std::string errorMsg { error ? error.value(): "" };

    // this will throw an exception if `call` is not present
    std::string call = tree.get<std::string>("call");
    bpt::ptree data;
    if (tree.find("data") != tree.not_found()) {
        data = tree.get_child("data");
    }
    return JSONMessage{ tree.get<std::uint32_t>("id"),
                        call,
                        errorMsg,
                        data };
}

std::string NXE::JSONUtils::serialize(std::uint32_t id, const std::string& call, const std::string &err, boost::property_tree::ptree data)
{
    JSONMessage m{ id, call, err, data };
    return serialize(m);
}
