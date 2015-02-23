#ifndef CALLS_H
#define CALLS_H

#include <string>
#include <map>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/transform.hpp>
#include <boost/fusion/container.hpp>

enum class CallType {
    moveBy = 0,
    zoomBy,
    unknown
};

typedef std::map<CallType, std::string> CallMapType;

#endif

