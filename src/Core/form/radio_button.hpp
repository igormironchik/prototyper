
/*
    SPDX-FileCopyrightText: 2025 Igor Mironchik <igor.mironchik@gmail.com>
    SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef PROTOTYPER__CORE__FORM_RADIOBUTTON_HPP__INCLUDED
#define PROTOTYPER__CORE__FORM_RADIOBUTTON_HPP__INCLUDED

// Prototyper include.
#include "checkbox.hpp"

namespace Prototyper
{

namespace Core
{

//
// FormRadioButton
//

//! Radio button on the form.
class FormRadioButton final : public FormCheckBox
{
    Q_OBJECT

public:
    FormRadioButton(const QRectF &rect,
                    Page *page,
                    QGraphicsItem *parent = 0);
    ~FormRadioButton() override;

    //! \return Type.
    static ObjectType staticObjectType()
    {
        return RadioButtonType;
    }

    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget = 0) override;

    //! Draw radio button.
    static void draw(QPainter *painter,
                     const QPen &pen,
                     const QBrush &brush,
                     const QFont &font,
                     const QRectF &rect,
                     qreal width,
                     bool isChecked,
                     const QString &text,
                     const QRectF &boundingRect,
                     int dpi = 0);

    QString typeName() const override;

    //! Clone object.
    FormObject *clone() const override;

private:
    Q_DISABLE_COPY(FormRadioButton)
}; // class FormRadioButton

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__FORM_RADIOBUTTON_HPP__INCLUDED
