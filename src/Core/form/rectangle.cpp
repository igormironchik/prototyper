
/*
    SPDX-FileCopyrightText: 2016-2024 Igor Mironchik <igor.mironchik@gmail.com>
    SPDX-License-Identifier: GPL-3.0-or-later
*/

// Prototyper include.
#include "rectangle.hpp"
#include "../constants.hpp"
#include "actions.hpp"
#include "page.hpp"
#include "resize_handle.hpp"
#include "undo_commands.hpp"
#include "utils.hpp"
#include "with_resize_and_move_handles.hpp"

// Qt include.
#include <QGraphicsScene>
#include <QPainter>
#include <QUndoStack>

namespace Prototyper
{

namespace Core
{

//
// FormRectPrivate
//

class FormRectPrivate
{
public:
    explicit FormRectPrivate(FormRect *parent)
        : q(parent)
        , m_handles(nullptr)
        , m_isHandleMoved(false)
    {
    }

    //! Init.
    void init();
    //! Update rect.
    void updateRect(const QRectF &r);

    //! Parent.
    FormRect *q;
    //! Resize & move handles.
    std::unique_ptr<WithResizeAndMoveHandles> m_handles;
    //! Sunsidiary rect.
    QRectF m_subsidiaryRect;
    //! Is handle moved?
    bool m_isHandleMoved;
    //! Rect.
    QRectF m_rect;
}; // class FormRectPrivate

void FormRectPrivate::init()
{
    m_handles.reset(new WithResizeAndMoveHandles(q, q->parentItem(), q->page()));

    m_handles->hide();

    q->setObjectPen(QPen(PageAction::instance()->strokeColor(), c_linePenWidth), false);

    q->setObjectBrush(QBrush(PageAction::instance()->fillColor()), false);
}

void FormRectPrivate::updateRect(const QRectF &r)
{
    m_rect = r;

    m_rect.moveTopLeft(QPointF(0.0, 0.0));

    QRectF hr = m_rect;

    hr.moveTopLeft(q->pos() + hr.topLeft());
    hr.adjust(-c_halfHandleSize * c_halfDivider * c_halfDivider,
              -c_halfHandleSize * c_halfDivider * c_halfDivider,
              c_halfHandleSize * c_halfDivider * c_halfDivider,
              c_halfHandleSize * c_halfDivider * c_halfDivider);

    m_handles->place(hr);

    q->update();
}

//
// FormRect
//

FormRect::FormRect(Page *page,
                   QGraphicsItem *parent)
    : QGraphicsItem(parent)
    , FormObject(FormObject::RectType,
                 page)
    , d(new FormRectPrivate(this))
{
    d->init();
}

FormRect::~FormRect() = default;

Cfg::Rect FormRect::cfg() const
{
    Cfg::Rect c;

    c.set_objectId(objectId());

    Cfg::Point p;
    p.set_x(MmPx::instance().toMmX(pos().x()));
    p.set_y(MmPx::instance().toMmY(pos().y()));

    c.set_pos(p);

    c.set_pen(Cfg::pen(objectPen()));

    c.set_brush(Cfg::brush(objectBrush()));

    Cfg::Point topLeft;
    topLeft.set_x(MmPx::instance().toMmX(pos().x()));
    topLeft.set_y(MmPx::instance().toMmY(pos().y()));

    c.set_topLeft(topLeft);

    Cfg::Size s;
    s.set_width(MmPx::instance().toMmX(d->m_rect.size().width()));
    s.set_height(MmPx::instance().toMmY(d->m_rect.size().height()));

    c.set_size(s);

    c.set_z(zValue());

    return c;
}

void FormRect::setCfg(const Cfg::Rect &c)
{
    setObjectId(c.objectId());

    setObjectPen(Cfg::fromPen(c.pen()), false);

    setObjectBrush(Cfg::fromBrush(c.brush()), false);

    const QRectF r(qRound(MmPx::instance().fromMmX(c.topLeft().x())),
                   qRound(MmPx::instance().fromMmY(c.topLeft().y())),
                   qRound(MmPx::instance().fromMmX(c.size().width())),
                   qRound(MmPx::instance().fromMmY(c.size().height())));

    setPos(QPointF(MmPx::instance().fromMmX(c.pos().x()), MmPx::instance().fromMmY(c.pos().y())));

    setZValue(c.z());

    d->updateRect(r);
}

QRectF FormRect::boundingRect() const
{
    if (d) {
        const qreal w = objectPen().widthF() / 2.0;

        return d->m_rect.adjusted(-w, -w, w, w);
    }

    return {};
}

void FormRect::paint(QPainter *painter,
                     const QStyleOptionGraphicsItem *option,
                     QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    painter->setPen(objectPen());
    painter->setBrush(objectBrush());

    painter->drawRect(d->m_rect);

    if (isSelected() && !group()) {
        d->m_handles->show();
    } else {
        d->m_handles->hide();
    }
}

void FormRect::setPosition(const QPointF &p,
                           bool pushUndoCommand)
{
    FormObject::setPosition(p, pushUndoCommand);

    setPos(p);

    d->updateRect(d->m_rect);
}

QPointF FormRect::position() const
{
    return pos();
}

void FormRect::setRectangle(const QRectF &r,
                            bool pushUndoCommand)
{
    FormObject::setRectangle(r, pushUndoCommand);

    setPos(r.topLeft());

    d->updateRect(r);

    scene()->update();
}

QRectF FormRect::rectangle() const
{
    QRectF r = d->m_rect;
    r.moveTopLeft(pos());

    return r;
}

void FormRect::setObjectPen(const QPen &p,
                            bool pushUndoCommand)
{
    QPen newPen = p;
    newPen.setWidth(2);

    FormObject::setObjectPen(newPen, pushUndoCommand);

    update();
}

void FormRect::setObjectBrush(const QBrush &b,
                              bool pushUndoCommand)
{
    FormObject::setObjectBrush(b, pushUndoCommand);

    update();
}

void FormRect::handleMoved(const QPointF &delta,
                           FormMoveHandle *handle)
{
    if (!d->m_isHandleMoved) {
        d->m_subsidiaryRect = rectangle();

        d->m_isHandleMoved = true;
    }

    if (handle == d->m_handles->m_move.get()) {
        moveBy(delta.x(), delta.y());

        d->updateRect(d->m_rect);
    } else if (handle == d->m_handles->m_topLeft.get()) {
        const QRectF r = d->m_rect.adjusted(delta.x(), delta.y(), 0.0, 0.0);

        if (d->m_handles->checkConstraint(r.size())) {
            d->updateRect(r);
        }
    } else if (handle == d->m_handles->m_top.get()) {
        const QRectF r = d->m_rect.adjusted(0.0, delta.y(), 0.0, 0.0);

        if (d->m_handles->checkConstraint(r.size())) {
            d->updateRect(r);
        }
    } else if (handle == d->m_handles->m_topRight.get()) {
        const QRectF r = d->m_rect.adjusted(0.0, delta.y(), delta.x(), 0.0);

        if (d->m_handles->checkConstraint(r.size())) {
            d->updateRect(r);
        }
    } else if (handle == d->m_handles->m_right.get()) {
        const QRectF r = d->m_rect.adjusted(0.0, 0.0, delta.x(), 0.0);

        if (d->m_handles->checkConstraint(r.size())) {
            d->updateRect(r);
        }
    } else if (handle == d->m_handles->m_bottomRight.get()) {
        const QRectF r = d->m_rect.adjusted(0.0, 0.0, delta.x(), delta.y());

        if (d->m_handles->checkConstraint(r.size())) {
            d->updateRect(r);
        }
    } else if (handle == d->m_handles->m_bottom.get()) {
        const QRectF r = d->m_rect.adjusted(0.0, 0.0, 0.0, delta.y());

        if (d->m_handles->checkConstraint(r.size())) {
            d->updateRect(r);
        }
    } else if (handle == d->m_handles->m_bottomLeft.get()) {
        const QRectF r = d->m_rect.adjusted(delta.x(), 0.0, 0.0, delta.y());

        if (d->m_handles->checkConstraint(r.size())) {
            d->updateRect(r);
        }
    } else if (handle == d->m_handles->m_left.get()) {
        const QRectF r = d->m_rect.adjusted(delta.x(), 0.0, 0.0, 0.0);

        if (d->m_handles->checkConstraint(r.size())) {
            d->updateRect(r);
        }
    }

    page()->update();
}

void FormRect::handleReleased(FormMoveHandle *handle)
{
    d->m_isHandleMoved = false;

    if (handle == d->m_handles->m_move.get()) {
        page()->undoStack()->push(new UndoMove(page(), objectId(), position() - d->m_subsidiaryRect.topLeft()));
    } else {
        QRectF r = rectangle();

        page()->undoStack()->push(new UndoResize(page(), objectId(), d->m_subsidiaryRect, r));
    }

    updatePropertiesValues();

    page()->emitChanged();
}

FormObject *FormRect::clone() const
{
    auto *o = new FormRect(page(), parentItem());

    o->setCfg(cfg());

    o->setObjectId(page()->nextId());

    return o;
}

} /* namespace Core */

} /* namespace Prototyper */
