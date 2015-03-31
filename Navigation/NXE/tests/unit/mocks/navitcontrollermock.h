#ifndef NAVITCONTROLLERMOCK_H
#define NAVITCONTROLLERMOCK_H

#include "navitipc.h"

struct NavitIPCMock : public NXE::NavitIPCInterface
{
    using Inject = NavitIPCMock();
    GMOCK_METHOD0_(, , , start, void());
    GMOCK_METHOD0_(, , , stop, void());
    GMOCK_METHOD0_(, , , quit, void());
    GMOCK_METHOD0_(, , , render, void());
    GMOCK_METHOD2_(, , , moveBy, void(int, int));
    GMOCK_METHOD1_(, , , zoomBy, void(int));
    GMOCK_METHOD0_(, , , zoom, int(void));
    GMOCK_METHOD0_(, , , speechSignal, SpeechSignal&());
    GMOCK_METHOD0_(, , , initializedSignal, InitializedSignal&());
    MOCK_METHOD1(setOrientation, void(int));
    MOCK_METHOD0(orientation, int());
};

#endif // NAVITCONTROLLERMOCK_H
