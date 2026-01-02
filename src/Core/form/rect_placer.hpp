
/*
    SPDX-FileCopyrightText: 2026 Igor Mironchik <igor.mironchik@gmail.com>
    SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef PROTOTYPER__CORE__FORM_RECT_PLACER_HPP__INCLUDED
#define PROTOTYPER__CORE__FORM_RECT_PLACER_HPP__INCLUDED

// Qt include.
#include <QGraphicsItem>

// C++ include.
#include <memory>

namespace Prototyper
{

namespace Core
{

//
// FormRectPlacer
//

class FormRectPlacerPrivate;

//! Helper for placing rectangle items.
class FormRectPlacer final : public QGraphicsItem
{
public:
    explicit FormRectPlacer(QGraphicsItem *parent);
    ~FormRectPlacer();

    //! Set start pos.
    void setStartPos(const QPointF &pos);

    //! \return Rect.
    const QRectF &rect() const;

    //! Set end pos.
    void setEndPos(const QPointF &pos);

    QRectF boundingRect() const override;

    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget = 0) override;

private:
    Q_DISABLE_COPY(FormRectPlacer)

    std::unique_ptr<FormRectPlacerPrivate> d;
}; // class FormRectPlacer

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__FORM_RECT_PLACER_HPP__INCLUDED
