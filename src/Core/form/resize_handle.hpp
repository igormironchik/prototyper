
/*
    SPDX-FileCopyrightText: 2026 Igor Mironchik <igor.mironchik@gmail.com>
    SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef PROTOTYPER__CORE__FORM_RESIZE_HANDLE_HPP__INCLUDED
#define PROTOTYPER__CORE__FORM_RESIZE_HANDLE_HPP__INCLUDED

// Prototyper include.
#include "move_handle.hpp"

namespace Prototyper
{

namespace Core
{

//
// FormResizeHandle
//

class FormResizeHandlePrivate;

//! rESIZE handler on the form.
class FormResizeHandle : public FormMoveHandle
{
public:
    FormResizeHandle(qreal halfSize,
                     const QPointF &zero,
                     qreal angle,
                     FormWithHandle *object,
                     QGraphicsItem *parent,
                     Page *form,
                     const QCursor &c = Qt::SizeAllCursor);
    ~FormResizeHandle() override;

    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget = 0) override;

private:
    Q_DISABLE_COPY(FormResizeHandle)

    inline FormResizeHandlePrivate *d_func()
    {
        return reinterpret_cast<FormResizeHandlePrivate *>(d.get());
    }
    inline const FormResizeHandlePrivate *d_func() const
    {
        return reinterpret_cast<const FormResizeHandlePrivate *>(d.get());
    }
}; // class FormResizeHandler

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__FORM_RESIZE_HANDLE_HPP__INCLUDED
