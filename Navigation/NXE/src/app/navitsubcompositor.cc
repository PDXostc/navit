#include "navitsubcompositor.h"
#include "alog.h"

#include <QtCompositor/QWaylandSurface>
#include <QtCompositor/QWaylandSurfaceItem>
#include <QtQml/QQmlEngine>

NavitSubCompositor::NavitSubCompositor(const QString& socketName /*=""*/)
    : QWaylandQuickCompositor(this, socketName.isEmpty() ? 0 : socketName.toLatin1().data(), QWaylandCompositor::DefaultExtensions | QWaylandCompositor::SubSurfaceExtension)
    , m_fullscreenSurface(nullptr)
{
    engine()->setOutputWarningsToStandardError(true);
    setResizeMode(QQuickView::SizeRootObjectToView);
    setColor(Qt::white);
    winId();

    addDefaultShell();

    connect(this, &NavitSubCompositor::afterRendering, this, &NavitSubCompositor::sendCallbacks);
    connect(engine(), &QQmlEngine::warnings, [](const QList<QQmlError>& warns) {
        std::for_each(warns.begin(), warns.end(), []( const QQmlError& err) {
            qDebug() << err;
//            aError() << err.toString().toLatin1().data();
        });
    });
}

QWaylandSurfaceItem *NavitSubCompositor::item(QWaylandSurface *surf)
{
    QWaylandSurfaceItem* ret = static_cast<QWaylandSurfaceItem*> (surf->views().first());
    ret->setSize(size());
    aTrace() << "Item " << static_cast<void*>(surf) << " first= " << static_cast<void*>(ret) << " it's size= " << ret->width() << "x" << ret->height();
    return ret;
}

QWaylandQuickSurface *NavitSubCompositor::fullscreenSurface() const
{
    return m_fullscreenSurface;
}

void NavitSubCompositor::setFullscreenSurface(QWaylandQuickSurface *surface)
{
    m_fullscreenSurface = surface;
    fullscreenSurfaceChanged();
}

void NavitSubCompositor::surfaceCreated(QWaylandSurface* surface)
{
    aInfo() << "An surface was created";
    connect(surface, &QWaylandSurface::surfaceDestroyed, this, &NavitSubCompositor::surfaceDestroyed);
    connect(surface, &QWaylandSurface::mapped, this, &NavitSubCompositor::surfaceMapped);
    connect(surface, &QWaylandSurface::unmapped, this, &NavitSubCompositor::surfaceUnmapped);
}

void NavitSubCompositor::sendCallbacks()
{
    sendFrameCallbacks(surfaces());
}

void NavitSubCompositor::surfaceMapped()
{
    QWaylandSurface* surface = qobject_cast<QWaylandSurface*>(sender());
    aTrace() << "Window added" << static_cast<void*>(surface) << " size= " << surface->size().width() << "x" << surface->size().height();
    emit windowAdded(QVariant::fromValue(surface));
}

void NavitSubCompositor::surfaceUnmapped()
{
    QWaylandSurface* surface = qobject_cast<QWaylandSurface*>(sender());
    aTrace() << "Surface unmapped" << static_cast<void*>(surface);
}

void NavitSubCompositor::surfaceDestroyed()
{
    QWaylandSurface* surface = qobject_cast<QWaylandSurface*>(sender());
    aTrace() << "Surface destroyed" << static_cast<void*>(surface);
}

void NavitSubCompositor::resizeEvent(QResizeEvent *event)
{
    QQuickView::resizeEvent(event);
    QWaylandCompositor::setOutputGeometry(QRect(0,0,width(), height()));
}
