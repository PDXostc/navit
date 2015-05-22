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
    MOCK_METHOD1(setZoom, void(int));
    MOCK_METHOD0(zoom, int(void));
    MOCK_METHOD0(speechSignal, SpeechSignalType&());
    MOCK_METHOD0(pointClickedSignal, PointClickedSignalType&());
    MOCK_METHOD0(initializedSignal, InitializedSignalType&());
    MOCK_METHOD0(routingSignal, RoutingSignalType&());
    MOCK_METHOD1(setOrientation, void(int));
    MOCK_METHOD0(orientation, int());
    MOCK_METHOD2(setCenter, void(double,double));
    MOCK_METHOD3(setDestination, void(double,double,const std::string&));
    MOCK_METHOD0(isNavigationRunning, bool());
    MOCK_METHOD0(clearDestination, void());
    MOCK_METHOD2(setPosition, void(double,double));
    MOCK_METHOD2(setPositionByInt, void(int,int));
    MOCK_METHOD2(addWaypoint, void(double,double));
    MOCK_METHOD1(setScheme, void(const std::string&));
    MOCK_METHOD0(currentCenter, NXE::Position());
    MOCK_METHOD0(startSearch, void());
    MOCK_METHOD0(finishSearch, void());
    MOCK_METHOD2(search, NXE::SearchResults(NXE::INavitIPC::SearchType, const std::string&));
    MOCK_METHOD2(selectSearchResult, void(NXE::INavitIPC::SearchType, std::int32_t));
    MOCK_METHOD3(searchPOIs, void(double,double,int));
    MOCK_METHOD1(setPitch, void(std::uint16_t));
    MOCK_METHOD0(distance, std::int32_t());
    MOCK_METHOD0(eta, std::int32_t());
};

#endif // NAVITCONTROLLERMOCK_H
