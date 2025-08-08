
/*
    SPDX-FileCopyrightText: 2016-2024 Igor Mironchik <igor.mironchik@gmail.com>
    SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef PROTOTYPER__CORE__FORM_POLYLINE_HANDLES_HPP__INCLUDED
#define PROTOTYPER__CORE__FORM_POLYLINE_HANDLES_HPP__INCLUDED

// Prototyper include.
#include "nodes_edit_resize_handle.hpp"
#include "resizable.hpp"

namespace Prototyper
{

namespace Core
{

//
// FormPolylineHandles
//

class FormPolylineHandlesPrivate;

//! Polyline handles.
class FormPolylineHandles final : public FormResizableProxy
{
    Q_OBJECT

signals:
    //! Current mode changed.
    void currentModeChanged();

public:
    FormPolylineHandles(FormResizable *resizable,
                        QGraphicsItem *parent,
                        Page *form);
    ~FormPolylineHandles() override;

    //! \return Current mode.
    NodesEditResizeHandle::Mode currentMode() const;
    //! Set mode.
    void setCurrentMode(NodesEditResizeHandle::Mode m);

    QRectF boundingRect() const override;

public slots:
    //! Hide/show resize handles.
    void setHandlesVisible(bool on = true);

private:
    inline const FormPolylineHandlesPrivate *d_ptr() const
    {
        return reinterpret_cast<const FormPolylineHandlesPrivate *>(d.get());
    }
    inline FormPolylineHandlesPrivate *d_ptr()
    {
        return reinterpret_cast<FormPolylineHandlesPrivate *>(d.get());
    }

private:
    Q_DISABLE_COPY(FormPolylineHandles)
}; // class FormPolylineHandles

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__FORM_POLYLINE_HANDLES_HPP__INCLUDED
