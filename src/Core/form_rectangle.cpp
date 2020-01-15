
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
#include "form_rectangle.hpp"
#include "form_with_resize_and_move_handles.hpp"
#include "form_actions.hpp"
#include "form_resize_handle.hpp"
#include "utils.hpp"
#include "page.hpp"
#include "form_undo_commands.hpp"
#include "constants.hpp"

// Qt include.
#include <QUndoStack>
#include <QPainter>
#include <QGraphicsScene>


namespace Prototyper {

namespace Core {

//
// FormRectPrivate
//

class FormRectPrivate {
public:
	explicit FormRectPrivate( FormRect * parent )
		:	q( parent )
		,	m_handles( nullptr )
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
	//! Rect.
	QRectF m_rect;
}; // class FormRectPrivate

void
FormRectPrivate::init()
{
	m_handles.reset( new WithResizeAndMoveHandles( q, q->parentItem(),
		q->page() ) );

	m_handles->setDeltaToZero( 1.0 );

	m_handles->hide();

	q->setObjectPen( QPen( PageAction::instance()->strokeColor(), c_linePenWidth ),
		false );

	q->setObjectBrush( Qt::transparent );
}

void
FormRectPrivate::updateRect( const QRectF & r )
{
	m_rect = r;

	m_rect.moveTopLeft( QPointF( 0.0, 0.0 ) );

	QRectF hr = q->boundingRect();

	hr.moveTopLeft( q->pos() + hr.topLeft() );
	hr.adjust(
		-c_halfHandleSize * c_halfDivider * c_halfDivider,
		-c_halfHandleSize * c_halfDivider * c_halfDivider,
		c_halfHandleSize * c_halfDivider * c_halfDivider,
		c_halfHandleSize * c_halfDivider * c_halfDivider );

	m_handles->place( hr );

	q->update();
}


//
// FormRect
//

FormRect::FormRect( Page * page, QGraphicsItem * parent )
	:	QGraphicsItem( parent )
	,	FormObject( FormObject::RectType, page )
	,	d( new FormRectPrivate( this ) )
{
	d->init();
}

FormRect::~FormRect() = default;

Cfg::Rect
FormRect::cfg() const
{
	Cfg::Rect c;

	c.set_objectId( objectId() );

	Cfg::Point p;
	p.set_x( MmPx::instance().toMmX( pos().x() ) );
	p.set_y( MmPx::instance().toMmY( pos().y() ) );

	c.set_pos( p );

	c.set_pen( Cfg::pen( objectPen() ) );

	c.set_brush( Cfg::brush( objectBrush() ) );

	Cfg::Point topLeft;
	topLeft.set_x( MmPx::instance().toMmX( pos().x() ) );
	topLeft.set_y( MmPx::instance().toMmY( pos().y() ) );

	c.set_topLeft( topLeft );

	Cfg::Size s;
	s.set_width( MmPx::instance().toMmX( d->m_rect.size().width() ) );
	s.set_height( MmPx::instance().toMmY( d->m_rect.size().height() ) );

	c.set_size( s );

	c.set_z( zValue() );

	return c;
}

void
FormRect::setCfg( const Cfg::Rect & c )
{
	setObjectId( c.objectId() );

	setObjectPen( Cfg::fromPen( c.pen() ), false );

	setObjectBrush( Cfg::fromBrush( c.brush() ) );

	const QRectF r( MmPx::instance().fromMmX( c.topLeft().x() ),
		MmPx::instance().fromMmY( c.topLeft().y() ),
		MmPx::instance().fromMmX( c.size().width() ),
		MmPx::instance().fromMmY( c.size().height() ) );

	setPos( QPointF( MmPx::instance().fromMmX( c.pos().x() ),
		MmPx::instance().fromMmY( c.pos().y() ) ) );

	setZValue( c.z() );

	d->updateRect( r );
}

QRectF
FormRect::boundingRect() const
{
	if( !d.isNull() )
	{
		const qreal w = objectPen().widthF() / 2.0;

		return d->m_rect.adjusted( -w, -w, w, w );
	}

	return {};
}

void
FormRect::paint( QPainter * painter, const QStyleOptionGraphicsItem * option,
	QWidget * widget )
{
	Q_UNUSED( option )
	Q_UNUSED( widget )

	painter->setPen( objectPen() );

	painter->drawRect( d->m_rect );

	if( isSelected() && !group() )
		d->m_handles->show();
	else
		d->m_handles->hide();
}

void
FormRect::setPosition( const QPointF & p, bool pushUndoCommand )
{
	FormObject::setPosition( p, pushUndoCommand );

	setPos( p );

	d->updateRect( d->m_rect );
}

QPointF
FormRect::position() const
{
	return pos();
}

void
FormRect::setRectangle( const QRectF & r, bool pushUndoCommand )
{
	FormObject::setRectangle( r, pushUndoCommand );

	setPos( r.topLeft() );

	d->updateRect( r );

	scene()->update();
}

QRectF
FormRect::rectangle() const
{
	QRectF r = d->m_rect;
	r.moveTopLeft( pos() );

	return r;
}

void
FormRect::setObjectPen( const QPen & p, bool pushUndoCommand )
{
	QPen newPen = p;
	newPen.setWidth( 2 );

	FormObject::setObjectPen( newPen, pushUndoCommand );

	update();
}

void
FormRect::handleMoved( const QPointF & delta, FormMoveHandle * handle )
{
	if( !d->m_isHandleMoved )
	{
		d->m_subsidiaryRect = rectangle();

		d->m_isHandleMoved = true;
	}

	if( handle == d->m_handles->m_move.data() )
	{
		moveBy( delta.x(), delta.y() );

		d->updateRect( d->m_rect );
	}
	else if( handle == d->m_handles->m_topLeft.data() )
	{
		const QRectF r =
			d->m_rect.adjusted( delta.x(), delta.y(), 0.0, 0.0 );

		if( d->m_handles->checkConstraint( r.size() ) )
			d->updateRect( r );
	}
	else if( handle == d->m_handles->m_top.data() )
	{
		const QRectF r =
			d->m_rect.adjusted( 0.0, delta.y(), 0.0, 0.0 );

		if( d->m_handles->checkConstraint( r.size() ) )
			d->updateRect( r );
	}
	else if( handle == d->m_handles->m_topRight.data() )
	{
		const QRectF r =
			d->m_rect.adjusted( 0.0, delta.y(), delta.x(), 0.0 );

		if( d->m_handles->checkConstraint( r.size() ) )
			d->updateRect( r );
	}
	else if( handle == d->m_handles->m_right.data() )
	{
		const QRectF r =
			d->m_rect.adjusted( 0.0, 0.0, delta.x(), 0.0 );

		if( d->m_handles->checkConstraint( r.size() ) )
			d->updateRect( r );
	}
	else if( handle == d->m_handles->m_bottomRight.data() )
	{
		const QRectF r =
			d->m_rect.adjusted( 0.0, 0.0, delta.x(), delta.y() );

		if( d->m_handles->checkConstraint( r.size() ) )
			d->updateRect( r );
	}
	else if( handle == d->m_handles->m_bottom.data() )
	{
		const QRectF r =
			d->m_rect.adjusted( 0.0, 0.0, 0.0, delta.y() );

		if( d->m_handles->checkConstraint( r.size() ) )
			d->updateRect( r );
	}
	else if( handle == d->m_handles->m_bottomLeft.data() )
	{
		const QRectF r =
			d->m_rect.adjusted( delta.x(), 0.0, 0.0, delta.y() );

		if( d->m_handles->checkConstraint( r.size() ) )
			d->updateRect( r );
	}
	else if( handle == d->m_handles->m_left.data() )
	{
		const QRectF r =
			d->m_rect.adjusted( delta.x(), 0.0, 0.0, 0.0 );

		if( d->m_handles->checkConstraint( r.size() ) )
			d->updateRect( r );
	}

	page()->update();
}

void
FormRect::handleReleased( FormMoveHandle * handle )
{
	d->m_isHandleMoved = false;

	if( handle == d->m_handles->m_move.data() )
		page()->undoStack()->push( new UndoMove( page(),
			objectId(), position() - d->m_subsidiaryRect.topLeft() ) );
	else
	{
		QRectF r = rectangle();

		page()->undoStack()->push( new UndoResize( page(),
			objectId(), d->m_subsidiaryRect, r ) );
	}

	updatePropertiesValues();

	page()->emitChanged();
}

FormObject *
FormRect::clone() const
{
	auto * o = new FormRect( page(), parentItem() );

	o->setCfg( cfg() );

	o->setObjectId( page()->nextId() );

	return o;
}

} /* namespace Core */

} /* namespace Prototyper */
