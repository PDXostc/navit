#include "navitnavigationproxy.h"
#include "nxe_instance.h"
#include "alog.h"
#include "locationproxy.h"

NavitNavigationProxy::NavitNavigationProxy(const std::shared_ptr<NXE::NXEInstance> &nxe, QObject *parent)
    : QObject(parent)
    , nxeInstance(nxe)
    , m_distance(-1)
    , m_eta(-1)
{
    nxeInstance->ipc()->routingSignal().connect([this](const std::string& manuver) {
        emit navigationManuver(QString::fromStdString(manuver));
    });
    nxeInstance->ipc()->distanceResponse().connect([this] (std::int32_t distance) {
        m_distance = distance;
        emit distanceToDestinationChanged();
    });

    nxeInstance->ipc()->navigationChanged().connect([this](bool navi) {
        aInfo() << "Navigation info changed to " << (navi ? "true":"false");
        emit navigationChanged();
    });

    nxeInstance->ipc()->etaResponse().connect([this] (std::int32_t eta) {
        aDebug() << "Eta received " << eta;

        // this is from Navit gui_gtk_statusbar.c
        time_t _eta = time(NULL) + eta/10;
        auto remainingEta = _eta - time(NULL);
        aDebug() << _eta << " rem = " << remainingEta;

        m_eta = remainingEta;
        emit etaChanged();
    });

    nxeInstance->ipc()->currentStreetResponse().connect([this](std::string st) {
        m_currentStreet = QString::fromStdString(st);
        emit currentStreetChanged();
    });
}

bool NavitNavigationProxy::navigation()
{
    return nxeInstance->ipc()->isNavigationRunning();
}

void NavitNavigationProxy::startNavigation(QObject* currentItem)
{
    LocationProxy* p = qobject_cast<LocationProxy*>(currentItem);
    nxeInstance->startNavigation(p->longitude(), p->latitude(), p->description().toStdString());
}

void NavitNavigationProxy::addWaypoint(QObject *item)
{
    aDebug() << "Adding waypoint";
    LocationProxy* p = qobject_cast<LocationProxy*>(item);
    nxeInstance->ipc()->addWaypoint(p->longitude(), p->latitude());
}

void NavitNavigationProxy::stopNavigation()
{
    nxeInstance->cancelNavigation();
}
