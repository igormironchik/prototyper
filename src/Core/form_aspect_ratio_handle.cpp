
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
#include "form_aspect_ratio_handle.hpp"

// Qt include.
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>


namespace Prototyper {

namespace Core {

//
// AspectRatioHandlePrivate
//

class AspectRatioHandlePrivate {
public:
	explicit AspectRatioHandlePrivate( AspectRatioHandle * parent )
		:	q( parent )
		,	m_keepAspectRatio( true )
		,	m_hovered( false )
		,	m_pressed( false )
	{
	}

	//! Init.
	void init();

	//! Parent.
	AspectRatioHandle * q;
	//! Keep aspect ratio?
	bool m_keepAspectRatio;
	//! Hovered?
	bool m_hovered;
	//! Pressed?
	bool m_pressed;
}; // class AspectRatioHandlePrivate

void
AspectRatioHandlePrivate::init()
{
	q->setCursor( Qt::ArrowCursor );

	q->setAcceptHoverEvents( true );
}


//
// AspectRatioHandle
//

AspectRatioHandle::AspectRatioHandle( QGraphicsItem * parent )
	:	QGraphicsItem( parent )
	,	d( new AspectRatioHandlePrivate( this ) )
{
	d->init();
}

AspectRatioHandle::~AspectRatioHandle()
{
}

bool
AspectRatioHandle::isKeepAspectRatio() const
{
	return d->m_keepAspectRatio;
}

void
AspectRatioHandle::setKeepAspectRatio( bool on )
{
	d->m_keepAspectRatio = on;

	update();
}

QRectF
AspectRatioHandle::boundingRect() const
{
	return QRectF( 0.0, 0.0, 12.0, 12.0 );
}

void
AspectRatioHandle::paint( QPainter * painter,
	const QStyleOptionGraphicsItem * option, QWidget * widget )
{
	Q_UNUSED( widget )

	const QRectF r = option->rect;

	if( d->m_keepAspectRatio )
	{
		painter->setPen( QPen( Qt::black, 1.0, Qt::DashLine ) );

		painter->setBrush( Qt::NoBrush );

		painter->drawRect( r );

		painter->setPen( Qt::black );

		if( d->m_hovered )
			painter->setBrush( Qt::red );
		else
			painter->setBrush( Qt::white );

		painter->drawRect( r.adjusted( 0.0, 0.0, -6.0, -6.0 ) );
	}
	else
	{
		painter->setPen( QPen( Qt::black, 1.0, Qt::DashLine ) );

		painter->setBrush( Qt::NoBrush );

		painter->drawRect( r.adjusted( 0.0, 0.0, 0.0, -6.0 ) );

		painter->setPen( Qt::black );

		if( d->m_hovered )
			painter->setBrush( Qt::red );
		else
			painter->setBrush( Qt::white );

		painter->drawRect( r.adjusted( 0.0, 0.0, -6.0, 0.0 ) );
	}
}

void
AspectRatioHandle::hoverEnterEvent( QGraphicsSceneHoverEvent * event )
{
	d->m_hovered = true;

	update();

	event->accept();
}

void
AspectRatioHandle::hoverLeaveEvent( QGraphicsSceneHoverEvent * event )
{
	d->m_hovered = false;

	update();

	event->accept();
}

void
AspectRatioHandle::mousePressEvent( QGraphicsSceneMouseEvent * event )
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
AspectRatioHandle::mouseReleaseEvent( QGraphicsSceneMouseEvent * event )
{
	if( event->button() == Qt::LeftButton && d->m_pressed )
	{
		d->m_pressed = false;

		if( boundingRect().contains( event->pos() ) )
		{
			d->m_keepAspectRatio = !d->m_keepAspectRatio;

			update();

			event->accept();

			return;
		}
	}

	event->ignore();
}

} /* namespace Core */

} /* namespace Prototyper */
