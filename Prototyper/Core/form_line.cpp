
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
#include "form_move_handle.hpp"
#include "form_actions.hpp"
#include "form.hpp"
#include "grid_snap.hpp"

// Qt include.
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOptionGraphicsItem>


namespace Prototyper {

namespace Core {

//
// FormLinePrivate
//

class FormLinePrivate {
public:
	FormLinePrivate( FormLine * parent )
		:	q( parent )
		,	m_h1( 0 )
		,	m_h2( 0 )
		,	m_move( 0 )
		,	m_showHandles( false )
	{
	}

	//! Init.
	void init();

	//! Parent.
	FormLine * q;
	//! First handle.
	FormMoveHandle * m_h1;
	//! Second handle.
	FormMoveHandle * m_h2;
	//! Move handler.
	FormMoveHandle * m_move;
	//! Show handles?
	bool m_showHandles;
}; // class FormLinePrivate

void
FormLinePrivate::init()
{	
	m_h1 = new FormMoveHandle( q, q );
	m_h1->hide();

	m_h2 = new FormMoveHandle( q, q );
	m_h2->hide();

	m_move = new FormMoveHandle( q, q );
	m_move->hide();

	q->setObjectPen( QPen( FormAction::instance()->strokeColor(), 2.0 ) );
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
	return QGraphicsLineItem::boundingRect().adjusted(
		-d->m_h1->halfOfSize(), -d->m_h1->halfOfSize(),
		d->m_h2->halfOfSize(), d->m_h2->halfOfSize() );
}

void
FormLine::paint( QPainter * painter, const QStyleOptionGraphicsItem * option,
	QWidget * widget )
{
	Q_UNUSED( widget )

	if( ( isSelected() || d->m_showHandles ) && !group() )
	{
		const QLineF l = line();

		d->m_h1->setPos( l.p1().x() - d->m_h1->halfOfSize(),
			l.p1().y() - d->m_h1->halfOfSize() );
		d->m_h1->show();

		d->m_h2->setPos( l.p2().x() - d->m_h2->halfOfSize(),
			l.p2().y() - d->m_h2->halfOfSize() );
		d->m_h2->show();

		d->m_move->setPos(
			( l.p1().x() + l.p2().x() ) / 2.0 - d->m_move->halfOfSize(),
			( l.p1().y() + l.p2().y() ) / 2.0 - d->m_move->halfOfSize() );
		d->m_move->show();
	}
	else
	{
		d->m_h1->hide();
		d->m_h1->clear();
		d->m_h2->hide();
		d->m_h2->clear();
		d->m_move->hide();
		d->m_move->clear();
	}

	QGraphicsLineItem::paint( painter, option, widget );
}

void
FormLine::setObjectPen( const QPen & p )
{
	FormObject::setObjectPen( p );

	setPen( p );
}

void
FormLine::showHandles( bool show )
{
	d->m_showHandles = show;

	d->m_h1->ignoreMouseEvents( show );
	d->m_h2->ignoreMouseEvents( show );
	d->m_move->ignoreMouseEvents( show );

	update();
}

QPointF
FormLine::pointUnderHandle( const QPointF & point, bool & intersected,
	bool & intersectedEnds ) const
{
	if( d->m_h1->contains( d->m_h1->mapFromScene( point ) ) )
	{
		intersected = true;

		intersectedEnds = true;

		return line().p1();
	}
	else if( d->m_h2->contains( d->m_h2->mapFromScene( point ) ) )
	{
		intersected = true;

		intersectedEnds = true;

		return line().p2();
	}
	else if( d->m_move->contains( d->m_move->mapFromScene( point ) ) )
	{
		const QLineF l = line();

		intersected = true;

		return QPointF( ( l.p1().x() + l.p2().x() ) / 2.0,
			( l.p1().y() + l.p2().y() ) / 2.0 );
	}
	else
	{
		intersected = false;

		return point;
	}
}

bool
FormLine::handleMouseMoveInHandles( const QPointF & point )
{
	if( d->m_h1->handleMouseMove( point ) )
		return true;
	else if( d->m_h2->handleMouseMove( point ) )
		return true;
	else if( d->m_move->handleMouseMove( point ) )
		return true;
	else
		return false;
}

void
FormLine::handleMoved( const QPointF & delta, FormMoveHandle * handle )
{
	if( isSelected() )
	{
		const QLineF l = line();

		if( handle == d->m_h1 )
			setLine( QLineF( l.p1() + delta, l.p2() ) );
		else if( handle == d->m_h2 )
			setLine( QLineF( l.p1(), l.p2() + delta ) );
		else if( handle == d->m_move )
			moveBy( delta.x(), delta.y() );
	}
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
