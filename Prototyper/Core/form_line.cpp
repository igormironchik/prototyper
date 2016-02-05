
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
#include "form_line.hpp"

// Qt include.
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>


namespace Prototyper {

namespace Core {

//
// FormLinePrivate
//

class FormLinePrivate {
public:
	FormLinePrivate( FormLine * parent )
		:	q( parent )
	{
	}

	//! Init.
	void init();

	//! Parent.
	FormLine * q;
}; // class FormLinePrivate

void
FormLinePrivate::init()
{
}


//
// FormLine
//

FormLine::FormLine( QGraphicsItem * parent )
	:	QGraphicsLineItem( parent )
	,	d( new FormLinePrivate( this ) )
{
	d->init();
}

FormLine::~FormLine()
{
}

QRectF
FormLine::boundingRect() const
{
	return QGraphicsLineItem::boundingRect();
}

void
FormLine::paint( QPainter * painter, const QStyleOptionGraphicsItem * option,
	QWidget * widget )
{
	QGraphicsLineItem::paint( painter, option, widget );
}

void
FormLine::hoverEnterEvent( QGraphicsSceneHoverEvent * event )
{
	QGraphicsLineItem::hoverEnterEvent( event );
}

void
FormLine::hoverMoveEvent( QGraphicsSceneHoverEvent * event )
{
	QGraphicsLineItem::hoverMoveEvent( event );
}

void
FormLine::mouseMoveEvent( QGraphicsSceneMouseEvent * event )
{
	QGraphicsLineItem::mouseMoveEvent( event );
}

void
FormLine::mousePressEvent( QGraphicsSceneMouseEvent * event )
{
	QGraphicsLineItem::mousePressEvent( event );
}

void
FormLine::mouseReleaseEvent( QGraphicsSceneMouseEvent * event )
{
	QGraphicsLineItem::mouseReleaseEvent( event );
}

} /* namespace Core */

} /* namespace Prototyper */
