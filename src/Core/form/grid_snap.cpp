
/*
    SPDX-FileCopyrightText: 2016-2024 Igor Mironchik <igor.mironchik@gmail.com>
    SPDX-License-Identifier: GPL-3.0-or-later
*/

// Prototyper include.
#include "grid_snap.hpp"
#include "../constants.hpp"

// Qt include.
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOptionGraphicsItem>

namespace Prototyper
{

namespace Core
{

//
// GridSnapPrivate
//

class GridSnapPrivate
{
public:
    explicit GridSnapPrivate(GridSnap *parent)
        : q(parent)
        , m_step(10.0)
        , m_size(c_snapHalfSize)
    {
    }

    //! Init.
    void init();
    //! Calc snap pos.
    void calc();
    //! Calc snap pos.
    inline QPointF calc(const QPointF &p);

    //! Parent.
    GridSnap *q;
    //! Grid step.
    int m_step;
    //! Pos.
    QPointF m_pos;
    //! Size.
    const int m_size;
}; // class GridSnapPrivate

void GridSnapPrivate::init()
{
    q->setZValue(c_mostTopZValue);
}

QPointF GridSnapPrivate::calc(const QPointF &p)
{
    int y = ((int)p.y()) / m_step * m_step + qRound((qreal)(((int)p.y()) % m_step) / (qreal)m_step) * m_step;

    return QPointF((int)p.x() / m_step * m_step + qRound((qreal)((int)p.x() % m_step) / (qreal)m_step) * m_step, y);
}

void GridSnapPrivate::calc()
{
    m_pos = calc(m_pos);

    q->setPos(m_pos - QPointF(m_size, m_size));
}

//
// GridSnap
//

GridSnap::GridSnap(QGraphicsItem *parent)
    : QGraphicsItem(parent)
    , d(nullptr)
{
    auto tmp = std::make_unique<GridSnapPrivate>(this);

    tmp->init();

    d.swap(tmp);
}

GridSnap::~GridSnap() = default;

const QPointF &GridSnap::snapPos() const
{
    return d->m_pos;
}

void GridSnap::setSnapPos(const QPointF &p)
{
    d->m_pos = p;

    d->calc();
}

void GridSnap::setGridStep(int step)
{
    d->m_step = step;

    d->calc();
}

QPointF GridSnap::snapped(const QPointF &p) const
{
    return d->calc(p);
}

QRectF GridSnap::boundingRect() const
{
    if (d) {
        return {0, 0, d->m_size * c_halfDivider, d->m_size * c_halfDivider};
    }

    return {};
}

void GridSnap::paint(QPainter *painter,
                     const QStyleOptionGraphicsItem *option,
                     QWidget *widget)
{
    Q_UNUSED(widget)

    QPen pen(Qt::gray, 1.0);
    pen.setCosmetic(true);

    painter->setPen(pen);

    painter->drawLine(option->rect.x(),
                      option->rect.y(),
                      option->rect.x() + option->rect.width(),
                      option->rect.y() + option->rect.height());
    painter->drawLine(option->rect.x() + option->rect.width(),
                      option->rect.y(),
                      option->rect.x(),
                      option->rect.y() + option->rect.height());
}

void GridSnap::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    event->ignore();
}

void GridSnap::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    event->ignore();
}

void GridSnap::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    event->ignore();
}

} /* namespace Core */

} /* namespace Prototyper */
