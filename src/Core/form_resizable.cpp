
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
#include "form_resizable.hpp"
#include "form_with_resize_and_move_handles.hpp"
#include "form_resize_handle.hpp"
#include "page.hpp"
#include "form_undo_commands.hpp"
#include "form_object.hpp"
#include "form_resizable_private.hpp"
#include "constants.hpp"

// Qt include.
#include <QStyleOptionGraphicsItem>
#include <QPainter>
#include <QUndoStack>


namespace Prototyper {

namespace Core {

//
// FormResizable
//

FormResizable::FormResizable() = default;

FormResizable::~FormResizable() = default;

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

FormResizableProxyPrivate::FormResizableProxyPrivate( FormResizable * resizable,
	FormResizableProxy * parent, Page * form )
	:	q( parent )
	,	m_object( resizable )
	,	m_rect( 0.0, 0.0,
			c_halfHandleSize * c_halfDivider * c_halfDivider * c_halfDivider,
			c_halfHandleSize * c_halfDivider * c_halfDivider * c_halfDivider )
	,	m_handles( nullptr )
	,	m_form( form )
	,	m_handleMoved( false )
{
}

FormResizableProxyPrivate::~FormResizableProxyPrivate() = default;

void
FormResizableProxyPrivate::init()
{
	auto tmp = std::make_unique< WithResizeAndMoveHandles> ( q, q, m_form );

	m_handles.swap( tmp );

	m_handles->setMinSize( QSizeF( c_minResizableSize, c_minResizableSize ) );

	m_handles->show();

	q->setZValue( c_mostTopZValue );
}

void
FormResizableProxyPrivate::place( const QRectF & rect )
{
	m_handles->place( rect );
}


//
// FormResizableProxy
//

FormResizableProxy::FormResizableProxy( FormResizable * resizable,
	QGraphicsItem * parent, Page * form )
	:	QGraphicsItem( parent )
	,	d( nullptr )
{
	auto tmp = std::make_unique< FormResizableProxyPrivate >( resizable, this, form );

	tmp->init();

	d.swap( tmp );
}

FormResizableProxy::FormResizableProxy(
	std::unique_ptr< FormResizableProxyPrivate > && dd,
	QGraphicsItem * parent )
	:	QGraphicsItem( parent )
	,	d( nullptr )
{
	std::unique_ptr< FormResizableProxyPrivate > tmp;

	tmp.swap( dd );

	tmp->init();

	d.swap( tmp );
}

FormResizableProxy::~FormResizableProxy() = default;

void
FormResizableProxy::setRect( const QRectF & rect )
{
	const QRectF tmp = d->m_rect.adjusted( -20.0, -20.0, 20.0, 20.0 );

	d->m_rect = rect;

	setPos( d->m_rect.topLeft() );

	d->place( FormResizableProxy::boundingRect() );

	update();

	parentItem()->update( tmp );
}

void
FormResizableProxy::setMinSize( const QSizeF & min )
{
	d->m_handles->setMinSize( min );
}

QRectF
FormResizableProxy::boundingRect() const
{
	if( d )
	{
		QRectF r = d->m_rect;
		r.moveTopLeft( QPointF( 0.0, 0.0 ) );

		return r.adjusted( -c_halfHandleSize * c_halfDivider * c_halfDivider,
			-c_halfHandleSize * c_halfDivider * c_halfDivider,
			c_halfHandleSize * c_halfDivider * c_halfDivider,
			c_halfHandleSize * c_halfDivider * c_halfDivider );
	}

	return {};
}

void
FormResizableProxy::paint( QPainter * painter,
	const QStyleOptionGraphicsItem * option,
	QWidget * widget )
{
	Q_UNUSED( widget )
	Q_UNUSED( painter )
	Q_UNUSED( option )
}

void
FormResizableProxy::handleMoved( const QPointF & delta, FormMoveHandle * handle )
{
	if( !d->m_handleMoved )
	{
		d->m_subsidiaryRect = d->m_rect;

		d->m_handleMoved = true;
	}

	if( handle == d->m_handles->m_move.get() )
	{
		d->m_rect.moveTop( d->m_rect.top() + delta.y() );
		d->m_rect.moveRight( d->m_rect.right() + delta.x() );

		setRect( d->m_rect );

		d->m_object->moveResizable( delta );
	}
	else if( handle == d->m_handles->m_topLeft.get() )
	{
		const QRectF r = d->m_rect.adjusted( delta.x(), delta.y(), 0.0, 0.0 );

		if( d->m_handles->checkConstraint( r.size() ) )
			setRect( r );
	}
	else if( handle == d->m_handles->m_top.get() )
	{
		const QRectF r = d->m_rect.adjusted( 0.0, delta.y(), 0.0, 0.0 );

		if( d->m_handles->checkConstraint( r.size() ) )
			setRect( r );
	}
	else if( handle == d->m_handles->m_topRight.get() )
	{
		const QRectF r = d->m_rect.adjusted( 0.0, delta.y(), delta.x(), 0.0 );

		if( d->m_handles->checkConstraint( r.size() ) )
			setRect( r );
	}
	else if( handle == d->m_handles->m_right.get() )
	{
		const QRectF r = d->m_rect.adjusted( 0.0, 0.0, delta.x(), 0.0 );

		if( d->m_handles->checkConstraint( r.size() ) )
			setRect( r );
	}
	else if( handle == d->m_handles->m_bottomRight.get() )
	{
		const QRectF r = d->m_rect.adjusted( 0.0, 0.0, delta.x(), delta.y() );

		if( d->m_handles->checkConstraint( r.size() ) )
			setRect( r );
	}
	else if( handle == d->m_handles->m_bottom.get() )
	{
		const QRectF r = d->m_rect.adjusted( 0.0, 0.0, 0.0, delta.y() );

		if( d->m_handles->checkConstraint( r.size() ) )
			setRect( r );
	}
	else if( handle == d->m_handles->m_bottomLeft.get() )
	{
		const QRectF r = d->m_rect.adjusted( delta.x(), 0.0, 0.0, delta.y() );

		if( d->m_handles->checkConstraint( r.size() ) )
			setRect( r );
	}
	else if( handle == d->m_handles->m_left.get() )
	{
		const QRectF r = d->m_rect.adjusted( delta.x(), 0.0, 0.0, 0.0 );

		if( d->m_handles->checkConstraint( r.size() ) )
			setRect( r );
	}
}

void
FormResizableProxy::handleReleased( FormMoveHandle * handle )
{
	d->m_handleMoved = false;

	if( handle != d->m_handles->m_move.get() )
	{
		d->m_object->resize( d->m_rect );

		auto * obj = dynamic_cast< FormObject* > ( d->m_object );

		if( obj )
		{
			d->m_form->undoStack()->push( new UndoResize(
				obj->page(), obj->objectId(),
				d->m_subsidiaryRect, d->m_rect ) );

			obj->page()->emitChanged();

			obj->updatePropertiesValues();
		}
	}
	else
	{
		auto * obj = dynamic_cast< FormObject* > ( d->m_object );

		if( obj )
		{
			d->m_form->undoStack()->push( new UndoMove(
				obj->page(), obj->objectId(),
				d->m_rect.topLeft() - d->m_subsidiaryRect.topLeft() ) );

			obj->page()->emitChanged();

			obj->updatePropertiesValues();
		}
	}
}

} /* namespace Core */

} /* namespace Prototyper */
