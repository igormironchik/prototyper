
/*
    SPDX-FileCopyrightText: 2026 Igor Mironchik <igor.mironchik@gmail.com>
    SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef PROTOTYPER__CORE__FORM_GRID_SNAP_H__INCLUDED
#define PROTOTYPER__CORE__FORM_GRID_SNAP_H__INCLUDED

// Qt include.
#include <QGraphicsItem>

// C++ include.
#include <memory>

namespace Prototyper
{

namespace Core
{

//
// GridSnap
//

class GridSnapPrivate;

//! Grid snap.
class GridSnap final : public QGraphicsItem
{
public:
    explicit GridSnap(QGraphicsItem *parent);
    ~GridSnap() override;

    //! \return Current position.
    const QPointF &snapPos() const;
    //! Set position, it will calculate snap position.
    void setSnapPos(const QPointF &p);

    //! Set grid step.
    void setGridStep(int step);

    //! \return Point snapped to grid.
    QPointF snapped(const QPointF &p) const;

    QRectF boundingRect() const override;

    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget) override;

protected:
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

private:
    Q_DISABLE_COPY(GridSnap)

    std::unique_ptr<GridSnapPrivate> d;
}; // class GridSnap

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__FORM_GRID_SNAP_H__INCLUDED
