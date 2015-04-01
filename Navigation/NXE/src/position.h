#ifndef POSITION_H
#define POSITION_H

#include <limits>

namespace NXE {

struct Position {
    double longitude{ std::numeric_limits<double>::quiet_NaN() };
    double latitude{ std::numeric_limits<double>::quiet_NaN() };
    double altitude{ std::numeric_limits<double>::quiet_NaN() };
};

} // NXE

#endif // POSITION_H
