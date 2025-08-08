
/*
    SPDX-FileCopyrightText: 2016-2024 Igor Mironchik <igor.mironchik@gmail.com>
    SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef PROTOTYPER__CORE__FORM_CHECKBOX_PRIVATE_HPP__INCLUDED
#define PROTOTYPER__CORE__FORM_CHECKBOX_PRIVATE_HPP__INCLUDED

// Qt include.
#include <QPointer>
#include <QWidget>

// C++ include.
#include <memory>

// Prototyper include.
#include "properties/checkbox_properties.hpp"
#include "properties/object_properties.hpp"
#include "properties/text_properties.hpp"
#include "properties/text_style_properties.hpp"
#include "resizable.hpp"

namespace Prototyper
{

namespace Core
{

class FormCheckBox;

//
// FormCheckBoxPrivate
//

class FormCheckBoxPrivate
{
public:
    FormCheckBoxPrivate(FormCheckBox *parent,
                        const QRectF &rect);
    virtual ~FormCheckBoxPrivate();

    //! Init.
    void init();
    //! Set rect.
    void setRect(const QRectF &rect);
    //! Connect properties signals.
    void connectProperties();
    //! Disconnect properties signals.
    void disconnectProperties();

    //! Parent.
    FormCheckBox *q;
    //! Rect.
    QRectF m_rect;
    //! Checked?
    bool m_checked;
    //! Handles.
    std::unique_ptr<FormResizableProxy> m_handles;
    //! Font.
    QFont m_font;
    //! Text.
    QString m_text;
    //! Width.
    qreal m_width;
    //! Properties.
    QPointer<QWidget> m_props;
    //! Object properties.
    QPointer<ObjectProperties> m_objProps;
    //! Text properties.
    QPointer<TextProperties> m_textProps;
    //! Text properties.
    QPointer<TextStyleProperties> m_textStyleProps;
    //! Check properties.
    QPointer<CheckBoxProperties> m_checkProps;
}; // class FormCheckBoxPrivate

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__FORM_CHECKBOX_PRIVATE_HPP__INCLUDED
