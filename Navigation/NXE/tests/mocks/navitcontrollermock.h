#ifndef NAVITCONTROLLERMOCK_H
#define NAVITCONTROLLERMOCK_H

#include "inavitipc.h"

struct NavitIPCMock : public NXE::INavitIPC {
    using Inject = NavitIPCMock();

    MOCK_METHOD0(quit, void());
    MOCK_METHOD0(render, void());
    MOCK_METHOD2(resize, void(int,int));
    MOCK_METHOD2(moveBy, void(int,int));
    MOCK_METHOD1(zoomBy, void(int));
    MOCK_METHOD0(zoom, int(void));
    MOCK_METHOD0(speechSignal, SpeechSignalType&());
    MOCK_METHOD0(pointClickedSignal, PointClickedSignalType&());
    MOCK_METHOD0(initializedSignal, InitializedSignalType&());
    MOCK_METHOD1(setOrientation, void(int));
    MOCK_METHOD0(orientation, int());
    MOCK_METHOD2(setCenter, void(double,double));
    MOCK_METHOD3(setDestination, void(double,double,const std::string&));
    MOCK_METHOD0(clearDestination, void());
    MOCK_METHOD2(setPosition, void(double,double));
    MOCK_METHOD2(setPositionByInt, void(int,int));
    MOCK_METHOD2(addWaypoint, void(double,double));
    MOCK_METHOD1(setScheme, void(const std::string&));
    MOCK_METHOD0(startSearch, void());
    MOCK_METHOD0(finishSearch, void());
    MOCK_METHOD1(searchCity, std::vector<NXE::City>(const std::string&));
    MOCK_METHOD1(searchCountry, std::vector<NXE::Country>(const std::string&));
};

#endif // NAVITCONTROLLERMOCK_H
