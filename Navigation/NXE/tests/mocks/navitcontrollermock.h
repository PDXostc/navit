#ifndef NAVITCONTROLLERMOCK_H
#define NAVITCONTROLLERMOCK_H

#include "inavitipc.h"

struct NavitIPCMock : public NXE::INavitIPC {
    using Inject = NavitIPCMock();

    MOCK_METHOD0(quit, void());
    MOCK_METHOD0(render, void());
    MOCK_METHOD2(moveBy, void(int,int));
    MOCK_METHOD1(zoomBy, void(int));
    MOCK_METHOD0(zoom, int(void));
    MOCK_METHOD0(speechSignal, SpeechSignal&());
    MOCK_METHOD0(initializedSignal, InitializedSignal&());
    MOCK_METHOD1(setOrientation, void(int));
    MOCK_METHOD0(orientation, int());
    MOCK_METHOD2(setCenter, void(double,double));
    MOCK_METHOD3(setDestination, void(double,double,const std::string&));
    MOCK_METHOD0(clearDestination, void());
    MOCK_METHOD2(setPosition, void(double,double));
};

#endif // NAVITCONTROLLERMOCK_H
