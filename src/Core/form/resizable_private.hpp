
/*
    SPDX-FileCopyrightText: 2025 Igor Mironchik <igor.mironchik@gmail.com>
    SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef PROTOTYPER__CORE__FORM_RESIZABLE_PRIVATE_HPP__INCLUDED
#define PROTOTYPER__CORE__FORM_RESIZABLE_PRIVATE_HPP__INCLUDED

// C++ include.
#include <memory>

// Prototyper include.
#include "move_handle.hpp"
#include "with_resize_and_move_handles.hpp"

namespace Prototyper
{

namespace Core
{

class FormResizable;

//
// FormResizableProxyPrivate
//

class FormResizableProxy;

class FormResizableProxyPrivate
{
public:
    FormResizableProxyPrivate(FormResizable *resizable,
                              FormResizableProxy *parent,
                              Page *form);

    virtual ~FormResizableProxyPrivate();

    //! Init.
    virtual void init();
    //! Place handles.
    virtual void place(const QRectF &rect);

    //! Parent.
    FormResizableProxy *q;
    //! Object.
    FormResizable *m_object;
    //! Rect.
    QRectF m_rect;
    //! Handles.
    std::unique_ptr<WithResizeAndMoveHandles> m_handles;
    //! Form.
    Page *m_form;
    //! Subsidiary rect.
    QRectF m_subsidiaryRect;
    //! Is hadnle moved.
    bool m_handleMoved;
}; // class FormResizableProxyPrivate;

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__FORM_RESIZABLE_PRIVATE_HPP__INCLUDED
