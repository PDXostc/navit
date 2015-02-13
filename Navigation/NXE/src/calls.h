#ifndef CALLS_H
#define CALLS_H

#include <string>
#include <map>

enum class CallType {
    moveBy = 0,

    unknown
};

typedef std::map<CallType, std::string> CallMapType;


#endif

