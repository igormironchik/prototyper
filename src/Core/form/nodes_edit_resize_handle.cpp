
/*!
	\file

	\author Igor Mironchik (igor.mironchik at gmail dot com).

	Copyright (c) 2016-2021 Igor Mironchik

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
#include "nodes_edit_resize_handle.hpp"
#include "../constants.hpp"

// Qt include.
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>


namespace Prototyper {

namespace Core {

//
// NodesEditResizeHandlePrivate
//

class NodesEditResizeHandlePrivate {
public:
	explicit NodesEditResizeHandlePrivate( NodesEditResizeHandle * parent )
		:	q( parent )
		,	m_mode( NodesEditResizeHandle::ResizeMode )
		,	m_pressed( false )
	{
	}

	//! Init.
	void init();
	//! Set tool tip.
	void setToolTip();

	//! Parent.
	NodesEditResizeHandle * q;
	//! Mode.
	NodesEditResizeHandle::Mode m_mode;
	//! Pressed?
	bool m_pressed;
}; // class NodesEditResizeHandlePrivate

void
NodesEditResizeHandlePrivate::init()
{
	q->setCursor( Qt::ArrowCursor );

	setToolTip();
}

void
NodesEditResizeHandlePrivate::setToolTip()
{
	if( m_mode == NodesEditResizeHandle::ResizeMode )
		q->setToolTip( QObject::tr( "Switch to nodes edit mode" ) );
	else
		q->setToolTip( QObject::tr( "Switch to resize mode" ) );
}


//
// NodesEditResizeHandle
//

NodesEditResizeHandle::NodesEditResizeHandle( QGraphicsItem * parent )
	:	QGraphicsObject( parent )
	,	d( new NodesEditResizeHandlePrivate( this ) )
{
	d->init();
}

NodesEditResizeHandle::~NodesEditResizeHandle() = default;

NodesEditResizeHandle::Mode
NodesEditResizeHandle::currentMode() const
{
	return d->m_mode;
}

void
NodesEditResizeHandle::setCurrentMode( Mode m )
{
	d->m_mode = m;

	emit currentModeChanged();

	update();
}

static const qreal c_halfSize = 6.0;

QRectF
NodesEditResizeHandle::boundingRect() const
{
	return { 0.0, 0.0, c_halfSize * c_halfDivider, c_halfSize * c_halfDivider };
}

void
NodesEditResizeHandle::paint( QPainter * painter,
	const QStyleOptionGraphicsItem * option, QWidget * widget )
{
	Q_UNUSED( widget )

	painter->setPen( QPen( Qt::black, 1.0, Qt::DashLine ) );

	painter->setBrush( Qt::NoBrush );

	painter->drawRect( option->rect );

	painter->setPen( QPen( Qt::black, 2.0, Qt::SolidLine ) );

	if( d->m_mode == ResizeMode )
	{
		painter->drawLine( 3, 3, c_halfSize * c_halfDivider - 3, c_halfSize * c_halfDivider - 3 );
		painter->drawLine( 3, c_halfSize * c_halfDivider - 3, c_halfSize * c_halfDivider - 3, 3 );
	}
	else
	{
		painter->drawLine( 3, c_halfSize, c_halfSize * c_halfDivider - 3, c_halfSize );
		painter->drawLine( c_halfSize, 3, c_halfSize, c_halfSize * c_halfDivider - 3 );
	}
}

void
NodesEditResizeHandle::mousePressEvent( QGraphicsSceneMouseEvent * event )
{
	if( event->button() == Qt::LeftButton )
	{
		d->m_pressed = true;

		event->accept();
	}
	else
		event->ignore();
}

void
NodesEditResizeHandle::mouseReleaseEvent( QGraphicsSceneMouseEvent * event )
{
	if( event->button() == Qt::LeftButton && d->m_pressed )
	{
		d->m_pressed = false;

		if( boundingRect().contains( event->pos() ) )
		{
			d->m_mode = ( d->m_mode == ResizeMode ? NodesEditMode : ResizeMode );

			d->setToolTip();

			emit currentModeChanged();

			update();

			event->accept();

			return;
		}
	}

	event->ignore();
}

} /* namespace Core */

} /* namespace Prototyper */
