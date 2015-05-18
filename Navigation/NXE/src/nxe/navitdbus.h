#ifndef NXE_NAVITDBUS_H
#define NXE_NAVITDBUS_H

#include "inavitipc.h"


namespace NXE {
class DBusController;

class NavitDBusPrivate;
class NavitDBus : public INavitIPC {
public:
    NavitDBus(DBusController& ctrl);
    ~NavitDBus();

    //! This will block until startup signal is received
    virtual void quit() override;
    virtual void setZoom(int newZoom) override;
    virtual void zoom() override;
    virtual void render() override;
    virtual void resize(int x, int y) override;
    virtual void orientation() override;
    virtual void setOrientation(int newOrientation) override;
    virtual void setCenter(double longitude, double latitude) override;
    virtual void setDestination(double longitude, double latitude, const std::string& description) override;
    virtual bool isNavigationRunning() override;
    virtual void setPosition(double longitude, double latitude) override;
    virtual void clearDestination() override;
    virtual void addWaypoint(double longitude, double latitude) override;
    virtual void setScheme(const std::string& scheme) override;
    virtual void setPitch(std::uint16_t newPitchValue) override;

    virtual void searchPOIs(double longitude, double latitude, int distance) override;
    virtual void currentCenter() override;

    virtual void startSearch() override;
    virtual void search(SearchType type, const std::string& searchString) override;
    virtual void selectSearchResult(SearchType type, std::int32_t id) override;
    virtual void finishSearch() override;
    virtual void distance() override;
    virtual void eta() override;
    virtual void setTracking(bool tracking) override;

    virtual IntSignalType &orientationResponse() override;
    virtual IntSignalType& zoomResponse() override;
    virtual EmptySignalType& searchPoiResponse() override;
    virtual CurrentCenterSignalType& currentCenterResponse() override;
    virtual SearchResultsSignalType& searchResponse() override;
    virtual IntSignalType& distanceResponse() override;
    virtual IntSignalType& etaResponse() override;
    virtual BoolSignalType& navigationChanged() override;

    virtual SpeechSignalType& speechSignal() override;
    virtual PointClickedSignalType& pointClickedSignal() override;
    virtual InitializedSignalType& initializedSignal() override;
    virtual RoutingSignalType& routingSignal() override;

private:
    std::unique_ptr<NavitDBusPrivate> d;
};

} // namespace NXE

#endif // NXE_NAVITDBUS_H
