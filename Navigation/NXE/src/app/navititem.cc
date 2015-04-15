#include "navititem.h"
#include "nxe_instance.h"
#include "alog.h"
#include "jsonmessage.h"

#include <QtGui/QPainter>

#include <functional>

extern NXE::NXEInstance* g_nxeInstance;

NavitItem::NavitItem(QQuickItem* parent)
    : QQuickPaintedItem(parent)
{
    Q_ASSERT(g_nxeInstance);
    instance = g_nxeInstance;

    instance->registerMessageCallback(std::bind(&NavitItem::nxeCallback, this, std::placeholders::_1));
    aInfo() << "Global instance addr= " << static_cast<void*>(instance);

    NXE::JSONMessage msg{ 0, "render" };
    instance->HandleMessage(msg);
}

void NavitItem::paint(QPainter* painter)
{
    if (ba.isEmpty()) {
        aTrace() << "buffer empty, do anything";
        return;
    }

//    QBrush brush(QColor("#007430"));

//    painter->setBrush(brush);
//    painter->setPen(Qt::NoPen);
//    painter->setRenderHint(QPainter::Antialiasing);

//    const QPointF points[3] = {
//        QPointF(10.0, boundingRect().height() - 10.0),
//        QPointF(20.0, boundingRect().height()),
//        QPointF(30.0, boundingRect().height() - 10.0),
//    };
//    painter->drawConvexPolygon(points, 3);

    aInfo() << "Yeah rendering! ba size= " << ba.size();
    aInfo() << (int)ba[0] << (int)ba[1] << (int)ba[2] << (int)ba[3];
    QImage img{ (uchar*)ba.data(), 1080, 1600, QImage::Format_RGB32 };
    if (img.isNull()) {
        aError() << "null!!!";
    }
    QPixmap p = QPixmap::fromImage(img);
    painter->drawPixmap(0, 0, width(), height(), p);

}

void NavitItem::nxeCallback(const std::string& cb)
{
    if (cb.size() == 7171200) {
        aInfo() << "Navit callback!";
        ba = QByteArray::fromStdString(cb);
        update();
    }
}
