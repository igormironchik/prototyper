
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
#include "form_polyline.hpp"
#include "form_move_handle.hpp"
#include "form_actions.hpp"
#include "form_resize_handle.hpp"

// Qt include.
#include <QStyleOptionGraphicsItem>
#include <QPolygonF>

// C++ include.
#include <algorithm>


namespace Prototyper {

namespace Core {

//
// FormPolylinePrivate
//

class FormPolylinePrivate {
public:
	FormPolylinePrivate( FormPolyline * parent )
		:	q( parent )
		,	m_start( 0 )
		,	m_end( 0 )
		,	m_closed( false )
		,	m_topLeft( 0 )
		,	m_top( 0 )
		,	m_topRight( 0 )
		,	m_left( 0 )
		,	m_bottomLeft( 0 )
		,	m_bottom( 0 )
		,	m_bottomRight( 0 )
		,	m_right( 0 )
		,	m_move( 0 )
	{
	}

	//! Init.
	void init();
	//! Make path.
	void makePath();
	//! Update lines.
	void updateLines( const QRectF & oldR, const QRectF & newR );
	//! \return Bounding rect.
	QRectF boundingRect() const;

	//! Parent.
	FormPolyline * q;
	//! Lines.
	QList< QLineF > m_lines;
	//! Polygon.
	QPolygonF m_polygon;
	//! Start handle.
	FormMoveHandle * m_start;
	//! End handle.
	FormMoveHandle * m_end;
	//! Closed?
	bool m_closed;
	//! Top-left resize.
	FormResizeHandle * m_topLeft;
	//! Top resize.
	FormResizeHandle * m_top;
	//! Top-right resize.
	FormResizeHandle * m_topRight;
	//! Left resize.
	FormResizeHandle * m_left;
	//! Left-bottom resize.
	FormResizeHandle * m_bottomLeft;
	//! Bottom resize.
	FormResizeHandle * m_bottom;
	//! Right-bottom resize.
	FormResizeHandle * m_bottomRight;
	//! Right resize.
	FormResizeHandle * m_right;
	//! Move handle.
	FormMoveHandle * m_move;
}; // class FormPolylinePrivate

void
FormPolylinePrivate::init()
{
	m_start = new FormMoveHandle( 3.0, QPointF( 3.0, 3.0 ), q, q );
	m_start->ignoreMouseEvents( true );

	m_end = new FormMoveHandle( 3.0, QPointF( 3.0, 3.0 ), q, q );
	m_end->ignoreMouseEvents( true );

	m_start->hide();
	m_end->hide();

	m_topLeft = new FormResizeHandle( 6.0, QPointF( 12.0, 12.0 ), -45.0, q, q );
	m_top = new FormResizeHandle( 6.0, QPointF( 6.0, 12.0 ), 0.0, q, q );
	m_topRight = new FormResizeHandle( 6.0, QPointF( 0.0, 12.0 ), 45.0, q, q );
	m_left = new FormResizeHandle( 6.0, QPointF( 12.0, 6.0 ), -90.0, q, q );
	m_bottomLeft = new FormResizeHandle( 6.0, QPointF( 12.0, 0.0 ), 45.0, q, q );
	m_bottom = new FormResizeHandle( 6.0, QPointF( 6.0, 0.0 ), 0.0, q, q );
	m_bottomRight = new FormResizeHandle( 6.0, QPointF( 0.0, 0.0 ), -45.0, q, q );
	m_right = new FormResizeHandle( 6.0, QPointF( 0.0, 6.0 ), 90, q, q );
	m_move = new FormMoveHandle( 3.0, QPointF( 3.0, 3.0 ), q, q );

	m_topLeft->hide();
	m_top->hide();
	m_topRight->hide();
	m_left->hide();
	m_bottomLeft->hide();
	m_bottom->hide();
	m_bottomRight->hide();
	m_right->hide();
	m_move->hide();

	q->setObjectPen( QPen( FormAction::instance()->strokeColor(), 2.0 ) );

	q->setObjectBrush( Qt::transparent );
}

void
FormPolylinePrivate::makePath()
{
	QPainterPath path;
	QPointF first;

	QVector< QPointF > points;
	points.reserve( m_lines.size() * 2 );

	for( int i = 0; i < m_lines.size(); ++i )
	{
		const QLineF line = m_lines.at( i );

		points.append( line.p1() );
		points.append( line.p2() );

		if( i == 0 )
		{
			path.moveTo( line.p1() );

			first = line.p1();
		}

		path.lineTo( line.p2() );

		if( i + 1 == m_lines.size() && line.p2() == first )
			m_closed = true;
	}

	m_polygon = QPolygonF( points );

	q->setPath( path );

	if( m_closed )
	{
		q->setBrush( q->objectBrush() );

		q->showHandles( false );
	}
}

void
FormPolylinePrivate::updateLines( const QRectF & oldR, const QRectF & newR )
{
	const qreal mx = oldR.width() / newR.width();
	const qreal my = oldR.height() / newR.height();

	std::for_each( m_lines.begin(), m_lines.end(),
		[ &oldR, &newR, mx, my ] ( QLineF & line )
		{
			line.setP1( QPointF( ( line.p1().x() - oldR.x() ) / mx + newR.x(),
				( line.p1().y() - oldR.y() ) / my + newR.y() ) );
			line.setP2( QPointF( ( line.p2().x() - oldR.x() ) / mx + newR.x(),
				( line.p2().y() - oldR.y() ) / my + newR.y() ) );
		}
	);

	makePath();
}

QRectF
FormPolylinePrivate::boundingRect() const
{
	return m_polygon.boundingRect();
}


//
// FormPolyline
//

FormPolyline::FormPolyline( QGraphicsItem * parent )
	:	QGraphicsPathItem( parent )
	,	d( new FormPolylinePrivate( this ) )
{
	d->init();
}

FormPolyline::~FormPolyline()
{
}

const QList< QLineF > &
FormPolyline::lines() const
{
	return d->m_lines;
}

void
FormPolyline::setLines( const QList< QLineF > & lns )
{
	d->m_closed = false;

	setBrush( Qt::transparent );

	foreach( const QLineF & line, lns )
		appendLine( line );
}

void
FormPolyline::appendLine( const QLineF & line )
{
	if( d->m_lines.isEmpty() )
		d->m_start->setPos( line.p1() -
			QPointF( d->m_start->halfOfSize(), d->m_start->halfOfSize() ) );

	if( d->m_lines.isEmpty() || d->m_lines.last().p2() == line.p1() )
	{
		d->m_lines.append( line );

		d->makePath();

		d->m_end->setPos( line.p2() -
			QPointF( d->m_end->halfOfSize(), d->m_end->halfOfSize() ) );
	}
	else
	{
		d->m_lines.prepend( QLineF( line.p2(), line.p1() ) );

		d->makePath();

		d->m_start->setPos( line.p2() -
			QPointF( d->m_start->halfOfSize(), d->m_start->halfOfSize() ) );
	}
}

void
FormPolyline::showHandles( bool show )
{
	if( show )
	{
		d->m_start->show();
		d->m_end->show();
	}
	else
	{
		d->m_start->hide();
		d->m_end->hide();
	}
}

bool
FormPolyline::isClosed() const
{
	return d->m_closed;
}

QPointF
FormPolyline::pointUnderHandle( const QPointF & p, bool & intersected ) const
{
	if( d->m_start->contains( d->m_start->mapFromScene( p ) ) )
	{
		intersected = true;

		return d->m_start->pos() +
			QPointF( d->m_start->halfOfSize(), d->m_start->halfOfSize() );
	}
	else if( d->m_end->contains( d->m_end->mapFromScene( p ) ) )
	{
		intersected = true;

		return d->m_end->pos() +
			QPointF( d->m_end->halfOfSize(), d->m_end->halfOfSize() );
	}
	else
	{
		intersected = false;

		return p;
	}
}

void
FormPolyline::setObjectPen( const QPen & p )
{
	FormObject::setObjectPen( p );

	setPen( p );
}

void
FormPolyline::setObjectBrush( const QBrush & b )
{
	if( d->m_closed )
		setBrush( b );

	FormObject::setObjectBrush( b );
}

QRectF
FormPolyline::boundingRect() const
{
	return QGraphicsPathItem::boundingRect().adjusted(
		-d->m_topLeft->halfOfSize() * 2.0,
		-d->m_topLeft->halfOfSize() * 2.0,
		d->m_bottomRight->halfOfSize() * 2.0,
		d->m_bottomRight->halfOfSize() * 2.0 );
}

void
FormPolyline::handleMouseMoveInHandles( const QPointF & p )
{
	if( d->m_start->handleMouseMove( p ) )
		return;
	else
		d->m_end->handleMouseMove( p );
}

void
FormPolyline::paint( QPainter * painter, const QStyleOptionGraphicsItem * option,
	QWidget * widget )
{
	QGraphicsPathItem::paint( painter, option, widget );

	if( isSelected() && !group() )
	{
		d->m_topLeft->setPos( option->rect.x(), option->rect.y() );
		d->m_top->setPos( option->rect.x() + option->rect.width() / 2.0 -
			d->m_top->halfOfSize(), option->rect.y() );
		d->m_topRight->setPos( option->rect.x() + option->rect.width() -
			d->m_topRight->halfOfSize() * 2.0, option->rect.y() );
		d->m_left->setPos( option->rect.x(),
			option->rect.y() + option->rect.height() / 2.0 -
				d->m_left->halfOfSize() );
		d->m_bottomLeft->setPos( option->rect.x(),
			option->rect.y() + option->rect.height() -
				d->m_bottomLeft->halfOfSize() * 2.0 );
		d->m_bottom->setPos( option->rect.x() + option->rect.width() / 2.0 -
				d->m_bottom->halfOfSize(),
			option->rect.y() + option->rect.height() -
				d->m_bottom->halfOfSize() * 2.0 );
		d->m_bottomRight->setPos( option->rect.x() + option->rect.width() -
				d->m_bottomRight->halfOfSize() * 2.0,
			option->rect.y() + option->rect.height() -
				d->m_bottomRight->halfOfSize() * 2.0 );
		d->m_right->setPos( option->rect.x() + option->rect.width() -
				d->m_right->halfOfSize() * 2.0,
			option->rect.y() + option->rect.height() / 2.0 -
				d->m_right->halfOfSize() );
		d->m_move->setPos( option->rect.x() + option->rect.width() / 2.0 -
				d->m_move->halfOfSize(),
			option->rect.y() + option->rect.height() / 2.0 -
				d->m_move->halfOfSize() );

		d->m_topLeft->show();
		d->m_top->show();
		d->m_topRight->show();
		d->m_left->show();
		d->m_bottomLeft->show();
		d->m_bottom->show();
		d->m_bottomRight->show();
		d->m_right->show();
		d->m_move->show();
	}
	else
	{
		d->m_topLeft->hide();
		d->m_top->hide();
		d->m_topRight->hide();
		d->m_left->hide();
		d->m_bottomLeft->hide();
		d->m_bottom->hide();
		d->m_bottomRight->hide();
		d->m_right->hide();
		d->m_move->hide();
	}
}

void
FormPolyline::handleMoved( const QPointF & delta, FormMoveHandle * handle )
{
	if( handle == d->m_move )
		moveBy( delta.x(), delta.y() );
	else if( handle == d->m_topLeft )
		d->updateLines( d->boundingRect(),
			d->boundingRect().adjusted( delta.x(), delta.y(), 0.0, 0.0 ) );
	else if( handle == d->m_top )
		d->updateLines( d->boundingRect(),
			d->boundingRect().adjusted( 0.0, delta.y(), 0.0, 0.0 ) );
	else if( handle == d->m_topRight )
		d->updateLines( d->boundingRect(),
			d->boundingRect().adjusted( 0.0, delta.y(), delta.x(), 0.0 ) );
	else if( handle == d->m_right )
		d->updateLines( d->boundingRect(),
			d->boundingRect().adjusted( 0.0, 0.0, delta.x(), 0.0 ) );
	else if( handle == d->m_bottomRight )
		d->updateLines( d->boundingRect(),
			d->boundingRect().adjusted( 0.0, 0.0, delta.x(), delta.y() ) );
	else if( handle == d->m_bottom )
		d->updateLines( d->boundingRect(),
			d->boundingRect().adjusted( 0.0, 0.0, 0.0, delta.y() ) );
	else if( handle == d->m_bottomLeft )
		d->updateLines( d->boundingRect(),
			d->boundingRect().adjusted( delta.x(), 0.0, 0.0, delta.y() ) );
	else if( handle == d->m_left )
		d->updateLines( d->boundingRect(),
			d->boundingRect().adjusted( delta.x(), 0.0, 0.0, 0.0 ) );
}

} /* namespace Core */

} /* namespace Prototyper */
