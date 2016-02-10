
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
#include "form_resizable.hpp"
#include "with_resize_and_move_handles.hpp"
#include "form_resize_handle.hpp"

// Qt include.
#include <QStyleOptionGraphicsItem>
#include <QPainter>


namespace Prototyper {

namespace Core {

//
// FormResizable
//

FormResizable::FormResizable()
{
}

FormResizable::~FormResizable()
{
}

void
FormResizable::resize( const QRectF & rect )
{
	Q_UNUSED( rect )
}

void
FormResizable::moveResizable( const QPointF & delta )
{
	Q_UNUSED( delta )
}


//
// FormResizableProxyPrivate
//

class FormResizableProxyPrivate {
public:
	FormResizableProxyPrivate( FormResizable * resizable,
		FormResizableProxy * parent )
		:	q( parent )
		,	m_object( resizable )
		,	m_rect( 0.0, 0.0, 24.0, 24.0 )
		,	m_handles( q )
	{
	}

	//! Init.
	void init();

	//! Parent.
	FormResizableProxy * q;
	//! Object.
	FormResizable * m_object;
	//! Rect.
	QRectF m_rect;
	//! Handles.
	WithResizeAndMoveHandles m_handles;
}; // class FormResizableProxyPrivate;

void
FormResizableProxyPrivate::init()
{
	m_handles.setMinSize( QSizeF( 35.0, 35.0 ) );

	m_handles.show();

	q->setZValue( 999 );
}


//
// FormResizableProxy
//

FormResizableProxy::FormResizableProxy( FormResizable * resizable,
	QGraphicsItem * parent )
	:	QGraphicsItem( parent )
	,	d( 0 )
{
	QScopedPointer< FormResizableProxyPrivate > tmp(
		new FormResizableProxyPrivate( resizable, this ) );

	tmp->init();

	d.swap( tmp );
}

FormResizableProxy::~FormResizableProxy()
{
}

void
FormResizableProxy::setRect( const QRectF & rect )
{
	const QRectF tmp = d->m_rect.adjusted( -20.0, -20.0, 20.0, 20.0 );

	d->m_rect = rect;

	d->m_handles.place( boundingRect() );

	update();

	parentItem()->update( tmp );
}

void
FormResizableProxy::setMinSize( const QSizeF & min )
{
	d->m_handles.setMinSize( min );
}

QRectF
FormResizableProxy::boundingRect() const
{
	if( !d.isNull() )
		return d->m_rect.adjusted( -12.0, -12.0, 12.0, 12.0 );
	else
		return QRectF();
}

void
FormResizableProxy::paint( QPainter * painter,
	const QStyleOptionGraphicsItem * option,
	QWidget * widget )
{
	Q_UNUSED( widget )

	painter->setPen( QPen( Qt::gray, 1.0, Qt::DashLine ) );
	painter->setBrush( Qt::NoBrush );

	painter->drawRect( option->rect );
}

void
FormResizableProxy::handleMoved( const QPointF & delta, FormMoveHandle * handle )
{
	if( handle == d->m_handles.m_move )
	{
		d->m_rect.moveTop( d->m_rect.top() + delta.y() );
		d->m_rect.moveRight( d->m_rect.right() + delta.x() );

		setRect( d->m_rect );

		d->m_object->moveResizable( delta );
	}
	else if( handle == d->m_handles.m_topLeft )
	{
		const QRectF r = d->m_rect.adjusted( delta.x(), delta.y(), 0.0, 0.0 );

		if( d->m_handles.checkConstraint( r.size() ) )
			setRect( r );
	}
	else if( handle == d->m_handles.m_top )
	{
		const QRectF r = d->m_rect.adjusted( 0.0, delta.y(), 0.0, 0.0 );

		if( d->m_handles.checkConstraint( r.size() ) )
			setRect( r );
	}
	else if( handle == d->m_handles.m_topRight )
	{
		const QRectF r = d->m_rect.adjusted( 0.0, delta.y(), delta.x(), 0.0 );

		if( d->m_handles.checkConstraint( r.size() ) )
			setRect( r );
	}
	else if( handle == d->m_handles.m_right )
	{
		const QRectF r = d->m_rect.adjusted( 0.0, 0.0, delta.x(), 0.0 );

		if( d->m_handles.checkConstraint( r.size() ) )
			setRect( r );
	}
	else if( handle == d->m_handles.m_bottomRight )
	{
		const QRectF r = d->m_rect.adjusted( 0.0, 0.0, delta.x(), delta.y() );

		if( d->m_handles.checkConstraint( r.size() ) )
			setRect( r );
	}
	else if( handle == d->m_handles.m_bottom )
	{
		const QRectF r = d->m_rect.adjusted( 0.0, 0.0, 0.0, delta.y() );

		if( d->m_handles.checkConstraint( r.size() ) )
			setRect( r );
	}
	else if( handle == d->m_handles.m_bottomLeft )
	{
		const QRectF r = d->m_rect.adjusted( delta.x(), 0.0, 0.0, delta.y() );

		if( d->m_handles.checkConstraint( r.size() ) )
			setRect( r );
	}
	else if( handle == d->m_handles.m_left )
	{
		const QRectF r = d->m_rect.adjusted( delta.x(), 0.0, 0.0, 0.0 );

		if( d->m_handles.checkConstraint( r.size() ) )
			setRect( r );
	}
}

void
FormResizableProxy::handleReleased( FormMoveHandle * handle )
{
	if( handle != d->m_handles.m_move )
		d->m_object->resize( d->m_rect );
}

} /* namespace Core */

} /* namespace Prototyper */
