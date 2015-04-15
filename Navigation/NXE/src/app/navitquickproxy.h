#ifndef NAVITQUICKPROXY_H
#define NAVITQUICKPROXY_H

#include <QObject>

namespace NXE {
struct JSONMessage;
}

class NavitQuickProxy : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int orientation READ orientation WRITE setOrientation NOTIFY orientationChanged)
public:
    explicit NavitQuickProxy(QObject *parent = 0);
    ~NavitQuickProxy();

    int orientation();
    void setOrientation(int);

signals:
    void orientationChanged();

public slots:
    void zoomIn();
    void zoomOut();
    void moveTo(int x, int y);
private:
    void navitCallback(const NXE::JSONMessage &msg);

    int m_orientation;
};

#endif // NAVITQUICKPROXY_H
