
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
#include "form_move_handle.hpp"
#include "page.hpp"
#include "form_grid_snap.hpp"
#include "form_actions.hpp"
#include "form_move_handle_private.hpp"
#include "constants.hpp"

// Qt include.
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QEvent>
#include <QGraphicsScene>


namespace Prototyper {

namespace Core {

//
// FormWithHandle
//

FormWithHandle::FormWithHandle() = default;

FormWithHandle::~FormWithHandle() = default;

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
	Page * form, const QCursor & c )
	:	q( parent )
	,	m_object( object )
	,	m_size( halfSize )
	,	m_hovered( false )
	,	m_pressed( false )
	,	m_ignoreMouse( false )
	,	m_zero( zero )
	,	m_addToZero( 0.0, 0.0 )
	,	m_cursor( c )
	,	m_form( form )
{
}

FormMoveHandlePrivate::~FormMoveHandlePrivate() = default;

void
FormMoveHandlePrivate::init()
{
	q->setAcceptHoverEvents( true );

	q->setCursor( m_cursor );

	q->setZValue( c_mostTopZValue );
}


//
// FormMoveHandle
//

FormMoveHandle::FormMoveHandle( qreal halfSize, const QPointF & zero,
	FormWithHandle * object, QGraphicsItem * parent, Page * form,
	const QCursor & c )
	:	QGraphicsObject( parent )
	,	d( nullptr )
{
	auto tmp = std::make_unique< FormMoveHandlePrivate >( halfSize, zero, object, this, form, c );

	tmp->init();

	d.swap( tmp );
}

FormMoveHandle::FormMoveHandle( std::unique_ptr< FormMoveHandlePrivate > && dd,
	QGraphicsItem * parent )
	:	QGraphicsObject( parent )
	,	d( nullptr )
{
	std::unique_ptr< FormMoveHandlePrivate > tmp;

	tmp.swap( dd );

	tmp->init();

	d.swap( tmp );
}

FormMoveHandle::~FormMoveHandle() = default;

qreal
FormMoveHandle::halfOfSize() const
{
	return d->m_size;
}

QRectF
FormMoveHandle::boundingRect() const
{
	if( d )
		return { 0, 0, d->m_size * c_halfDivider, d->m_size * c_halfDivider };

	return {};
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

	d->m_hovered = false;

	update();

	return false;
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
FormMoveHandle::setDeltaToZero( const QPointF & delta )
{
	d->m_addToZero = delta;
}

void
FormMoveHandle::hoverEnterEvent( QGraphicsSceneHoverEvent * event )
{
	PageAction::instance()->page()->snapItem()->setSnapPos(
		mapToScene( event->pos() ) );

	d->m_hovered = true;

	update();

	QGraphicsItem::hoverEnterEvent( event );
}

void
FormMoveHandle::hoverMoveEvent( QGraphicsSceneHoverEvent * event )
{
	PageAction::instance()->page()->snapItem()->setSnapPos(
		mapToScene( event->pos() ) );

	QGraphicsItem::hoverMoveEvent( event );
}

void
FormMoveHandle::hoverLeaveEvent( QGraphicsSceneHoverEvent * event )
{
	PageAction::instance()->page()->snapItem()->setSnapPos(
		mapToScene( event->pos() ) );

	d->m_hovered = false;

	update();

	QGraphicsItem::hoverLeaveEvent( event );
}

void
FormMoveHandle::mouseMoveEvent( QGraphicsSceneMouseEvent * event )
{
	PageAction::instance()->page()->snapItem()->setSnapPos(
		event->scenePos() );

	if( d->m_pressed && !d->m_ignoreMouse )
	{
		const QPointF delta = mapFromScene( event->scenePos() ) - d->m_pos;

		moved( delta );

		event->accept();
	}
	else
		event->ignore();
}

void
FormMoveHandle::mousePressEvent( QGraphicsSceneMouseEvent * event )
{
	PageAction::instance()->page()->snapItem()->setSnapPos(
		event->scenePos() );

	if( event->button() == Qt::LeftButton && !d->m_ignoreMouse )
	{
		d->m_pressed = true;
		d->m_pos = mapFromScene( event->scenePos() );

		d->m_touchDelta = d->m_pos - d->m_zero - d->m_addToZero;

		event->accept();
	}
	else
		event->ignore();
}

void
FormMoveHandle::mouseReleaseEvent( QGraphicsSceneMouseEvent * event )
{
	PageAction::instance()->page()->snapItem()->setSnapPos(
		event->scenePos() );

	if( event->button() == Qt::LeftButton )
	{
		d->m_pressed = false;

		QPointF delta( 0.0, 0.0 );

		if( PageAction::instance()->isSnapEnabled() )
			delta =
				PageAction::instance()->page()->snapItem()->snapPos() -
				mapToScene( d->m_pos ) + d->m_touchDelta;

		setPos( pos() + delta );

		moved( delta );

		released( this );
	}

	if( !d->m_ignoreMouse )
		event->accept();
	else
		event->ignore();
}

bool
FormMoveHandle::eventFilter( QObject * watched, QEvent * event )
{
	if( watched == d->m_form->scene() )
	{
		switch( event->type() )
		{
			case QEvent::GraphicsSceneMouseMove :
			{
				if( d->m_pressed )
				{
					auto * me = dynamic_cast< QGraphicsSceneMouseEvent* > ( event );

					mouseMoveEvent( me );

					return true;
				}

				return false;
			}
				break;

			case QEvent::GraphicsSceneMousePress :
			{
				auto * me = dynamic_cast< QGraphicsSceneMouseEvent* > ( event );

				mousePressEvent( me );

				return true;
			}
				break;

			case QEvent::GraphicsSceneMouseRelease :
			{
				if( d->m_pressed )
				{
					auto * me = dynamic_cast< QGraphicsSceneMouseEvent* > ( event );

					mouseReleaseEvent( me );

					return true;
				}

				return false;
			}
				break;

			default :
				return false;
		}
	}
	else
		return false;
}

} /* namespace Core */

} /* namespace Prototyper */
