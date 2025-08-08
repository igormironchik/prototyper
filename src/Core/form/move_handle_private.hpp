
/*
    SPDX-FileCopyrightText: 2016-2024 Igor Mironchik <igor.mironchik@gmail.com>
    SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef PROTOTYPER__CORE__FORM_MOVE_HANDLER_PRIVATE_HPP__INCLUDED
#define PROTOTYPER__CORE__FORM_MOVE_HANDLER_PRIVATE_HPP__INCLUDED

// Qt include.
#include <QCursor>
#include <QGraphicsObject>

namespace Prototyper
{

namespace Core
{

class FormMoveHandle;
class Page;
class FormWithHandle;

//
// FormMoveHandlePrivate
//

class FormMoveHandlePrivate
{
public:
    FormMoveHandlePrivate(qreal halfSize,
                          const QPointF &zero,
                          FormWithHandle *object,
                          FormMoveHandle *parent,
                          Page *form,
                          const QCursor &c,
                          bool followCursor);
    virtual ~FormMoveHandlePrivate();

    //! Init.
    virtual void init();

    //! Parent.
    FormMoveHandle *q;
    //! Object.
    FormWithHandle *m_object;
    //! Half of the size.
    qreal m_size;
    //! Hovered.
    bool m_hovered;
    //! Pressed.
    bool m_pressed;
    //! Mouse point.
    QPointF m_pos;
    //! Ignore mouse events?
    bool m_ignoreMouse;
    //! Follow cursor?
    bool m_followCursor;
    //! Zero point.
    QPointF m_zero;
    //! Touch delta.
    QPointF m_touchDelta;
    //! Cursor.
    QCursor m_cursor;
    //! Form.
    Page *m_form;
}; // class FormMoveHandlerPrivate

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__FORM_MOVE_HANDLER_PRIVATE_HPP__INCLUDED
