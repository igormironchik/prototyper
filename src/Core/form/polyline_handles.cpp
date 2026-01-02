
/*
    SPDX-FileCopyrightText: 2026 Igor Mironchik <igor.mironchik@gmail.com>
    SPDX-License-Identifier: GPL-3.0-or-later
*/

// Prototyper include.
#include "polyline_handles.hpp"
#include "../constants.hpp"
#include "resizable_private.hpp"

namespace Prototyper
{

namespace Core
{

//
// FormPolylineHandlesPrivate
//

class FormPolylineHandlesPrivate : public FormResizableProxyPrivate
{
public:
    FormPolylineHandlesPrivate(FormResizable *resizable,
                               FormPolylineHandles *parent,
                               Page *form)
        : FormResizableProxyPrivate(resizable,
                                    parent,
                                    form)
    {
    }

    ~FormPolylineHandlesPrivate() override = default;

    //! Init.
    void init() Q_DECL_OVERRIDE;
    //! Place handles.
    void place(const QRectF &rect) Q_DECL_OVERRIDE;

    inline const FormPolylineHandles *q_ptr() const
    {
        return static_cast<const FormPolylineHandles *>(q);
    }
    inline FormPolylineHandles *q_ptr()
    {
        return static_cast<FormPolylineHandles *>(q);
    }

    //! Aspect ratio handle.
    std::unique_ptr<NodesEditResizeHandle> m_modeHandle;
}; // class FormPolylineHandlesPrivate

void FormPolylineHandlesPrivate::init()
{
    FormResizableProxyPrivate::init();

    m_modeHandle.reset(new NodesEditResizeHandle(q));

    FormPolylineHandles::connect(m_modeHandle.get(),
                                 &NodesEditResizeHandle::currentModeChanged,
                                 q_ptr(),
                                 &FormPolylineHandles::currentModeChanged);
}

void FormPolylineHandlesPrivate::place(const QRectF &rect)
{
    FormResizableProxyPrivate::place(rect);

    m_modeHandle->setPos(rect.x() - c_halfHandleSize * c_halfDivider * c_halfDivider,
                         rect.y() - c_halfHandleSize * c_halfDivider * c_halfDivider);

    q->parentItem()->update(rect.adjusted(-c_halfHandleSize * c_halfDivider * c_halfDivider * c_halfDivider,
                                          -c_halfHandleSize * c_halfDivider * c_halfDivider * c_halfDivider,
                                          c_halfHandleSize * c_halfDivider * c_halfDivider * c_halfDivider,
                                          c_halfHandleSize * c_halfDivider * c_halfDivider * c_halfDivider));
}

//
// FormPolylineHandles
//

FormPolylineHandles::FormPolylineHandles(FormResizable *resizable,
                                         QGraphicsItem *parent,
                                         Page *form)
    : FormResizableProxy(std::make_unique<FormPolylineHandlesPrivate>(resizable,
                                                                      this,
                                                                      form),
                         parent)
{
}

FormPolylineHandles::~FormPolylineHandles() = default;

NodesEditResizeHandle::Mode FormPolylineHandles::currentMode() const
{
    return d_ptr()->m_modeHandle->currentMode();
}

void FormPolylineHandles::setCurrentMode(NodesEditResizeHandle::Mode m)
{
    d_ptr()->m_modeHandle->setCurrentMode(m);
}

QRectF FormPolylineHandles::boundingRect() const
{
    return FormResizableProxy::boundingRect().adjusted(-c_halfHandleSize * c_halfDivider * c_halfDivider,
                                                       -c_halfHandleSize * c_halfDivider * c_halfDivider,
                                                       0.0,
                                                       0.0);
}

void FormPolylineHandles::setHandlesVisible(bool on)
{
    if (on) {
        d->m_handles->show();
    } else {
        d->m_handles->hide();
    }
}

} /* namespace Core */

} /* namespace Prototyper */
