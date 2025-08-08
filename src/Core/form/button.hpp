
/*
    SPDX-FileCopyrightText: 2016-2024 Igor Mironchik <igor.mironchik@gmail.com>
    SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef PROTOTYPER__CORE__FORM_BUTTON_HPP__INCLUDED
#define PROTOTYPER__CORE__FORM_BUTTON_HPP__INCLUDED

// Qt include.
#include <QGraphicsObject>

// C++ include.
#include <memory>

// Prototyper include.
#include "object.hpp"
#include "project_cfg.hpp"
#include "resizable.hpp"

namespace Prototyper
{

namespace Core
{

class TextWithOpts;

//
// FormButton
//

class FormButtonPrivate;

//! Button on the form.
class FormButton final : public QGraphicsObject, public FormObject, public FormResizable
{
    Q_OBJECT

public:
    FormButton(const QRectF &rect,
               Page *page,
               QGraphicsItem *parent = 0);
    ~FormButton() override;

    //! \return Type.
    static ObjectType staticObjectType()
    {
        return ButtonType;
    }

    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget = 0) override;

    //! Set pen.
    void setObjectPen(const QPen &p,
                      bool pushUndoCommand = true) override;
    //! Set brush.
    void setObjectBrush(const QBrush &b,
                        bool pushUndoCommand = true) override;

    //! \return Cfg.
    Cfg::Button cfg() const;
    //! Set cfg.
    void setCfg(const Cfg::Button &c);

    //! \return Text.
    Cfg::TextStyle text() const;
    //! Set text.
    void setText(const Cfg::TextStyle &c);

    QRectF boundingRect() const override;

    //! Position elements.
    void setPosition(const QPointF &pos,
                     bool pushUndoCommand = true) override;
    //! \return Position of the element.
    QPointF position() const override;

    //! \return Rectangle of the element.
    QRectF rectangle() const override;
    //! Set rectangle.
    void setRectangle(const QRectF &rect,
                      bool pushUndoCommand = true) override;

    //! \return Default size.
    virtual QSizeF defaultSize() const override;

    QWidget *properties(QWidget *parent) override;
    void updatePropertiesValues() override;

    //! Clone object.
    FormObject *clone() const override;

protected:
    //! Resize.
    void resize(const QRectF &rect) override;
    //! Move resizable.
    void moveResizable(const QPointF &delta) override;

private:
    friend class FormButtonPrivate;

    Q_DISABLE_COPY(FormButton)

    std::unique_ptr<FormButtonPrivate> d;
}; // class FormButton

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__FORM_BUTTON_HPP__INCLUDED
