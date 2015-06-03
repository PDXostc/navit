#include "navitnavigationproxy.h"
#include "nxe_instance.h"
#include "alog.h"
#include "locationproxy.h"

#include <QtCore/QTimer>

NavitNavigationProxy::NavitNavigationProxy(const std::shared_ptr<NXE::NXEInstance>& nxe, QObject* parent)
    : QObject(parent)
    , nxeInstance(nxe)
    , m_distance(-1)
    , m_eta(-1)
{
    connect(this, &NavitNavigationProxy::requestMoveToCurrentPosition, this, &NavitNavigationProxy::moveToCurrentPositionWitTimeout, Qt::QueuedConnection);

    nxeInstance->ipc()->routingSignal().connect([this](const std::string& manuver) {
        emit navigationManuver(QString::fromStdString(manuver));
    });
    nxeInstance->ipc()->distanceResponse().connect([this](std::int32_t distance) {
        aTrace() << "Distance received = " << distance;
        m_distance = distance;
        emit distanceToDestinationChanged();
    });

    nxeInstance->ipc()->navigationChanged().connect([this](bool navi) {
        aTrace() << "Navigation info changed to " << (navi ? "true":"false");
        if (navi) {
            aDebug() << "Zoomin to route since navigation is on";
            nxeInstance->ipc()->clearMapMarker();
        }
        emit navigationChanged();
    });

    nxeInstance->ipc()->etaResponse().connect([this](std::int32_t eta) {
        aTrace() << "Eta received " << eta;

        // this is from Navit gui_gtk_statusbar.c
        time_t _eta = time(NULL) + eta/10;
        auto remainingEta = _eta - time(NULL);
        aTrace() << _eta << " rem = " << remainingEta;

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

QObject *NavitNavigationProxy::currentNaviItem() const
{
    aDebug() << Q_FUNC_INFO << static_cast<void*>(m_currentNavigationItem.data()) << m_currentNavigationItem->itemText().toStdString();
    return m_currentNavigationItem.data();
}

void NavitNavigationProxy::startNavigation(QObject* currentItem)
{
    aInfo() << "Starting navigation";

    LocationProxy* p = qobject_cast<LocationProxy*>(currentItem);
    m_currentNavigationItem.reset(LocationProxy::clone(p));
    emit currentNaviLocationChanged();
    aDebug() << Q_FUNC_INFO << static_cast<void*>(m_currentNavigationItem.data()) << m_currentNavigationItem->itemText().toStdString();
    nxeInstance->startNavigation(p->longitude(), p->latitude(), p->description().toStdString());
}

void NavitNavigationProxy::addWaypoint(QObject* item)
{
    aInfo() << "Adding waypoint";
    LocationProxy* p = qobject_cast<LocationProxy*>(item);
    nxeInstance->ipc()->addWaypoint(p->longitude(), p->latitude());

    emit waypointAdded();
}

void NavitNavigationProxy::stopNavigation()
{
    nxeInstance->cancelNavigation();
}

void NavitNavigationProxy::moveToCurrentPositionWitTimeout(int timeout)
{
    QTimer::singleShot(timeout, [this]() {
        nxeInstance->ipc()->setTracking(true);
        auto pos = nxeInstance->gps()->position();
        nxeInstance->ipc()->setPosition(pos.longitude, pos.latitude);
    });
}
