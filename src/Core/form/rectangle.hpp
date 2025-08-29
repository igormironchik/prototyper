
/*
    SPDX-FileCopyrightText: 2025 Igor Mironchik <igor.mironchik@gmail.com>
    SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef PROTOTYPER__CORE__FORM_RECTANGLE_HPP__INCLUDED
#define PROTOTYPER__CORE__FORM_RECTANGLE_HPP__INCLUDED

// Qt include.
#include <QGraphicsItem>

// C++ include.
#include <memory>

// Prototyper include.
#include "move_handle.hpp"
#include "object.hpp"
#include "project_cfg.hpp"

namespace Prototyper
{

namespace Core
{

class Page;

//
// FormRect
//

class FormRectPrivate;

//! Rect on the form.
class FormRect final : public QGraphicsItem, public FormObject, public FormWithHandle
{
public:
    explicit FormRect(Page *page,
                      QGraphicsItem *parent = 0);
    ~FormRect() override;

    //! \return Type.
    static ObjectType staticObjectType()
    {
        return RectType;
    }

    //! \return Cfg.
    Cfg::Rect cfg() const;
    //! Set cfg.
    void setCfg(const Cfg::Rect &c);

    QRectF boundingRect() const override;

    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget = 0) override;

    //! Position elements.
    void setPosition(const QPointF &pos,
                     bool pushUndoCommand = true) override;
    //! \return Position of the element.
    QPointF position() const override;

    //! Set rect.
    void setRectangle(const QRectF &r,
                      bool pushUndoCommand = true) override;
    //! \return Rectangle of the element.
    QRectF rectangle() const override;

    //! Set pen.
    void setObjectPen(const QPen &p,
                      bool pushUndoCommand = true) override;
    //! Set brush.
    void setObjectBrush(const QBrush &b,
                        bool pushUndoCommand = true) override;

    //! Clone object.
    FormObject *clone() const override;

protected:
    //! Handle moved.
    void handleMoved(const QPointF &delta,
                     FormMoveHandle *handle) override;
    //! Handle released.
    void handleReleased(FormMoveHandle *handle) override;

private:
    Q_DISABLE_COPY(FormRect)

    std::unique_ptr<FormRectPrivate> d;
}; // class FormRect

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__FORM_RECTANGLE_HPP__INCLUDED
