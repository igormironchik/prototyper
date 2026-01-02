
/*
    SPDX-FileCopyrightText: 2026 Igor Mironchik <igor.mironchik@gmail.com>
    SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef PROTOTYPER__CORE__FORM_WITH_RESIZE_AND_MOVE_HANDLES_HPP__INCLUDED
#define PROTOTYPER__CORE__FORM_WITH_RESIZE_AND_MOVE_HANDLES_HPP__INCLUDED

// Qt include.
#include <QSizeF>
#include <QtGlobal>

// C++ include.
#include <memory>

QT_BEGIN_NAMESPACE
class QGraphicsItem;
class QSizeF;
class QRectF;
QT_END_NAMESPACE

namespace Prototyper
{

namespace Core
{

class FormWithHandle;
class FormMoveHandle;
class FormResizeHandle;
class Page;

//
// WithResizeAndMoveHandles
//

//! Storage of resize and move handles.
class WithResizeAndMoveHandles final
{
public:
    WithResizeAndMoveHandles(FormWithHandle *object,
                             QGraphicsItem *parent,
                             Page *form);
    ~WithResizeAndMoveHandles();

    //! Place handles.
    void place(const QRectF &r);
    //! Show handles.
    void show();
    //! Hide handles.
    void hide();
    //! Check constraint.
    bool checkConstraint(const QSizeF &s);
    //! Set min size.
    void setMinSize(const QSizeF &min);

    //! Top-left resize.
    std::unique_ptr<FormResizeHandle> m_topLeft;
    //! Top resize.
    std::unique_ptr<FormResizeHandle> m_top;
    //! Top-right resize.
    std::unique_ptr<FormResizeHandle> m_topRight;
    //! Left resize.
    std::unique_ptr<FormResizeHandle> m_left;
    //! Left-bottom resize.
    std::unique_ptr<FormResizeHandle> m_bottomLeft;
    //! Bottom resize.
    std::unique_ptr<FormResizeHandle> m_bottom;
    //! Right-bottom resize.
    std::unique_ptr<FormResizeHandle> m_bottomRight;
    //! Right resize.
    std::unique_ptr<FormResizeHandle> m_right;
    //! Move handle.
    std::unique_ptr<FormMoveHandle> m_move;
    //! Parent.
    QGraphicsItem *q;
    //! Min size.
    QSizeF m_min;
}; // class WithResizeAndMoveHandles

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__FORM_WITH_RESIZE_AND_MOVE_HANDLES_HPP__INCLUDED
