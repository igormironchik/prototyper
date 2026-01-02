
/*
    SPDX-FileCopyrightText: 2026 Igor Mironchik <igor.mironchik@gmail.com>
    SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef PROTOTYPER__CORE__FORM_SPINBOX_HPP__INCLUDED
#define PROTOTYPER__CORE__FORM_SPINBOX_HPP__INCLUDED

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
// FormSpinBox
//

class FormSpinBoxPrivate;

//! Spinbox on the form.
class FormSpinBox final : public QGraphicsObject, public FormObject, public FormResizable
{
    Q_OBJECT

public:
    FormSpinBox(const QRectF &rect,
                Page *page,
                QGraphicsItem *parent = 0);
    ~FormSpinBox() override;

    //! \return Type.
    static ObjectType staticObjectType()
    {
        return SpinBoxType;
    }

    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget = 0) override;

    static void draw(QPainter *painter,
                     const QRectF &rect,
                     const QPen &pen,
                     const QBrush &brush,
                     const QFont &font,
                     const QString &text,
                     int dpi = 0);

    static qreal boxHeight(int dpi = 0);

    //! Set pen.
    void setObjectPen(const QPen &p,
                      bool pushUndoCommand = true) override;
    //! Set brush.
    void setObjectBrush(const QBrush &b,
                        bool pushUndoCommand = true) override;

    //! \return Cfg.
    Cfg::SpinBox cfg() const;
    //! Set cfg.
    void setCfg(const Cfg::SpinBox &c);

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
                      bool pushUndoCommand) override;

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
    friend class FormSpinBoxPrivate;

    Q_DISABLE_COPY(FormSpinBox)

    std::unique_ptr<FormSpinBoxPrivate> d;
}; // class FormSpinBox

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__FORM_SPINBOX_HPP__INCLUDED
