
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
#include "form_move_handle.hpp"
#include "form.hpp"
#include "grid_snap.hpp"
#include "form_actions.hpp"

// Qt include.
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneMouseEvent>


namespace Prototyper {

namespace Core {

//
// FormWithHandle
//

FormWithHandle::FormWithHandle()
{
}

FormWithHandle::~FormWithHandle()
{
}

void
FormWithHandle::handleMoved( const QPointF & delta, FormMoveHandle * handle )
{
	Q_UNUSED( delta )
	Q_UNUSED( handle )
}

void
FormWithHandle::handleReleased( FormMoveHandle * handle )
{
	Q_UNUSED( handle )
}


//
// FormMoveHandlePrivate
//

FormMoveHandlePrivate::FormMoveHandlePrivate( qreal halfSize,
	const QPointF & zero, FormWithHandle * object, FormMoveHandle * parent,
	const QCursor & c )
	:	q( parent )
	,	m_object( object )
	,	m_size( halfSize )
	,	m_hovered( false )
	,	m_pressed( false )
	,	m_ignoreMouse( false )
	,	m_zero( zero )
	,	m_cursor( c )
{
}

FormMoveHandlePrivate::~FormMoveHandlePrivate()
{
}

void
FormMoveHandlePrivate::init()
{
	q->setAcceptHoverEvents( true );

	q->setCursor( m_cursor );
}


//
// FormMoveHandle
//

FormMoveHandle::FormMoveHandle( qreal halfSize, const QPointF & zero,
	FormWithHandle * object, QGraphicsItem * parent, const QCursor & c )
	:	QGraphicsItem( parent )
	,	d( new FormMoveHandlePrivate( halfSize, zero, object, this, c ) )
{
	d->init();
}

FormMoveHandle::FormMoveHandle( FormMoveHandlePrivate * dd,
	QGraphicsItem * parent )
	:	QGraphicsItem( parent )
	,	d( dd )
{
	d->init();
}

FormMoveHandle::~FormMoveHandle()
{
}

qreal
FormMoveHandle::halfOfSize() const
{
	return d->m_size;
}

QRectF
FormMoveHandle::boundingRect() const
{
	return QRectF( 0, 0, d->m_size * 2, d->m_size * 2 );
}

void
FormMoveHandle::paint( QPainter * painter,
	const QStyleOptionGraphicsItem * option, QWidget * widget )
{
	Q_UNUSED( widget )

	painter->setRenderHint( QPainter::Antialiasing, false );

	painter->setPen( Qt::black );

	if( d->m_hovered )
		painter->setBrush( Qt::red );
	else
		painter->setBrush( Qt::white );

	painter->drawRect( option->rect );
}

void
FormMoveHandle::moved( const QPointF & delta )
{
	d->m_object->handleMoved( delta, this );
}

void
FormMoveHandle::released( FormMoveHandle * handle )
{
	d->m_object->handleReleased( handle );
}

void
FormMoveHandle::ignoreMouseEvents( bool on )
{
	d->m_ignoreMouse = on;

	if( !d->m_ignoreMouse )
	{
		d->m_hovered = false;

		update();
	}
}

bool
FormMoveHandle::handleMouseMove( const QPointF & point )
{
	if( contains( mapFromScene( point ) ) )
	{
		d->m_hovered = true;

		update();

		return true;
	}
	else
	{
		d->m_hovered = false;

		update();

		return false;
	}
}

void
FormMoveHandle::clear()
{
	d->m_hovered = false;

	update();
}

const QCursor &
FormMoveHandle::handleCursor() const
{
	return d->m_cursor;
}

void
FormMoveHandle::hoverEnterEvent( QGraphicsSceneHoverEvent * event )
{
	d->m_hovered = true;

	update();

	QGraphicsItem::hoverEnterEvent( event );
}

void
FormMoveHandle::hoverLeaveEvent( QGraphicsSceneHoverEvent * event )
{
	d->m_hovered = false;

	update();

	QGraphicsItem::hoverLeaveEvent( event );
}

void
FormMoveHandle::mouseMoveEvent( QGraphicsSceneMouseEvent * event )
{
	FormAction::instance()->form()->snapItem()->setSnapPos(
		mapToScene( event->pos() ) );

	if( d->m_pressed && !d->m_ignoreMouse )
	{
		const QPointF delta = event->pos() - d->m_pos;

		moved( delta );

		event->accept();
	}
	else
		event->ignore();
}

void
FormMoveHandle::mousePressEvent( QGraphicsSceneMouseEvent * event )
{
	FormAction::instance()->form()->snapItem()->setSnapPos(
		mapToScene( event->pos() ) );

	if( event->button() == Qt::LeftButton && !d->m_ignoreMouse )
	{
		d->m_pressed = true;
		d->m_pos = event->pos();
		d->m_touchDelta = event->pos() - d->m_zero;

		event->accept();
	}
	else
		event->ignore();
}

void
FormMoveHandle::mouseReleaseEvent( QGraphicsSceneMouseEvent * event )
{
	FormAction::instance()->form()->snapItem()->setSnapPos(
		mapToScene( event->pos() ) );

	if( event->button() == Qt::LeftButton )
	{
		d->m_pressed = false;

		if( FormAction::instance()->isSnapEnabled() )
		{
			const QPointF delta =
				FormAction::instance()->form()->snapItem()->snapPos() -
				mapToScene( d->m_pos ) + d->m_touchDelta;

			setPos( pos() + delta );

			moved( delta );

			released( this );
		}
	}

	if( !d->m_ignoreMouse )
		event->accept();
	else
		event->ignore();
}

} /* namespace Core */

} /* namespace Prototyper */
