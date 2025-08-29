
/*
    SPDX-FileCopyrightText: 2025 Igor Mironchik <igor.mironchik@gmail.com>
    SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef PROTOTYPER__CORE__FORM_IMAGE_HANDLES_HPP__INCLUDED
#define PROTOTYPER__CORE__FORM_IMAGE_HANDLES_HPP__INCLUDED

// Prototyper include.
#include "resizable.hpp"

namespace Prototyper
{

namespace Core
{

//
// FormImageHandles
//

class FormImageHandlesPrivate;

//! Image handles.
class FormImageHandles final : public FormResizableProxy
{
public:
    FormImageHandles(FormResizable *resizable,
                     QGraphicsItem *parent,
                     Page *form);
    ~FormImageHandles() override;

    //! \return Is keep aspect ratio?
    bool isKeepAspectRatio() const;
    //! Set keep aspect ratio.
    void setKeepAspectRatio(bool on = true);

    QRectF boundingRect() const override;

private:
    inline const FormImageHandlesPrivate *d_ptr() const
    {
        return reinterpret_cast<const FormImageHandlesPrivate *>(d.get());
    }
    inline FormImageHandlesPrivate *d_ptr()
    {
        return reinterpret_cast<FormImageHandlesPrivate *>(d.get());
    }

private:
    Q_DISABLE_COPY(FormImageHandles)
}; // class FormImageHandles

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__FORM_IMAGE_HANDLES_HPP__INCLUDED
