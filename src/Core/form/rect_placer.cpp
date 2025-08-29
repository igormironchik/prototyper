
/*
    SPDX-FileCopyrightText: 2025 Igor Mironchik <igor.mironchik@gmail.com>
    SPDX-License-Identifier: GPL-3.0-or-later
*/

// Prototyper include.
#include "rect_placer.hpp"

// Qt include.
#include <QPainter>
#include <QStyleOptionGraphicsItem>

namespace Prototyper
{

namespace Core
{

//
// FormRectPlacerPrivate
//

class FormRectPlacerPrivate
{
public:
    explicit FormRectPlacerPrivate(FormRectPlacer *parent)
        : q(parent)
        , m_rect(0.0,
                 0.0,
                 0.0,
                 0.0)
        , m_startPos(0.0,
                     0.0)
        , m_width(0.0)
        , m_height(0.0)
    {
    }

    //! Init.
    void init();

    //! Parent.
    FormRectPlacer *q;
    //! Rect.
    QRectF m_rect;
    //! Start pos.
    QPointF m_startPos;
    //! Width.
    qreal m_width;
    //! Height.
    qreal m_height;
}; // class FormRectPlacerPrivate

void FormRectPlacerPrivate::init()
{
}

//
// FormRectPlacer
//

FormRectPlacer::FormRectPlacer(QGraphicsItem *parent)
    : QGraphicsItem(parent)
    , d(nullptr)
{
    auto tmp = std::make_unique<FormRectPlacerPrivate>(this);

    tmp->init();

    d.swap(tmp);
}

FormRectPlacer::~FormRectPlacer() = default;

void FormRectPlacer::setStartPos(const QPointF &pos)
{
    d->m_startPos = pos;
    d->m_rect = QRectF(pos.x(), pos.y(), 0.0, 0.0);

    update();
}

const QRectF &FormRectPlacer::rect() const
{
    return d->m_rect;
}

void FormRectPlacer::setEndPos(const QPointF &pos)
{
    const QRectF tmp = d->m_rect.adjusted(-20.0, -20.0, 20.0, 20.0);

    d->m_width = pos.x() - d->m_startPos.x();
    d->m_height = pos.y() - d->m_startPos.y();

    d->m_rect = QRectF(d->m_startPos.x(), d->m_startPos.y(), d->m_width, d->m_height);

    update();

    parentItem()->update(tmp);
}

QRectF FormRectPlacer::boundingRect() const
{
    if (d) {
        return d->m_rect;
    }

    return {};
}

void FormRectPlacer::paint(QPainter *painter,
                           const QStyleOptionGraphicsItem *option,
                           QWidget *widget)
{
    Q_UNUSED(widget)

    painter->setPen(QPen(Qt::gray, 1.0, Qt::DashLine));
    painter->setBrush(Qt::NoBrush);

    painter->drawRect(option->rect);
}

} /* namespace Core */

} /* namespace Prototyper */
