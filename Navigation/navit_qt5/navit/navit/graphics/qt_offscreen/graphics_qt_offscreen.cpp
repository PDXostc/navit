#include <glib.h>
#include <cstring>
#include "graphics_qt_offscreen.h"
#include "navit/window.h"
#include "navit/event.h"
#include "navit/callback.h"
#include "config.h"
#include "plugin.h"

#include <fcntl.h> /* Defines O_ * constants */
#include <sys/stat.h> /* Defines mode constants */
#include <sys/mman.h>
#include <sys/file.h>
#include <unistd.h>
#include <sys/types.h>

#include <QtCore/QtDebug>
#include <QtCore/QEventLoop>
#include <QtCore/QElapsedTimer>

#include <QtGui/QPen>
#include <QtGui/QBrush>
#include <QtGui/QPixmapCache>
#include <QtGui/QFont>
#include <QtGui/QPainter>
#include <QtGui/QPaintEngine>
#include <QtGui/QOpenGLContext>
#include <QtGui/QOpenGLPaintDevice>
#include <QtGui/QWindow>
#include <QtOpenGL/QGLFramebufferObject>
#include <QtOpenGL/QGLFramebufferObjectFormat>

#include <QtWidgets/QApplication>

void qt_offscreen_draw(graphics_priv* gr);
void event_qt_remove_timeout(event_timeout*);

namespace {
const std::uint16_t defaultWidth = 1900;
const std::uint16_t defaultHeight = 1080;
const std::string sharedMemoryName = "Navit_shm";
int sharedMemoryFd = -1;

// For testing purposes only
struct Counter {
    int polygons = 0;
    int lines = 0;
    int text = 0;
    QElapsedTimer draw;
};

static Counter ctrs;
}

template <typename T>
QDebug operator<<(QDebug dbg, const std::unique_ptr<T>& ptr)
{
    dbg.space() << ptr.get();
    return dbg.space();
}

QDebug operator<<(QDebug dbg, const Counter& ctrs)
{
    dbg.nospace() << "Time= " << ctrs.draw.elapsed() << " Polygons = " << ctrs.polygons << " lines= " << ctrs.lines << " text= " << ctrs.text;
    return dbg.space();
}

bool backendOpenGL()
{
    const QByteArray val = qgetenv("OFFSCREEN_OPENGL");
    if (val.isEmpty())
        return false;
    return val.toInt() == 1;
}

bool dumpImage()
{
    const QByteArray val = qgetenv("OFFSCREEN_DUMP");
    if (val.isEmpty())
        return false;
    return val.toInt() == 1;
}

// This sets up QPainter based on whether OpenGL is on/off
void setupQtPainter(graphics_priv* ret)
{
    qDebug() << Q_FUNC_INFO;
    if (ret->opengl) {
        QSurfaceFormat fmt;

        ret->window.reset(new QWindow);
        ret->window->setSurfaceType(QWindow::OpenGLSurface);
        ret->window->setFormat(fmt);
        ret->window->create();

        ret->context.reset(new QOpenGLContext);
        ret->context->setFormat(fmt);
        if (!ret->context->create())
            qFatal("NO!!");
        ret->context->makeCurrent(ret->window.get());

        const QRect drawRect(0, 0, defaultWidth, defaultHeight);
        const QSize drawRectSize = drawRect.size();

        QGLFramebufferObjectFormat fboFormat;
        fboFormat.setSamples(16);
        fboFormat.setAttachment(QGLFramebufferObject::CombinedDepthStencil);

        ret->fbo.reset(new QGLFramebufferObject(drawRectSize, fboFormat));
        ret->fbo->bind();

        ret->glBuffer.reset(new QOpenGLPaintDevice(drawRectSize));
        ret->painter.reset(new QPainter(ret->glBuffer.get()));
        ret->buffer = ret->glBuffer.get();
    } else {
        ret->pixmapBuffer.reset(new QPixmap(defaultWidth, defaultHeight));
        ret->pixmapBuffer->fill();
        ret->painter.reset(new QPainter(ret->pixmapBuffer.get()));
        ret->buffer = ret->pixmapBuffer.get();
    }

    // set up shared memory
    sharedMemoryFd = shm_open(sharedMemoryName.c_str(), O_CREAT | O_TRUNC | O_RDWR, 0666);
    if (sharedMemoryFd == -1) {
        qFatal("Unable to open shm");
        return;
    }

    if (ftruncate(sharedMemoryFd, defaultWidth * defaultHeight * 4) != 0) {
        qFatal("Unable to open shm");
        return;
    }

    qDebug() << "Finished set up graphics" << ret << ret->buffer << "shared mem=";
}

void
qt_offscreen_draw(graphics_priv* gr)
{
    static int count = 0;
    QImage img;
    if (gr->opengl) {
        img = gr->fbo->toImage();
    }
    else {
        img = gr->pixmapBuffer->toImage();
    }

    qDebug() << "Copying to buffer. Buff size="
             << "image size" << img.byteCount();

    void *to = mmap(0, img.byteCount(), PROT_READ | PROT_WRITE, MAP_SHARED, sharedMemoryFd, 0);
    const char *from = reinterpret_cast<char*>(img.bits());
    std::memcpy(to, from, img.byteCount());
    char *cc = reinterpret_cast<char*>(to);

    qDebug() << "First 4 bytes"
             <<(int)(cc[0])
             <<(int)(cc[1])
             <<(int)(cc[2])
             <<(int)(cc[3]);

    if (gr->dumpFrame) {
        const QString frame = QString("/tmp/frame%1.png").arg(count);
        qWarning() << "Saving frame " << frame;
        img.save(frame);
    }

    qWarning() << "[" << count++ << "]" << ctrs;
    ctrs.polygons = 0;
    ctrs.lines = 0;
    ctrs.text = 0;
}

struct graphics_font_priv {
    QFont* font;
};

struct graphics_image_priv {
    QPixmap* pixmap;
};

static void graphics_destroy(graphics_priv* gr)
{
    gr->freetype_methods.destroy();
    shm_unlink(sharedMemoryName.c_str());
    delete gr;
}

static void font_destroy(struct graphics_font_priv* font)
{
}

static struct graphics_font_methods font_methods = {
    font_destroy
};

static graphics_font_priv* font_new(graphics_priv* gr, graphics_font_methods* meth, char* fontfamily, int size, int flags)
{
    graphics_font_priv* ret = new graphics_font_priv;
    ret->font = new QFont("Arial", size / 20);
    *meth = font_methods;
    return ret;
}

static void gc_destroy(graphics_gc_priv* gc)
{
    delete gc->pen;
    delete gc->brush;
    delete gc;
}

static void gc_set_linewidth(graphics_gc_priv* gc, int w)
{
    gc->pen->setWidth(w);
}

static void gc_set_dashes(graphics_gc_priv* gc, int w, int offset, unsigned char* dash_list, int n)
{
}

static void gc_set_foreground(graphics_gc_priv* gc, struct color* c)
{
    QColor col(c->r >> 8, c->g >> 8, c->b >> 8 /* , c->a >> 8 */);
    gc->pen->setColor(col);
    gc->brush->setColor(col);
    gc->c = *c;
}

static void gc_set_background(graphics_gc_priv* gc, color* c)
{
}

static struct graphics_gc_methods gc_methods = {
    gc_destroy,
    gc_set_linewidth,
    gc_set_dashes,
    gc_set_foreground,
    gc_set_background
};

static graphics_gc_priv* gc_new(graphics_priv* gr, graphics_gc_methods* meth)
{
    *meth = gc_methods;
    graphics_gc_priv* ret = new graphics_gc_priv;
    ret->pen = new QPen();
    ret->brush = new QBrush(Qt::SolidPattern);
    return ret;
}

static struct graphics_image_priv* image_new(graphics_priv* gr, graphics_image_methods* meth, char* path, int* w, int* h, struct point* hot, int rotation)
{
    struct graphics_image_priv* ret;
    QPixmap* cachedPixmap;
    QString key(path);

    ret = new graphics_image_priv;

    cachedPixmap = QPixmapCache::find(key);
    if (!cachedPixmap) {
        ret->pixmap = new QPixmap(path);
        if (ret->pixmap->isNull()) {
            delete ret;
            return nullptr;
        }

        QPixmapCache::insert(key, QPixmap(*ret->pixmap));
    } else {
        ret->pixmap = new QPixmap(*cachedPixmap);
    }

    *w = ret->pixmap->width();
    *h = ret->pixmap->height();
    if (hot) {
        hot->x = *w / 2;
        hot->y = *h / 2;
    }

    return ret;
}

static void draw_lines(graphics_priv* gr, graphics_gc_priv* gc, point* p, int count)
{
    ctrs.lines++;
    int i;
    static QPolygon polygon;
    polygon.resize(count);

    for (i = 0; i < count; i++)
        polygon.setPoint(i, p[i].x, p[i].y);
    gr->painter->setPen(*gc->pen);
    gr->painter->drawPolyline(polygon);
}

static void draw_polygon(graphics_priv* gr, graphics_gc_priv* gc, point* p, int count)
{
    int i;
    QPolygon polygon;
    ctrs.polygons++;

    for (i = 0; i < count; i++)
        polygon.putPoints(i, 1, p[i].x, p[i].y);
    gr->painter->setPen(*gc->pen);
    gr->painter->setBrush(*gc->brush);
    gr->painter->drawConvexPolygon(polygon);
}

static void draw_rectangle(graphics_priv* gr, graphics_gc_priv* gc, point* p, int w, int h)
{
    gr->painter->fillRect(p->x, p->y, w, h, *gc->brush);
}

static void draw_circle(graphics_priv* gr, graphics_gc_priv* gc, point* p, int r)
{
    gr->painter->setPen(*gc->pen);
    gr->painter->drawArc(p->x - r / 2, p->y - r / 2, r, r, 0, 360 * 16);
}

static void draw_text(graphics_priv* gr, graphics_gc_priv* fg, graphics_gc_priv* bg, graphics_font_priv* font, char* text, point* p, int dx, int dy)
{
    ctrs.text++;
    font_freetype_text* t;
    font_freetype_glyph* g, **gp;
    color transparent = { 0x0000, 0x0000, 0x0000, 0x0000 };
    color* fgc = &fg->c, *bgc = &bg->c;

    int i, x, y;

    if (!font)
        return;
    t = gr->freetype_methods.text_new(text, (struct font_freetype_font*)font, dx, dy);
    x = p->x << 6;
    y = p->y << 6;
    gp = t->glyph;
    i = t->glyph_count;
    if (bg) {
        while (i-- > 0) {
            g = *gp++;
            if (g->w && g->h) {
                unsigned char* data;
                QImage img(g->w + 2, g->h + 2, QImage::Format_ARGB32_Premultiplied);
                data = img.bits();
                gr->freetype_methods.get_shadow(g, (unsigned char*)data, 32, img.bytesPerLine(), bgc, &transparent);
                gr->painter->drawImage(((x + g->x) >> 6) - 1, ((y + g->y) >> 6) - 1, img);
            }
            x += g->dx;
            y += g->dy;
        }
    } else
        bgc = &transparent;
    x = p->x << 6;
    y = p->y << 6;
    gp = t->glyph;
    i = t->glyph_count;
    while (i-- > 0) {
        g = *gp++;
        if (g->w && g->h) {
            unsigned char* data;
            QImage img(g->w, g->h, QImage::Format_ARGB32_Premultiplied);
            data = img.bits();
            gr->freetype_methods.get_glyph(g, (unsigned char*)data, 32, img.bytesPerLine(), fgc, bgc, &transparent);
            gr->painter->drawImage((x + g->x) >> 6, (y + g->y) >> 6, img);
        }
        x += g->dx;
        y += g->dy;
    }
    gr->freetype_methods.text_destroy(t);
}

static void draw_image(graphics_priv* gr, graphics_gc_priv* fg, point* p, graphics_image_priv* img)
{
    gr->painter->drawPixmap(p->x, p->y, *img->pixmap);
}

static void background_gc(graphics_priv* gr, graphics_gc_priv* gc)
{
    gr->background_gc = gc;
    gr->rgba[2] = gc->c.r >> 8;
    gr->rgba[1] = gc->c.g >> 8;
    gr->rgba[0] = gc->c.b >> 8;
    gr->rgba[3] = gc->c.a >> 8;
}

static void draw_mode(graphics_priv* gr, draw_mode_num mode)
{
    if (gr == nullptr || gr->buffer == nullptr) {
        qWarning() << "Graphics not ready yet" << gr << gr->buffer;
        return;
    }
    qDebug() << Q_FUNC_INFO << gr << gr->buffer;
    if (mode == draw_mode_begin) {
        ctrs.draw.start();
        if (gr->buffer->paintingActive()) {
            gr->buffer->paintEngine()->painter()->end();
        }
        gr->painter->begin(gr->buffer);
    }
    if (mode == draw_mode_end) {
        gr->painter->end();
        qt_offscreen_draw(gr);
    }
    gr->mode = mode;
}

static int argc = 1;
static char* argv[] = { nullptr, nullptr, nullptr };

static void
disable_suspend(window* win)
{
}

static void* get_data(graphics_priv* this_, const char* type)
{
    struct window* win;

    if (!strcmp(type, "window")) {
        win = new window;
        //        this_->buffer->paintEngine()->painter()->end();
        callback_list_call_attr_2(this_->cbl, attr_resize, defaultWidth, defaultHeight);
        win->priv = this_;
        win->fullscreen = nullptr;
        win->disable_suspend = disable_suspend;
        return win;
    }
    return nullptr;
}

static void
image_free(graphics_priv* gr, graphics_image_priv* priv)
{
    delete priv->pixmap;
    delete priv;
}

static void
get_text_bbox(graphics_priv* gr, graphics_font_priv* font, char* text, int dx, int dy, point* ret, int estimate)
{
    QString tmp = QString::fromUtf8(text);
    gr->painter->setFont(*font->font);
    QRect r = gr->painter->boundingRect(0, 0, gr->w, gr->h, 0, tmp);
    ret[0].x = 0;
    ret[0].y = -r.height();
    ret[1].x = 0;
    ret[1].y = 0;
    ret[2].x = r.width();
    ret[2].y = 0;
    ret[3].x = r.width();
    ret[3].y = -r.height();
}

static void overlay_disable(graphics_priv* gr, int disable)
{
    gr->overlay_disable = disable;
}

static struct graphics_methods graphics_methods = {
    graphics_destroy,
    draw_mode,
    draw_lines,
    draw_polygon,
    draw_rectangle,
    draw_circle,
    draw_text,
    draw_image,
    nullptr,
    nullptr,
    font_new,
    gc_new,
    background_gc,
    overlay_new,
    image_new,
    get_data,
    image_free,
    get_text_bbox,
    overlay_disable,
    nullptr,
    nullptr,
};

static graphics_priv* overlay_new(struct graphics_priv* gr, struct graphics_methods* meth, struct point* p, int w, int h, int alpha, int wraparound)
{
    qDebug() << Q_FUNC_INFO;
    *meth = graphics_methods;
    struct graphics_priv* ret = new graphics_priv;
    if (gr->font_freetype_new) {
        ret->font_freetype_new = gr->font_freetype_new;
        gr->font_freetype_new(&ret->freetype_methods);
        meth->font_new = (struct graphics_font_priv * (*)(struct graphics_priv*, struct graphics_font_methods*, char*, int, int))ret->freetype_methods.font_new;
        meth->get_text_bbox = (void (*)(struct graphics_priv*, struct graphics_font_priv*, char*, int, int, struct point*, int))ret->freetype_methods.get_text_bbox;
    }
    ret->wraparound = wraparound;
    ret->painter.reset(new QPainter);
    ret->p = *p;
    ret->parent = gr;
    ret->next = gr->overlays;
    gr->overlays = ret;
    return ret;
}

static graphics_priv* event_gr;

static void event_qt_main_loop_run()
{
    event_gr->app->exec();
}

static void event_qt_main_loop_quit()
{
    exit(0);
}

static event_watch* event_qt_add_watch(int fd, event_watch_cond cond, callback* cb)
{
    event_watch* ret = new event_watch;
    ret->fd = fd;
    ret->cb = cb;
    return ret;
}

static void
event_qt_remove_watch(event_watch* ev)
{
    delete ev;
}

static event_timeout*
event_qt_add_timeout(int timeout, int multi, callback* cb)
{
    return nullptr;
}

void
event_qt_remove_timeout(event_timeout*)
{
}

static event_idle*
event_qt_add_idle(int priority, callback* cb)
{
    return (event_idle*)event_qt_add_timeout(0, 1, cb);
}

static void
event_qt_remove_idle(event_idle* ev)
{
    event_qt_remove_timeout((event_timeout*)ev);
}

static void
event_qt_call_callback(callback_list* )
{
}

static event_methods event_qt_methods = {
    event_qt_main_loop_run,
    event_qt_main_loop_quit,
    event_qt_add_watch,
    event_qt_remove_watch,
    event_qt_add_timeout,
    event_qt_remove_timeout,
    event_qt_add_idle,
    event_qt_remove_idle,
    event_qt_call_callback,
};

struct event_priv {
};

event_priv*
event_qt_new(event_methods* meth)
{
    *meth = event_qt_methods;
    return nullptr;
}

static graphics_priv* graphics_qt_offscreen_new(navit* nav, struct graphics_methods* meth, attr** attrs, callback_list* cbl)
{
    struct graphics_priv* ret;
    struct font_priv* (*font_freetype_new)(void* meth);
    struct attr* attr;

    if (event_gr)
        return nullptr;
    if (!event_request_system("qt", "graphics_qt_qpainter_new"))
        return nullptr;
    font_freetype_new = (struct font_priv * (*)(void*))plugin_get_font_type("freetype");
    if (!font_freetype_new) {
        return nullptr;
    }
    ret = new graphics_priv;
    ret->cbl = cbl;
    *meth = graphics_methods;
    ret->nav = nav;
    ret->font_freetype_new = font_freetype_new;
    font_freetype_new(&ret->freetype_methods);
    meth->font_new = (struct graphics_font_priv * (*)(struct graphics_priv*, struct graphics_font_methods*, char*, int, int))ret->freetype_methods.font_new;
    meth->get_text_bbox = (void (*)(struct graphics_priv*, struct graphics_font_priv*, char*, int, int, struct point*, int))ret->freetype_methods.get_text_bbox;

    ret->app.reset(new QApplication(argc, argv));
    ret->opengl = backendOpenGL();
    ret->dumpFrame = dumpImage();
    qWarning() << "Have opengl=" <<  ret->opengl;
    setupQtPainter(ret);
    qDebug() << "Crated pixmap" << ret->buffer << "and painter" << ret->painter;
    ret->painter->fillRect(0, 0, ret->buffer->width(), ret->buffer->height(), QBrush());
    event_gr = ret;
    ret->w = ret->buffer->width();
    ret->h = ret->buffer->height();
    if ((attr = attr_search(attrs, nullptr, attr_w)))
        ret->w = attr->u.num;
    if ((attr = attr_search(attrs, nullptr, attr_h)))
        ret->h = attr->u.num;
    if ((attr = attr_search(attrs, nullptr, attr_window_title)))
        ret->window_title = std::string(attr->u.str);
    else
        ret->window_title = "Navit";

    return ret;
}

extern "C" {
void plugin_init()
{
    plugin_register_graphics_type("qt_offscreen", graphics_qt_offscreen_new);
    plugin_register_event_type("qt", event_qt_new);
}
}
