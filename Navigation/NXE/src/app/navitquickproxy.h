#ifndef NAVITQUICKPROXY_H
#define NAVITQUICKPROXY_H

#include <QObject>

class NavitQuickProxy : public QObject
{
    Q_OBJECT
public:
    explicit NavitQuickProxy(QObject *parent = 0);
    ~NavitQuickProxy();

signals:

public slots:
    void zoomIn();
    void zoomOut();
};

#endif // NAVITQUICKPROXY_H
