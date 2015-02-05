#ifndef JSONMESSAGE_H
#define JSONMESSAGE_H

#include <string>
#include <boost/optional.hpp>

namespace NXE{

struct JSONMessage {
    // necessary fields
    const int id;
    const std::string call;

    // not necessary fields
    const boost::optional<int> errorCode;
    const boost::optional<std::string> data;
};

namespace JSONUtils {
JSONMessage deserialize(std::string buff);
std::string serialize(JSONMessage json);
}

}

#endif // JSONMESSAGE_H

