#ifndef JSONMESSAGE_H
#define JSONMESSAGE_H

#include <string>
#include <map>
#include <boost/optional.hpp>
#include <boost/multi_index_container.hpp>

#include "calls.h"

namespace NXE{

struct JSONMessage {
    // necessary fields
    const int id;
    const CallType call;

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

