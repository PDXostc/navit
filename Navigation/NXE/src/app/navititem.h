#ifndef NAVITITEM_H
#define NAVITITEM_H

#include <QtQuick/QQuickPaintedItem>
namespace NXE {
class NXEInstance;
}

class NavitItem : public QQuickPaintedItem
{
    Q_PROPERTY(int zoom READ zoom WRITE setZoom NOTIFY zoomChanged)
public:
    NavitItem(QQuickItem *parent = nullptr);
    void paint(QPainter *painter) override;

private:
    void nxeCallback(const std::string& cb);
    // TODO: REMOVE THIS ASAP
    NXE::NXEInstance* instance {nullptr};
    QByteArray ba;
};

#endif // NAVITITEM_H
