
/*
    SPDX-FileCopyrightText: 2025 Igor Mironchik <igor.mironchik@gmail.com>
    SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef PROTOTYPER__CORE__FORM_COMBOBOX_HPP__INCLUDED
#define PROTOTYPER__CORE__FORM_COMBOBOX_HPP__INCLUDED

// Qt include.
#include <QGraphicsItem>

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

//
// FormComboBox
//

class FormComboBoxPrivate;

//! Combobox on the form.
class FormComboBox final : public QGraphicsItem, public FormObject, public FormResizable
{
public:
    FormComboBox(const QRectF &rect,
                 Page *page,
                 QGraphicsItem *parent = 0);
    ~FormComboBox() override;

    //! \return Type.
    static ObjectType staticObjectType()
    {
        return ComboBoxType;
    }

    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget = 0) override;

    static void draw(QPainter *painter,
                     const QRectF &rect,
                     const QPen &pen,
                     const QBrush &brush,
                     int dpi = 0);

    static qreal boxHeight(int dpi = 0);

    //! Set pen.
    void setObjectPen(const QPen &p,
                      bool pushUndoCommand = true) override;
    //! Set brush.
    void setObjectBrush(const QBrush &b,
                        bool pushUndoCommand = true) override;

    //! \return Cfg.
    Cfg::ComboBox cfg() const;
    //! Set cfg.
    void setCfg(const Cfg::ComboBox &c);

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

    //! Clone object.
    FormObject *clone() const override;

protected:
    //! Resize.
    void resize(const QRectF &rect) override;
    //! Move resizable.
    void moveResizable(const QPointF &delta) override;

private:
    friend class FormComboBoxPrivate;

    Q_DISABLE_COPY(FormComboBox)

    std::unique_ptr<FormComboBoxPrivate> d;
}; // class FormComboBox

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__FORM_COMBOBOX_HPP__INCLUDED
