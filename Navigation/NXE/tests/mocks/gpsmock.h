#ifndef GPSMOCK_H
#define GPSMOCK_H

#include "igpsprovider.h"
#include <gmock/gmock.h>

struct GPSMock : public NXE::IGPSProvider {
    using Inject = GPSMock();
    MOCK_CONST_METHOD0(position, NXE::Position());
    MOCK_METHOD1(addPostionUpdate, void(const NXE::IGPSProvider::PositionUpdateCb&));
};

#endif // GPSMOCK_H
