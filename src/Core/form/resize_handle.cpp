
/*
    SPDX-FileCopyrightText: 2025 Igor Mironchik <igor.mironchik@gmail.com>
    SPDX-License-Identifier: GPL-3.0-or-later
*/

// Prototyper include.
#include "resize_handle.hpp"
#include "move_handle_private.hpp"

// Qt include.
#include <QPainter>
#include <QPainterPath>

namespace Prototyper
{

namespace Core
{

//
// FormResizeHandlePrivate
//

class FormResizeHandlePrivate : public FormMoveHandlePrivate
{
public:
    FormResizeHandlePrivate(qreal halfSize,
                            const QPointF &zero,
                            qreal angle,
                            FormWithHandle *object,
                            FormResizeHandle *parent,
                            Page *form,
                            const QCursor &c)
        : FormMoveHandlePrivate(halfSize,
                                zero,
                                object,
                                parent,
                                form,
                                c,
                                false)
        , m_angle(angle)
    {
    }

    //! Init.
    void init() Q_DECL_OVERRIDE;

    inline FormResizeHandle *q_func()
    {
        return dynamic_cast<FormResizeHandle *>(q);
    }
    inline const FormResizeHandle *q_func() const
    {
        return dynamic_cast<const FormResizeHandle *>(q);
    }

    //! Angle.
    qreal m_angle;
    //! Path.
    QPainterPath m_path;
}; // class FormResizeHandlePrivate

void FormResizeHandlePrivate::init()
{
    FormMoveHandlePrivate::init();

    const qreal h = m_size * 0.6;

    m_path.moveTo(0, -m_size);
    m_path.lineTo(m_size, -m_size + h);
    m_path.lineTo(m_size - h, -m_size + h);
    m_path.lineTo(m_size - h, m_size - h);
    m_path.lineTo(m_size, m_size - h);
    m_path.lineTo(0, m_size);
    m_path.lineTo(-m_size, m_size - h);
    m_path.lineTo(-m_size + h, m_size - h);
    m_path.lineTo(-m_size + h, -m_size + h);
    m_path.lineTo(-m_size, -m_size + h);
    m_path.lineTo(0, -m_size);
}

//
// FormResizeHandle
//

FormResizeHandle::FormResizeHandle(qreal halfSize,
                                   const QPointF &zero,
                                   qreal angle,
                                   FormWithHandle *object,
                                   QGraphicsItem *parent,
                                   Page *form,
                                   const QCursor &c)
    : FormMoveHandle(std::make_unique<FormResizeHandlePrivate>(halfSize,
                                                               zero,
                                                               angle,
                                                               object,
                                                               this,
                                                               form,
                                                               c),
                     parent)
{
}

FormResizeHandle::~FormResizeHandle() = default;

void FormResizeHandle::paint(QPainter *painter,
                             const QStyleOptionGraphicsItem *option,
                             QWidget *widget)
{
    Q_UNUSED(widget)
    Q_UNUSED(option)

    FormResizeHandlePrivate *dd = d_func();

    painter->translate(dd->m_size, dd->m_size);
    painter->rotate(dd->m_angle);

    if (dd->m_hovered) {
        painter->setBrush(Qt::red);
    } else {
        painter->setBrush(Qt::white);
    }

    painter->setPen(Qt::black);

    painter->drawPath(dd->m_path);
}

} /* namespace Core */

} /* namespace Prototyper */
