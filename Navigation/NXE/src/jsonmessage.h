#ifndef JSONMESSAGE_H
#define JSONMESSAGE_H

#include <string>

namespace NXE{

struct JSONMessage {
    const int id;
    const int errorCode;
    const std::string data;
};

namespace JSONUtils {
JSONMessage deserialize(std::string buff);
std::string serialize(JSONMessage json);
}

}

#endif // JSONMESSAGE_H

