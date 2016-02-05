
/*!
	\file

	\author Igor Mironchik (igor.mironchik at gmail dot com).

	Copyright (c) 2016 Igor Mironchik

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
#include "form_text.hpp"

// Qt include.
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>


namespace Prototyper {

namespace Core {

//
// FormTextPrivate
//

class FormTextPrivate {
public:
	FormTextPrivate( FormText * parent )
		:	q( parent )
	{
	}

	//! Init.
	void init();

	//! Parent.
	FormText * q;
}; // class FormTextPrivate

void
FormTextPrivate::init()
{
}


//
// FormText
//

FormText::FormText( QGraphicsItem * parent )
	:	QGraphicsTextItem( parent )
	,	d( new FormTextPrivate( this ) )
{
	d->init();
}

FormText::~FormText()
{
}

QRectF
FormText::boundingRect() const
{
	return QGraphicsTextItem::boundingRect();
}

void
FormText::paint( QPainter * painter, const QStyleOptionGraphicsItem * option,
	QWidget * widget )
{
	QGraphicsTextItem::paint( painter, option, widget );
}

void
FormText::hoverEnterEvent( QGraphicsSceneHoverEvent * event )
{
	QGraphicsTextItem::hoverEnterEvent( event );
}

void
FormText::hoverMoveEvent( QGraphicsSceneHoverEvent * event )
{
	QGraphicsTextItem::hoverMoveEvent( event );
}

void
FormText::mouseMoveEvent( QGraphicsSceneMouseEvent * event )
{
	QGraphicsTextItem::mouseMoveEvent( event );
}

void
FormText::mousePressEvent( QGraphicsSceneMouseEvent * event )
{
	QGraphicsTextItem::mousePressEvent( event );
}

void
FormText::mouseReleaseEvent( QGraphicsSceneMouseEvent * event )
{
	QGraphicsTextItem::mouseReleaseEvent( event );
}

} /* namespace Core */

} /* namespace Prototyper */
