
/*!
	\file

	\author Igor Mironchik (igor.mironchik at gmail dot com).

	Copyright (c) 2016-2020 Igor Mironchik

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// Prototyper include.
#include "form_radio_button.hpp"
#include "form_checkbox_properties.hpp"
#include "form_image_handles.hpp"
#include "page.hpp"
#include "form_undo_commands.hpp"
#include "form_checkbox_private.hpp"
#include "constants.hpp"

// Qt include.
#include <QWidget>
#include <QPainter>
#include <QMenu>
#include <QAction>
#include <QGraphicsSceneContextMenuEvent>
#include <QUndoStack>
#include <QGraphicsScene>


namespace Prototyper {

namespace Core {

//
// FormRadioButton
//

FormRadioButton::FormRadioButton( const QRectF & rect, Page * page,
	QGraphicsItem * parent )
	:	FormCheckBox( rect, page, FormObject::RadioButtonType, parent )
{
}

FormRadioButton::~FormRadioButton() = default;

void
FormRadioButton::paint( QPainter * painter,
	const QStyleOptionGraphicsItem * option, QWidget * widget )
{
	Q_UNUSED( option )
	Q_UNUSED( widget )

	draw( painter,
		objectPen(),
		objectBrush(),
		d->m_font,
		QRectF( 0.0, 0.0, d->m_rect.width(), d->m_rect.height() ),
		d->m_width,
		d->m_checked,
		d->m_text,
		boundingRect() );

	if( isSelected() && !group() )
		d->m_handles->show();
	else
		d->m_handles->hide();
}

void
FormRadioButton::draw( QPainter * painter, const QPen & pen, const QBrush & brush,
	const QFont & font, const QRectF & rect, qreal width, bool isChecked, const QString & text,
	const QRectF & boundingRect, int dpi )
{
	Q_UNUSED( width )

	painter->setPen( pen );
	painter->setBrush( brush );

	QRectF r = rect;

	if( r.height() > boxHeight( dpi ) )
	{
		r.setTopLeft( QPointF( r.topLeft().x(),
			r.topLeft().y() + ( r.height() - boxHeight( dpi ) ) / c_halfDivider ) );
		r.setHeight( boxHeight( dpi ) );
	}

	painter->drawEllipse( r );

	const qreal o = r.height() / 10.0;

	if( isChecked )
	{
		painter->setBrush( QBrush( pen.color() ) );

		painter->drawEllipse( r.adjusted( o, o, -o, -o ) );
	}

	painter->setFont( font );

	r = boundingRect;
	r.moveLeft( r.x() + boxHeight( dpi ) + o * c_halfDivider );

	painter->drawText( r, Qt::AlignLeft | Qt::AlignVCenter, text );
}

QString
FormRadioButton::typeName() const
{
	return tr( "Radio Button" );
}

FormObject *
FormRadioButton::clone() const
{
	auto * o = new FormRadioButton( rectangle(), page(), parentItem() );

	o->setCfg( cfg() );

	o->setObjectId( page()->nextId() );

	return o;
}

} /* namespace Core */

} /* namespace Prototyper */
