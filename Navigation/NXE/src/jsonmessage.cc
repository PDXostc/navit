#include "jsonmessage.h"
#include "log.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <iostream>

using namespace NXE;
namespace bpt = boost::property_tree;

namespace {
//const CallMapType types{
//    { CallType::moveBy, "moveBy" },
//    { CallType::zoomBy, "zoomBy" }
//    };
}

std::string JSONUtils::serialize(NXE::JSONMessage json)
{
    bpt::ptree tree;
    tree.put("id", json.id);
    tree.put("call",json.call);

    if (json.errorCode) {
        tree.put("errorCode", json.errorCode.get());
    }

    if (json.data) {
        tree.put("data", json.data.get());
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
    nDebug() << "JSON read";
    boost::optional<int> errorCode = tree.get_optional<int>("errorCode");

    // this will throw an exception if `call` is not present
    std::string call = tree.get<std::string>("call");
    boost::optional<std::string> data = tree.get_optional<std::string>("data");

//    CallMapType::const_iterator it = std::find_if(types.begin(), types.end(), [&call](const std::pair<CallType, std::string>& pair) -> bool {
//                return pair.second == call;
//    });

    return JSONMessage{ tree.get<std::uint32_t>("id"),
//                        it != types.end() ? it->first : CallType::unknown,
                        call,
                        errorCode,
                        data };
}


std::string NXE::JSONUtils::serialize(std::uint32_t id, const std::string &call, int ec, std::string data)
{
    JSONMessage m {id, call, ec, data};
    return serialize(m);
}
