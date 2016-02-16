
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
#include "form_grid_snap.hpp"
#include "utils.hpp"

// Qt include.
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QApplication>
#include <QGraphicsScene>


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
	//! Place child.
	void placeChild();
	//! Create handles.
	void createHandles();

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
	createHandles();

	m_h1->hide();
	m_h2->hide();
	m_move->hide();

	q->setObjectPen( QPen( FormAction::instance()->strokeColor(), 2.0 ) );
}

void
FormLinePrivate::placeChild()
{
	const QLineF l = q->line();

	m_h1->setPos( l.p1().x() - m_h1->halfOfSize(),
		l.p1().y() - m_h1->halfOfSize() );

	m_h2->setPos( l.p2().x() - m_h2->halfOfSize(),
		l.p2().y() - m_h2->halfOfSize() );

	m_move->setPos(
		( l.p1().x() + l.p2().x() ) / 2.0 - m_move->halfOfSize(),
		( l.p1().y() + l.p2().y() ) / 2.0 - m_move->halfOfSize() );
}

void
FormLinePrivate::createHandles()
{
	m_h1 = new FormMoveHandle( 3.0, QPointF( 3.0, 3.0 ), q, q,
		Qt::CrossCursor );

	m_h2 = new FormMoveHandle( 3.0, QPointF( 3.0, 3.0 ),q, q,
		Qt::CrossCursor );

	m_move = new FormMoveHandle( 3.0, QPointF( 3.0, 3.0 ), q, q,
		Qt::SizeAllCursor );
}


//
// FormLine
//

FormLine::FormLine( Form * form, QGraphicsItem * parent )
	:	QGraphicsLineItem( parent )
	,	FormObject( FormObject::LineType, form )
	,	d( new FormLinePrivate( this ) )
{
	d->init();
}

FormLine::~FormLine()
{
}

Cfg::Line
FormLine::cfg() const
{
	const QLineF l = line();

	Cfg::Line c;

	Cfg::Point p1;
	p1.setX( l.p1().x() );
	p1.setY( l.p1().y() );

	Cfg::Point p2;
	p2.setX( l.p2().x() );
	p2.setY( l.p2().y() );

	c.p1() = p1;
	c.p2() = p2;

	Cfg::Point p;
	p.setX( pos().x() );
	p.setY( pos().y() );

	c.pos() = p;

	c.setObjectId( objectId() );

	c.setPen( Cfg::pen( objectPen() ) );

	return c;
}

void
FormLine::setCfg( const Cfg::Line & c )
{
	QLineF line;
	line.setP1( QPointF( c.p1().x(), c.p1().y() ) );
	line.setP2( QPointF( c.p2().x(), c.p2().y() ) );

	setLine( line );

	d->placeChild();

	setPos( QPointF( c.pos().x(), c.pos().y() ) );

	setObjectId( c.objectId() );

	setObjectPen( Cfg::fromPen( c.pen() ) );
}

QRectF
FormLine::boundingRect() const
{
	return QGraphicsLineItem::boundingRect()
		.adjusted( -3.0, -3.0, 3.0, 3.0 );
}

void
FormLine::paint( QPainter * painter, const QStyleOptionGraphicsItem * option,
	QWidget * widget )
{
	Q_UNUSED( widget )

	if( ( isSelected() || d->m_showHandles ) && !group() )
	{
		if( d->m_h1 )
		{
			d->placeChild();

			if( !d->m_showHandles )
				d->m_h1->setCursor( d->m_h1->handleCursor() );

			d->m_h1->show();

			if( !d->m_showHandles )
				d->m_h2->setCursor( d->m_h2->handleCursor() );

			d->m_h2->show();

			if( !d->m_showHandles )
				d->m_move->setCursor( d->m_move->handleCursor() );

			d->m_move->show();
		}
	}
	else
	{
		if( d->m_h1 )
		{
			d->m_h1->hide();
			d->m_h1->clear();
			d->m_h1->unsetCursor();
			d->m_h2->hide();
			d->m_h2->clear();
			d->m_h2->unsetCursor();
			d->m_move->hide();
			d->m_move->clear();
			d->m_h2->unsetCursor();
		}
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
	d->m_h1->unsetCursor();
	d->m_h2->unsetCursor();
	d->m_move->unsetCursor();

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
