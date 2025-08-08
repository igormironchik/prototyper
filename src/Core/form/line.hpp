
/*
    SPDX-FileCopyrightText: 2016-2024 Igor Mironchik <igor.mironchik@gmail.com>
    SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef PROTOTYPER__CORE__FORM_LINE_HPP__INCLUDED
#define PROTOTYPER__CORE__FORM_LINE_HPP__INCLUDED

// Qt include.
#include <QGraphicsLineItem>

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
class FormLineMoveHandle;

//
// FormLine
//

class FormLinePrivate;

//! Line on the form.
class FormLine final : public QGraphicsLineItem, public FormObject, public FormWithHandle
{
public:
    explicit FormLine(Page *page,
                      QGraphicsItem *parent = 0);
    ~FormLine() override;

    //! \return Type.
    static ObjectType staticObjectType()
    {
        return LineType;
    }

    //! \return Cfg.
    Cfg::Line cfg() const;
    //! Set cfg.
    void setCfg(const Cfg::Line &c);

    QRectF boundingRect() const override;

    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget = 0) override;

    //! Set pen.
    void setObjectPen(const QPen &p,
                      bool pushUndoCommand = true) override;

    //! Show handles.
    void showHandles(bool show = true);

    //! \return Point in the middle of handle if
    //! given point contained by handle.
    QPointF pointUnderHandle(const QPointF &point,
                             bool &intersected,
                             bool &intersectedEnds) const;

    //! Handle mouse move in handles.
    bool handleMouseMoveInHandles(const QPointF &point);

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

    //! Clone object.
    FormObject *clone() const override;

protected:
    //! Handle moved.
    void handleMoved(const QPointF &delta,
                     FormMoveHandle *handle) override;
    //! Handle released.
    void handleReleased(FormMoveHandle *handle) override;

protected:
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

protected:
    friend class UndoChangeLine;

    //! Place handles.
    void placeHandles();

private:
    Q_DISABLE_COPY(FormLine)

    std::unique_ptr<FormLinePrivate> d;
}; // class FormLine

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__FORM_LINE_HPP__INCLUDED
