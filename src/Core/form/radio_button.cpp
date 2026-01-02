
/*
    SPDX-FileCopyrightText: 2026 Igor Mironchik <igor.mironchik@gmail.com>
    SPDX-License-Identifier: GPL-3.0-or-later
*/

// Prototyper include.
#include "radio_button.hpp"
#include "../constants.hpp"
#include "checkbox_private.hpp"
#include "image_handles.hpp"
#include "page.hpp"
#include "properties/checkbox_properties.hpp"
#include "undo_commands.hpp"

// Qt include.
#include <QAction>
#include <QGraphicsScene>
#include <QGraphicsSceneContextMenuEvent>
#include <QMenu>
#include <QPainter>
#include <QUndoStack>
#include <QWidget>

namespace Prototyper
{

namespace Core
{

//
// FormRadioButton
//

FormRadioButton::FormRadioButton(const QRectF &rect,
                                 Page *page,
                                 QGraphicsItem *parent)
    : FormCheckBox(rect,
                   page,
                   FormObject::RadioButtonType,
                   parent)
{
}

FormRadioButton::~FormRadioButton() = default;

void FormRadioButton::paint(QPainter *painter,
                            const QStyleOptionGraphicsItem *option,
                            QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    draw(painter,
         objectPen(),
         objectBrush(),
         d->m_font,
         QRectF(0.0, 0.0, d->m_rect.width(), d->m_rect.height()),
         d->m_width,
         d->m_checked,
         d->m_text,
         boundingRect());

    if (isSelected() && !group()) {
        d->m_handles->show();
    } else {
        d->m_handles->hide();
    }
}

void FormRadioButton::draw(QPainter *painter,
                           const QPen &pen,
                           const QBrush &brush,
                           const QFont &font,
                           const QRectF &rect,
                           qreal width,
                           bool isChecked,
                           const QString &text,
                           const QRectF &boundingRect,
                           int dpi)
{
    Q_UNUSED(width)

    painter->setPen(pen);
    painter->setBrush(brush);

    QRectF r = rect;

    if (r.height() > boxHeight(dpi)) {
        r.setTopLeft(QPointF(r.topLeft().x(), r.topLeft().y() + (r.height() - boxHeight(dpi)) / c_halfDivider));
        r.setHeight(boxHeight(dpi));
    }

    painter->drawEllipse(r);

    const qreal o = r.height() / 10.0;

    if (isChecked) {
        painter->setBrush(QBrush(pen.color()));

        painter->drawEllipse(r.adjusted(o, o, -o, -o));
    }

    painter->setFont(font);

    r = boundingRect;
    r.moveLeft(r.x() + boxHeight(dpi) + o * c_halfDivider);

    painter->drawText(r, Qt::AlignLeft | Qt::AlignVCenter, text);
}

QString FormRadioButton::typeName() const
{
    return tr("Radio Button");
}

FormObject *FormRadioButton::clone() const
{
    auto *o = new FormRadioButton(rectangle(), page(), parentItem());

    o->setCfg(cfg());

    o->setObjectId(page()->nextId());

    return o;
}

} /* namespace Core */

} /* namespace Prototyper */
