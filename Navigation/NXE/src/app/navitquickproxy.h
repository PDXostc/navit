#ifndef NAVITQUICKPROXY_H
#define NAVITQUICKPROXY_H

#include <QtCore/QObject>
#include <memory>

namespace NXE {
struct JSONMessage;
class NXEInstance;
}
struct Context;

class NavitQuickProxy : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int orientation READ orientation WRITE setOrientation NOTIFY orientationChanged)
public:
    explicit NavitQuickProxy(const QString& socketName, QObject *parent = 0);

    int orientation();
    void setOrientation(int);

signals:
    void orientationChanged();

public slots:
    void zoomIn();
    void zoomOut();
    void moveTo(int x, int y);
    void render();

private:
    std::shared_ptr<Context> context;
    std::shared_ptr<NXE::NXEInstance> nxeInstance;
    int m_orientation;
};

#endif // NAVITQUICKPROXY_H
