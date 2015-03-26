#ifndef NAVITCONTROLLERMOCK_H
#define NAVITCONTROLLERMOCK_H

#include "navitipc.h"

struct NavitIPCMock : public NXE::NavitIPCInterface {
    GMOCK_METHOD0_(, , , start, void());
    GMOCK_METHOD0_(, , , stop, void());
    GMOCK_METHOD0_(, , , render, void());
    GMOCK_METHOD2_(, , , moveBy, void(int, int));
    GMOCK_METHOD1_(, , , zoomBy, void(int));
    GMOCK_METHOD0_(, , , zoom, int(void));
    GMOCK_METHOD0_(, , , speechSignal, SpeechSignal&());
    GMOCK_METHOD0_(, , , initializedSignal, InitializedSignal&());
};

#endif // NAVITCONTROLLERMOCK_H
