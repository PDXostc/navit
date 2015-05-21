/**
 * Navit, a modular navigation system.
 * Copyright (C) 2005-2008 Navit Team
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 */
#include "graphics_qt_qpainter.h"
#include "RenderArea.h"

#include "RenderArea.moc"

#include <QtWidgets/QGestureEvent>

bool touchStarted = false;
bool zoomIn = false;
bool gestureInProgress = false;
int numberOfTaps =0;
bool mouseTimerRunning = false;
QPoint lastTouchEvent;
QPoint touchPosition;
QPoint firstTouchPosition;
bool sentSignal = false;
bool isTapAndHold =false;
#ifdef QT_QPAINTER_USE_EMBEDDING
EmbeddedWidget::EmbeddedWidget(struct graphics_priv *priv, QWidget* child, QWidget *parent) 
: QX11EmbedWidget(parent) {
    this->gra=priv;
		this->setWindowTitle(priv->window_title);
        QStackedLayout* _outerLayout = new QStackedLayout(this);
        this->setLayout(_outerLayout);
        _outerLayout->addWidget(child);
        _outerLayout->setCurrentWidget(child);
}

void EmbeddedWidget::closeEvent(QCloseEvent* event) 
{
	gra->widget->processClose();
}
#endif /* QT_QPAINTER_USE_EMBEDDING */

//##############################################################################################################
//# Description: Constructor
//# Comment: Using a QPixmap for rendering the graphics
//# Authors: Martin Schaller (04/2008)
//##############################################################################################################
RenderArea::RenderArea(struct graphics_priv *priv, QT_QPAINTER_RENDERAREA_PARENT *parent, int w, int h, int overlay)
	: QT_QPAINTER_RENDERAREA_PARENT(parent)
{
	pixmap = new QPixmap(w, h);
#ifndef QT_QPAINTER_NO_WIDGET
	if (!overlay) {
#if QT_VERSION >= 0x040700                                                 
        qDebug() << "Registering gestures";
		grabGesture(Qt::PinchGesture);
		grabGesture(Qt::SwipeGesture);
		grabGesture(Qt::PanGesture);
        grabGesture(Qt::TapAndHoldGesture);
        grabGesture(Qt::TapGesture);
#endif
		setWindowTitle(priv->window_title);
	}
#endif
	is_overlay=overlay;
	gra=priv;
#ifdef QT_QPAINTER_USE_EVENT_QT
	timer_type=g_hash_table_new(NULL, NULL);
	timer_callback=g_hash_table_new(NULL, NULL);
	watches=g_hash_table_new(NULL, NULL);
#ifndef QT_QPAINTER_NO_WIDGET
	setAttribute(Qt::WA_OpaquePaintEvent, true);
    setAttribute(Qt::WA_AcceptTouchEvents, true);
#endif
#endif
}

//##############################################################################################################
//# Description: QWidget:closeEvent
//# Comment: Deletes navit object and stops event loop on graphics shutdown
//##############################################################################################################
void RenderArea::processClose() 
{
	callback_list_call_attr_0(this->cbl, attr_window_closed);
}
void RenderArea::closeEvent(QCloseEvent* event) 
{
	this->processClose();
}

bool RenderArea::event(QEvent *event)
{
	if (event->type() == QEvent::Gesture) {
        QGestureEvent* gest = static_cast<QGestureEvent*>(event);
        if( QGesture* pinch = gest->gesture(Qt::PinchGesture) ) {
            // pinch gesture
            QPinchGesture *pinchEve = static_cast<QPinchGesture *>(pinch);
            qDebug() << "Pinch " << pinchEve  << pinchEve->state() << pinchEve->scaleFactor() << pinchEve->lastScaleFactor();
            struct point p;
            int button = 4;
            if(pinchEve->state() == Qt::GestureStarted) {
                gestureInProgress = true;
            }
            if (pinchEve->state() == Qt::GestureFinished ) {
                gestureInProgress = false;
                if (pinchEve->totalScaleFactor() > 1 ) {
                    button = 4;
                } else {
                    button = 5;
                }

                p.x = pinchEve->lastCenterPoint().x();
                p.y = pinchEve->lastCenterPoint().y();

                callback_list_call_attr_3(this->cbl, attr_button, GINT_TO_POINTER(1), GINT_TO_POINTER(button), GINT_TO_POINTER(&p));
                callback_list_call_attr_3(this->cbl, attr_button, GINT_TO_POINTER(0), GINT_TO_POINTER(button), GINT_TO_POINTER(&p));
            }
        } else if(QGesture* tap = gest->gesture(Qt::TapGesture) ) {
            if (tap->state() == Qt::GestureFinished ) {
                QTapGesture* gest = static_cast<QTapGesture*>(tap);
                qDebug() << "tap tap" << tap->state();
                lastTouchEvent = gest->position().toPoint();
                numberOfTaps++;
                if(!mouseTimerRunning) {
                    mouseTimerRunning = true;
                    QTimer::singleShot(500,this, SLOT(mouseTimer()));
                }
            }
        } else if (QGesture* tapAndHold = gest->gesture(Qt::TapAndHoldGesture) ) {
            if (tapAndHold->state() == Qt::GestureFinished) {
                qDebug() << "Touch and hold";
                QTapAndHoldGesture* gest = static_cast<QTapAndHoldGesture*>(tapAndHold);
                if (gest) {
                    QPoint pressPoint = gest->position().toPoint();
                    point p;
                    p.x=pressPoint.x();
                    p.y=pressPoint.y();
                    qDebug() << "Sending t&h signal";
                    callback_list_call_attr_1(this->cbl, attr_signal_on_map_click,  GINT_TO_POINTER(&p));
                    isTapAndHold = true;
                }
            }
        }

        event->accept();
        return true;
    } else if(event->type() == QEvent::TouchBegin) {
        if (!gestureInProgress) {
            qDebug() << "Touch begin";
            QTouchEvent* te = static_cast<QTouchEvent*>(event);
            QTouchEvent::TouchPoint p = te->touchPoints().at(0);
            touchPosition = p.pos().toPoint();
            firstTouchPosition = touchPosition;
            qDebug() << "Touch point " << touchPosition;
            mouseEvent(1, p.pos().toPoint());
        }
        event->accept();
    } else if (event->type() == QEvent::TouchUpdate) {
        if (!gestureInProgress) {
            QTouchEvent* te = static_cast<QTouchEvent*>(event);
            QTouchEvent::TouchPoint point = te->touchPoints().at(0);
            struct point p;
            p.x=point.pos().x();
            p.y=point.pos().y();
            touchPosition = point.pos().toPoint();
            qDebug() << "Touch point " << touchPosition;
            callback_list_call_attr_1(this->cbl, attr_motion, (void *)&p);
        }
        event->accept();
    } else if (event->type() == QEvent::TouchEnd) {
        if (!gestureInProgress) {
            QTouchEvent* te = static_cast<QTouchEvent*>(event);
            QTouchEvent::TouchPoint p = te->touchPoints().at(0);
            mouseEvent(0, p.pos().toPoint());
            qDebug() << "First = " << firstTouchPosition << " last=" << touchPosition;
            QRect firstRect (firstTouchPosition.x() - 2, firstTouchPosition.y() - 2, 4, 4);
            QRect lastRect ( touchPosition.x() - 2, touchPosition.y() - 2, 4, 4);
            qDebug() << "frect = " << firstRect << "lrect=" << lastRect;
            if (firstRect.intersects(lastRect)) {
                // this is a tap
                if (!isTapAndHold) {
                    qDebug() << "Single tap detected!";
                    callback_list_call_attr_1(this->cbl, attr_signal_on_map_click_tap,  GINT_TO_POINTER(&p));
                } else {
                    qDebug() << "Single tap, but t&h already sent";
                }
            } else {
                if (!isTapAndHold) {
                    // this was a move opeartion
                    qDebug() << "Move gesture, sent click_tap";
                    callback_list_call_attr_1(this->cbl, attr_signal_on_map_click_tap,  GINT_TO_POINTER(&p));
                }
            }
            touchPosition = p.pos().toPoint();
        }
        isTapAndHold = false;
        event->accept();
    } else {
        return QWidget::event(event);
    }
}
//##############################################################################################################
//# Description: QWidget:sizeHint
//# Comment: This property holds the recommended size for the widget
//# Authors: Martin Schaller (04/2008)
//##############################################################################################################
QSize RenderArea::sizeHint() const
{
	return QSize(gra->w, gra->h);
}

//##############################################################################################################
//# Description: QWidget:paintEvent
//# Comment: A paint event is a request to repaint all or part of the widget.
//# Authors: Martin Schaller (04/2008)
//##############################################################################################################
void RenderArea::paintEvent(QPaintEvent * event)
{
	qt_qpainter_draw(gra, &event->rect(), 1);
}

void RenderArea::do_resize(QSize size)
{
    if (pixmap->paintingActive()) {
        pixmap->paintEngine()->painter()->end();
    }
	delete pixmap;
	pixmap=new QPixmap(size);
	pixmap->fill();
    QPainter painter(pixmap);
    QBrush brush;
	painter.fillRect(0, 0, size.width(), size.height(), brush);
	dbg(lvl_debug,"size %dx%d\n", size.width(), size.height());
	dbg(lvl_debug,"pixmap %p %dx%d\n", pixmap, pixmap->width(), pixmap->height());
	callback_list_call_attr_2(this->cbl, attr_resize, GINT_TO_POINTER(size.width()), GINT_TO_POINTER(size.height()));
}

//##############################################################################################################
//# Description: QWidget::resizeEvent()
//# Comment: When resizeEvent() is called, the widget already has its new geometry.
//# Authors: Martin Schaller (04/2008)
//##############################################################################################################
void RenderArea::resizeEvent(QResizeEvent * event)
{
	if (!this->is_overlay) {
		RenderArea::do_resize(event->size());
	}
}

//##############################################################################################################
//# Description: Method to handle mouse clicks
//# Comment: Delegate of QWidget::mousePressEvent and QWidget::mouseReleaseEvent (see below)
//# Authors: Martin Schaller (04/2008)
//##############################################################################################################
void RenderArea::mouseEvent(int pressed, QPoint point)
{
    qDebug() << Q_FUNC_INFO;
    struct point p;
    p.x=point.x();
    p.y=point.y();
    callback_list_call_attr_3(this->cbl, attr_button, GINT_TO_POINTER(pressed), GINT_TO_POINTER(1), GINT_TO_POINTER(&p));
}

void RenderArea::mousePressEvent(QMouseEvent *event)
{
    struct point p;

    qDebug() << Q_FUNC_INFO;
    mouseEvent(1, event->pos());
}

void RenderArea::mouseReleaseEvent(QMouseEvent *event)
{
    qDebug() << Q_FUNC_INFO;
    mouseEvent(0, event->pos());
    struct point p;

    p.x = event->pos().x();
    p.y = event->pos().y();
    if (!sentSignal) {
        qDebug() << "Sending click_tap singal";
        callback_list_call_attr_1(this->cbl, attr_signal_on_map_click_tap,  GINT_TO_POINTER(&p));
    }

    sentSignal = false;
}

//##############################################################################################################
//# Description: QWidget::mouseMoveEvent
//# Comment: If mouse tracking is switched on, mouse move events occur even if no mouse button is pressed.
//# Authors: Martin Schaller (04/2008)
//##############################################################################################################
void RenderArea::mouseMoveEvent(QMouseEvent *event)
{
    qDebug() << Q_FUNC_INFO;
    struct point p;
    p.x=event->x();
    p.y=event->y();
    callback_list_call_attr_1(this->cbl, attr_motion, (void *)&p);
}

//##############################################################################################################
//# Description: Qt Event :: Zoom in/out with the mouse's scrollwheel
//# Comment:
//# Authors: Stefan Klumpp (04/2008)
//##############################################################################################################
void RenderArea::wheelEvent(QWheelEvent *event)
{
    qDebug() <</* */Q_FUNC_INFO;
    struct point p;
    int button;
	
    p.x=event->x();	// xy-coordinates of the mouse pointer
    p.y=event->y();
	
    if (event->delta() > 0)	// wheel movement away from the person
        button=4;
    else if (event->delta() < 0) // wheel movement towards the person
        button=5;
    else
        button=-1;
	
    if (button != -1) {
        callback_list_call_attr_3(this->cbl, attr_button, GINT_TO_POINTER(1), GINT_TO_POINTER(button), GINT_TO_POINTER(&p));
        callback_list_call_attr_3(this->cbl, attr_button, GINT_TO_POINTER(0), GINT_TO_POINTER(button), GINT_TO_POINTER(&p));
    }
	
    event->accept();
}

#define CASE(x) case x:

void RenderArea::keyPressEvent(QKeyEvent *event)
{
    qDebug() << Q_FUNC_INFO;
    QString str=event->text();
    const char *text=str.toUtf8().constData();
    dbg(lvl_debug,"enter text='%s' 0x%x (%zu) key=%d\n", text, text[0], strlen(text), event->key());
    if (!text || !text[0] || text[0] == 0x7f) {
        dbg(lvl_debug,"special key\n");
        switch (event->key()) {
        case 4099:
            {
                char text_backspace[] = {NAVIT_KEY_BACKSPACE,'\0'};
                text=text_backspace;
            }
            break;
        case 4101:
#ifdef QT_QPAINTER_CUSTOM_RETURN
        QT_QPAINTER_CUSTOM_RETURN
#endif
            {
                char text_return[] = {NAVIT_KEY_RETURN,'\0'};
                text=text_return;
            }
            break;
        case 4114:
#ifdef QT_QPAINTER_CUSTOM_LEFT
        QT_QPAINTER_CUSTOM_LEFT
#endif
            {
                char text_left[] = {NAVIT_KEY_LEFT,'\0'};
                text=text_left;
            }
            break;
        case 4115:
#ifdef QT_QPAINTER_CUSTOM_UP
        QT_QPAINTER_CUSTOM_UP
#endif
            {
                char text_up[] = {NAVIT_KEY_UP,'\0'};
                text=text_up;
            }
            break;
        case 4116:
#ifdef QT_QPAINTER_CUSTOM_RIGHT
        QT_QPAINTER_CUSTOM_RIGHT
#endif
            {
                char text_right[] = {NAVIT_KEY_RIGHT,'\0'};
                text=text_right;
            }
            break;
        case 4117:
#ifdef QT_QPAINTER_CUSTOM_DOWN
        QT_QPAINTER_CUSTOM_DOWN
#endif
            {
                char text_down[] = {NAVIT_KEY_DOWN,'\0'};
                text=text_down;
            }
            break;
        }
    }
    callback_list_call_attr_1(this->cbl, attr_keypress, (void *)text);
    event->accept();
}

void RenderArea::watchEvent(int fd)
{
#ifdef QT_QPAINTER_USE_EVENT_QT
	struct event_watch *ev=(struct event_watch *)g_hash_table_lookup(watches, (void *)fd);
	dbg(lvl_debug,"fd=%d ev=%p cb=%p\n", fd, ev, ev->cb);
	callback_call_0(ev->cb);
#endif
}

void RenderArea::mouseTimer()
{
    mouseTimerRunning = false;
    qDebug() << Q_FUNC_INFO<< numberOfTaps << lastTouchEvent;
    int button {0};
    struct point p;

    p.x = lastTouchEvent.x();
    p.y = lastTouchEvent.y();

    if (numberOfTaps > 3 || numberOfTaps == 1) {
        // don't do anything
        numberOfTaps = 0;
        return;
    }

    if(numberOfTaps == 2) {
        qDebug() <<  "zoom in";
        button = 4;
    } else if (numberOfTaps == 3) {
        qDebug() <<  "zoom out";
        button = 5;
    }

    callback_list_call_attr_3(this->cbl, attr_button, GINT_TO_POINTER(1), GINT_TO_POINTER(button), GINT_TO_POINTER(&p));
    numberOfTaps = 0;
}

#ifdef QT_QPAINTER_USE_EVENT_QT
void RenderArea::timerEvent(QTimerEvent *event)
{
	int id=event->timerId();
	struct callback *cb=(struct callback *)g_hash_table_lookup(timer_callback, (void *)id);
	if (cb) 
		callback_call_0(cb);
	if (!g_hash_table_lookup(timer_type, (void *)id))
		event_qt_remove_timeout((struct event_timeout *)id);
}
#endif /* QT_QPAINTER_USE_EVENT_QT */

