
/*
    SPDX-FileCopyrightText: 2016-2024 Igor Mironchik <igor.mironchik@gmail.com>
    SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef PROTOTYPER__CORE__FORM_MOVE_HANDLER_HPP__INCLUDED
#define PROTOTYPER__CORE__FORM_MOVE_HANDLER_HPP__INCLUDED

// Qt include.
#include <QCursor>
#include <QGraphicsObject>

// C++ include.
#include <memory>

namespace Prototyper
{

namespace Core
{

class FormMoveHandle;

//
// FormWithHandle
//

//! Base class for objects with handle.
class FormWithHandle
{
public:
    FormWithHandle();
    virtual ~FormWithHandle();

protected:
    friend class FormMoveHandle;

    //! Handle moved.
    virtual void handleMoved(const QPointF &delta,
                             FormMoveHandle *handle);
    //! Handle released.
    virtual void handleReleased(FormMoveHandle *handle);

private:
    Q_DISABLE_COPY(FormWithHandle)
}; // class FormWithHandle

class Page;
class FormMoveHandlePrivate;

//
// FormMoveHandle
//

//! Move handler on the form.
class FormMoveHandle : public QGraphicsObject
{
public:
    FormMoveHandle(qreal halfSize,
                   const QPointF &zero,
                   FormWithHandle *object,
                   QGraphicsItem *parent,
                   Page *form,
                   const QCursor &c = QCursor(),
                   bool followCursor = false);
    ~FormMoveHandle() override;

    //! \return Half of the size of the edge.
    qreal halfOfSize() const;

    QRectF boundingRect() const override;

    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget = 0) override;

    //! Ignore mouse events.
    void ignoreMouseEvents(bool on = true);

    //! Handle mouse move.
    bool handleMouseMove(const QPointF &point);

    //! Clear.
    void clear();

    //! \return Cursor.
    const QCursor &handleCursor() const;

protected:
    //! Handle was moved.
    virtual void moved(const QPointF &delta);
    //! Handle released.
    virtual void released(FormMoveHandle *handle);

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    bool eventFilter(QObject *watched,
                     QEvent *event) override;

protected:
    FormMoveHandle(std::unique_ptr<FormMoveHandlePrivate> &&dd,
                   QGraphicsItem *parent);

protected:
    std::unique_ptr<FormMoveHandlePrivate> d;

private:
    Q_DISABLE_COPY(FormMoveHandle)
}; // class FormMoveHandler

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__FORM_MOVE_HANDLER_HPP__INCLUDED
