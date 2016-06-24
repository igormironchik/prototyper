
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
#include "form_rectangle.hpp"
#include "form_with_resize_and_move_handles.hpp"
#include "form_actions.hpp"
#include "form_resize_handle.hpp"
#include "utils.hpp"
#include "form.hpp"
#include "form_undo_commands.hpp"

// Qt include.
#include <QUndoStack>


namespace Prototyper {

namespace Core {

//
// FormRectPrivate
//

class FormRectPrivate {
public:
	FormRectPrivate( FormRect * parent )
		:	q( parent )
		,	m_handles( 0 )
		,	m_isHandleMoved( false )
	{
	}

	//! Init.
	void init();
	//! Update rect.
	void updateRect( const QRectF & r );

	//! Parent.
	FormRect * q;
	//! Resize & move handles.
	QScopedPointer< WithResizeAndMoveHandles > m_handles;
	//! Sunsidiary rect.
	QRectF m_subsidiaryRect;
	//! Is handle moved?
	bool m_isHandleMoved;
}; // class FormRectPrivate

void
FormRectPrivate::init()
{
	m_handles.reset( new WithResizeAndMoveHandles( q, q->parentItem(),
		q->form() ) );

	m_handles->setDeltaToZero( 1.0 );

	m_handles->hide();

	q->setObjectPen( QPen( FormAction::instance()->strokeColor(), 2.0 ) );

	q->setObjectBrush( Qt::transparent );
}

void
FormRectPrivate::updateRect( const QRectF & r )
{
	q->setRect( r );

	QRectF hr = q->boundingRect();

	hr.moveTopLeft( q->pos() + hr.topLeft() );
	hr.adjust( -12.0, -12.0, 12.0, 12.0 );

	m_handles->place( hr );

	q->update();
}


//
// FormRect
//

FormRect::FormRect( Form * form, QGraphicsItem * parent )
	:	QGraphicsRectItem( parent )
	,	FormObject( FormObject::RectType, form )
	,	d( new FormRectPrivate( this ) )
{
	d->init();
}

FormRect::~FormRect()
{
}

Cfg::Rect
FormRect::cfg() const
{
	Cfg::Rect c;

	c.setObjectId( objectId() );

	Cfg::Point p;
	p.setX( pos().x() );
	p.setY( pos().y() );

	c.setPos( p );

	c.setPen( Cfg::pen( objectPen() ) );

	c.setBrush( Cfg::brush( objectBrush() ) );

	c.setLink( link() );

	Cfg::Point topLeft;
	topLeft.setX( rect().topLeft().x() );
	topLeft.setY( rect().topLeft().y() );

	c.setTopLeft( topLeft );

	Cfg::Size s;
	s.setWidth( rect().size().width() );
	s.setHeight( rect().size().height() );

	c.setSize( s );

	return c;
}

void
FormRect::setCfg( const Cfg::Rect & c )
{
	setObjectId( c.objectId() );

	setObjectPen( Cfg::fromPen( c.pen() ) );

	setObjectBrush( Cfg::fromBrush( c.brush() ) );

	const QRectF r( c.topLeft().x(), c.topLeft().y(),
		c.size().width(), c.size().height() );

	setPos( QPointF( c.pos().x(), c.pos().y() ) );

	setRectangle( r );

	setLink( c.link() );
}

QRectF
FormRect::boundingRect() const
{
	return QGraphicsRectItem::boundingRect();
}

void
FormRect::paint( QPainter * painter, const QStyleOptionGraphicsItem * option,
	QWidget * widget )
{
	QGraphicsRectItem::paint( painter, option, widget );

	if( isSelected() && !group() )
		d->m_handles->show();
	else
		d->m_handles->hide();
}

void
FormRect::setObjectPen( const QPen & p )
{
	FormObject::setObjectPen( p );

	setPen( p );
}

void
FormRect::setObjectBrush( const QBrush & b )
{
	setBrush( b );

	FormObject::setObjectBrush( b );
}

void
FormRect::setPosition( const QPointF & pos )
{
	form()->undoStack()->push( new UndoMove< FormRect > ( form(),
		objectId(), pos - position() ) );

	setPos( pos - rect().topLeft() );

	d->updateRect( rect() );
}

QPointF
FormRect::position() const
{
	return pos() + rect().topLeft();
}

void
FormRect::setRectangle( const QRectF & r )
{
	if( d->m_handles->checkConstraint( r.size() ) )
		d->updateRect( r );
}

QRectF
FormRect::rectangle() const
{
	QRectF r = boundingRect();
	r.moveTopLeft( position() );

	return r;
}

void
FormRect::handleMoved( const QPointF & delta, FormMoveHandle * handle )
{
	if( !d->m_isHandleMoved )
	{
		d->m_subsidiaryRect = rect();

		d->m_isHandleMoved = true;
	}

	if( handle == d->m_handles->m_move.data() )
	{
		moveBy( delta.x(), delta.y() );

		d->updateRect( rect() );
	}
	else if( handle == d->m_handles->m_topLeft.data() )
	{
		const QRectF r =
			rect().adjusted( delta.x(), delta.y(), 0.0, 0.0 );

		if( d->m_handles->checkConstraint( r.size() ) )
			d->updateRect( r );
	}
	else if( handle == d->m_handles->m_top.data() )
	{
		const QRectF r =
			rect().adjusted( 0.0, delta.y(), 0.0, 0.0 );

		if( d->m_handles->checkConstraint( r.size() ) )
			d->updateRect( r );
	}
	else if( handle == d->m_handles->m_topRight.data() )
	{
		const QRectF r =
			rect().adjusted( 0.0, delta.y(), delta.x(), 0.0 );

		if( d->m_handles->checkConstraint( r.size() ) )
			d->updateRect( r );
	}
	else if( handle == d->m_handles->m_right.data() )
	{
		const QRectF r =
			rect().adjusted( 0.0, 0.0, delta.x(), 0.0 );

		if( d->m_handles->checkConstraint( r.size() ) )
			d->updateRect( r );
	}
	else if( handle == d->m_handles->m_bottomRight.data() )
	{
		const QRectF r =
			rect().adjusted( 0.0, 0.0, delta.x(), delta.y() );

		if( d->m_handles->checkConstraint( r.size() ) )
			d->updateRect( r );
	}
	else if( handle == d->m_handles->m_bottom.data() )
	{
		const QRectF r =
			rect().adjusted( 0.0, 0.0, 0.0, delta.y() );

		if( d->m_handles->checkConstraint( r.size() ) )
			d->updateRect( r );
	}
	else if( handle == d->m_handles->m_bottomLeft.data() )
	{
		const QRectF r =
			rect().adjusted( delta.x(), 0.0, 0.0, delta.y() );

		if( d->m_handles->checkConstraint( r.size() ) )
			d->updateRect( r );
	}
	else if( handle == d->m_handles->m_left.data() )
	{
		const QRectF r =
			rect().adjusted( delta.x(), 0.0, 0.0, 0.0 );

		if( d->m_handles->checkConstraint( r.size() ) )
			d->updateRect( r );
	}

	form()->update();
}

void
FormRect::handleReleased( FormMoveHandle * handle )
{
	d->m_isHandleMoved = false;

	if( handle == d->m_handles->m_move.data() )
		form()->undoStack()->push( new UndoMove< FormRect > ( form(),
			objectId(), rect().topLeft() - d->m_subsidiaryRect.topLeft() ) );
	else
		form()->undoStack()->push( new UndoResize< FormRect > ( form(),
			objectId(), d->m_subsidiaryRect, rect() ) );
}

} /* namespace Core */

} /* namespace Prototyper */
