#ifndef IGPSPROVIDER_H
#define IGPSPROVIDER_H

#include "position.h"

#include <functional>

namespace NXE {

class IGPSProvider {
public:
    typedef std::function<void(const Position& pos)> PositionUpdateCb;
    virtual ~IGPSProvider() {}

    virtual Position position() const = 0;
};

} // NXE

#endif // IGPSPROVIDER_H
