#ifndef NAVITNAVIGATIONPROXY_H
#define NAVITNAVIGATIONPROXY_H

#include <QObject>
#include <memory>

namespace NXE{
class NXEInstance;
}

class NavitNavigationProxy : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool navigation READ navigation NOTIFY navigationChanged)
    Q_PROPERTY(int distanceToDestination READ distanceToDestination NOTIFY distanceToDestinationChanged)
    Q_PROPERTY(int eta READ eta NOTIFY etaChanged)
    Q_PROPERTY(QString currentStreet READ currentStreet NOTIFY currentStreetChanged)
public:
    NavitNavigationProxy(const std::shared_ptr<NXE::NXEInstance> &nxe, QObject *parent = 0);
    // Navigation API
    bool navigation();

    int distanceToDestination() const { return m_distance;}
    int eta() { return m_eta;}
    QString currentStreet() const {return m_currentStreet;}
signals:
    // Navigation
    void navigationChanged();
    void navigationManuver(const QString& manuverDescription);
    void distanceToDestinationChanged();
    void etaChanged();
    void currentStreetChanged();


    // Private don't use
    void requestMoveToCurrentPosition(int timeout);

public slots:
    void startNavigation(QObject* currentItem);
    void addWaypoint(QObject* item);
    void stopNavigation();
private slots:
    void moveToCurrentPositionWitTimeout(int timeout);
private:
    std::shared_ptr<NXE::NXEInstance> nxeInstance;
    int m_distance;
    int m_eta;
    QString m_currentStreet;
};

#endif // NAVITNAVIGATIONPROXY_H
