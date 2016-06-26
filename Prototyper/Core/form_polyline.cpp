
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
#include "form_with_resize_and_move_handles.hpp"
#include "utils.hpp"
#include "form.hpp"
#include "form_undo_commands.hpp"

// Qt include.
#include <QStyleOptionGraphicsItem>
#include <QPolygonF>
#include <QApplication>
#include <QPainter>
#include <QUndoStack>


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
		,	m_handles( 0 )
		,	m_handleMoved( false )
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
	QScopedPointer< FormMoveHandle > m_start;
	//! End handle.
	QScopedPointer< FormMoveHandle > m_end;
	//! Closed?
	bool m_closed;
	//! Resize & move handles.
	QScopedPointer< WithResizeAndMoveHandles > m_handles;
	//! Resized bounding rect.
	QRectF m_resized;
	//! Sunsidiary rect.
	QRectF m_subsidiaryRect;
	//! Handle moved?
	bool m_handleMoved;
}; // class FormPolylinePrivate

void
FormPolylinePrivate::init()
{
	m_start.reset( new FormMoveHandle( 3.0, QPointF( 3.0, 3.0 ), q,
		q->parentItem(), q->form() ) );
	m_start->ignoreMouseEvents( true );
	m_start->unsetCursor();

	m_end.reset( new FormMoveHandle( 3.0, QPointF( 3.0, 3.0 ), q,
		q->parentItem(), q->form() ) );
	m_end->ignoreMouseEvents( true );
	m_end->unsetCursor();

	m_start->hide();
	m_end->hide();

	QScopedPointer< WithResizeAndMoveHandles > tmp(
		new WithResizeAndMoveHandles( q, q->parentItem(), q->form() ) );

	m_handles.swap( tmp );

	m_handles->setMinSize( QSizeF( 25.0, 25.0 ) );

	m_handles->setDeltaToZero( 1.0 );

	m_handles->hide();

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
		else
			m_closed = false;
	}

	m_polygon = QPolygonF( points );

	q->setPath( path );

	QRectF r = boundingRect();
	r.moveTopLeft( r.topLeft() + q->pos() );

	m_resized = r;

	if( m_closed )
	{
		q->setBrush( q->objectBrush() );

		q->showHandles( false );
	}
}

void
FormPolylinePrivate::updateLines( const QRectF & oldR, const QRectF & newR )
{
	Q_UNUSED( oldR )

	const QRectF b = boundingRect();

	m_resized = newR;

	const qreal w = (qreal) q->objectPen().width() / 2.0;

	m_handles->place( m_resized.adjusted( -12.0 - w, -12.0 - w,
		12.0 + w, 12.0 + w ) );

	const qreal mx = b.width() / m_resized.width();
	const qreal my = b.height() / m_resized.height();

	const QPointF p = q->pos();

	QPainterPath path;

	for( int i = 0; i < m_lines.size(); ++i )
	{
		const QLineF line = m_lines.at( i );

		if( i == 0 )
		{
			path.moveTo( QPointF(
				( line.p1().x() - b.x() ) / mx + m_resized.x() - p.x(),
				( line.p1().y() - b.y() ) / my + m_resized.y() - p.y() ) );
		}

		path.lineTo( QPointF(
			( line.p2().x() - b.x() ) / mx + m_resized.x() - p.x(),
			( line.p2().y() - b.y() ) / my + m_resized.y() - p.y() ) );
	}

	q->setPath( path );

	//q->setPos( 0.0, 0.0 );
}

QRectF
FormPolylinePrivate::boundingRect() const
{
	return m_polygon.boundingRect();
}


//
// FormPolyline
//

FormPolyline::FormPolyline( Form * form, QGraphicsItem * parent )
	:	QGraphicsPathItem( parent )
	,	FormObject( FormObject::PolylineType, form )
	,	d( new FormPolylinePrivate( this ) )
{
	d->init();
}

FormPolyline::~FormPolyline()
{
}

Cfg::Polyline
FormPolyline::cfg() const
{
	Cfg::Polyline c;

	c.setObjectId( objectId() );

	Cfg::Point p;
	p.setX( pos().x() );
	p.setY( pos().y() );

	c.setPos( p );

	foreach( const QLineF & l, d->m_lines )
	{
		Cfg::Point p1;
		p1.setX( l.p1().x() );
		p1.setY( l.p1().y() );

		Cfg::Point p2;
		p2.setX( l.p2().x() );
		p2.setY( l.p2().y() );

		Cfg::Line line;
		line.setP1( p1 );
		line.setP2( p2 );

		line.setPen( Cfg::pen( objectPen() ) );

		c.line().append( line );
	}

	c.setPen( Cfg::pen( objectPen() ) );

	c.setBrush( Cfg::brush( objectBrush() ) );

	c.setLink( link() );

	Cfg::Size s;
	s.setWidth( d->m_resized.width() );
	s.setHeight( d->m_resized.height() );

	c.setSize( s );

	return c;
}

void
FormPolyline::setCfg( const Cfg::Polyline & c )
{
	setObjectId( c.objectId() );

	d->m_lines.clear();

	setPos( QPointF( c.pos().x(), c.pos().y() ) );

	foreach( const Cfg::Line & l, c.line() )
	{
		QLineF line;
		line.setP1( QPointF( l.p1().x(), l.p1().y() ) );
		line.setP2( QPointF( l.p2().x(), l.p2().y() ) );

		appendLine( line );
	}

	setObjectPen( Cfg::fromPen( c.pen() ) );

	setObjectBrush( Cfg::fromBrush( c.brush() ) );

	const QRectF b = d->boundingRect();

	d->m_resized = QRectF( c.pos().x() + b.x(), c.pos().y() + b.y(),
		c.size().width(), c.size().height() );

	const qreal w = (qreal) objectPen().width() / 2.0;

	d->m_handles->place( d->m_resized.adjusted( -12.0 - w, -12.0 - w,
		12.0 + w, 12.0 + w ) );

	d->updateLines( QRectF(), d->m_resized );

	setLink( c.link() );
}

void
FormPolyline::draw( QPainter * painter, const Cfg::Polyline & cfg )
{
	QList< QLineF > lines;

	foreach( const Cfg::Line & l, cfg.line() )
	{
		QLineF line;
		line.setP1( QPointF( l.p1().x(), l.p1().y() ) );
		line.setP2( QPointF( l.p2().x(), l.p2().y() ) );

		lines.append( line );
	}

	QVector< QPointF > points;
	points.reserve( lines.size() * 2 );

	for( int i = 0; i < lines.size(); ++i )
	{
		const QLineF line = lines.at( i );

		points.append( line.p1() );
		points.append( line.p2() );
	}

	QPolygonF polygon( points );

	const QRectF b = polygon.boundingRect();

	const qreal mx = b.width() / cfg.size().width();
	const qreal my = b.height() / cfg.size().height();

	const QPointF p( cfg.pos().x(), cfg.pos().y() );

	QPainterPath path;

	for( int i = 0; i < lines.size(); ++i )
	{
		const QLineF line = lines.at( i );

		if( i == 0 )
		{
			path.moveTo( QPointF(
				( line.p1().x() - b.x() ) / mx + b.x() + p.x(),
				( line.p1().y() - b.y() ) / my + b.y() + p.y() ) );
		}

		path.lineTo( QPointF(
			( line.p2().x() - b.x() ) / mx + b.x() + p.x(),
			( line.p2().y() - b.y() ) / my + b.y() + p.y() ) );
	}

	painter->setPen( Cfg::fromPen( cfg.pen() ) );

	painter->setBrush( Cfg::fromBrush( cfg.brush() ) );

	painter->drawPath( path );
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
			QPointF( d->m_start->halfOfSize(), d->m_start->halfOfSize() ) +
			pos() );

	if( d->m_lines.isEmpty() || d->m_lines.last().p2() == line.p1() )
	{
		d->m_lines.append( line );

		d->makePath();

		d->m_end->setPos( line.p2() -
			QPointF( d->m_end->halfOfSize(), d->m_end->halfOfSize() ) + pos() );
	}
	else
	{
		d->m_lines.prepend( QLineF( line.p2(), line.p1() ) );

		d->makePath();

		d->m_start->setPos( line.p2() -
			QPointF( d->m_start->halfOfSize(), d->m_start->halfOfSize() ) +
			pos() );
	}
}

void
FormPolyline::removeLine( const QLineF & line )
{
	if( d->m_lines.first() == line )
	{
		d->m_lines.removeFirst();

		d->makePath();

		d->m_start->setPos( d->m_lines.first().p1() -
			QPointF( d->m_start->halfOfSize(), d->m_start->halfOfSize() ) +
			pos() );
	}
	else if( d->m_lines.last() == line )
	{
		d->m_lines.removeLast();

		d->makePath();

		d->m_end->setPos( d->m_lines.last().p2() -
			QPointF( d->m_start->halfOfSize(), d->m_start->halfOfSize() ) +
			pos() );
	}
}

int
FormPolyline::countOfLines() const
{
	return d->m_lines.count();
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
			QPointF( d->m_start->halfOfSize(), d->m_start->halfOfSize() ) +
			pos();
	}
	else if( d->m_end->contains( d->m_end->mapFromScene( p ) ) )
	{
		intersected = true;

		return d->m_end->pos() +
			QPointF( d->m_end->halfOfSize(), d->m_end->halfOfSize() ) + pos();
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
	return QGraphicsPathItem::boundingRect();
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
		const qreal w = (qreal) objectPen().width() / 2.0;

		d->m_handles->place( d->m_resized.adjusted( -12.0 - w, -12.0 - w,
			12.0 + w, 12.0 + w ) );

		d->m_handles->show();
	}
	else
		d->m_handles->hide();
}

void
FormPolyline::setPosition( const QPointF & p, bool pushUndoCommand )
{
	if( pushUndoCommand )
		form()->undoStack()->push( new UndoMove< FormPolyline > ( form(),
			objectId(), p - position() ) );

	setPos( p - d->boundingRect().topLeft() );

	QRectF r = boundingRect();
	r.moveTopLeft( pos() + r.topLeft() );

	d->m_resized.moveTopLeft( r.topLeft() );

	const qreal w = (qreal) objectPen().width() / 2.0;
	r.adjust( -12.0 - w, -12.0 - w, 12.0 + w, 12.0 + w );

	d->m_handles->place( r );
}

QPointF
FormPolyline::position() const
{
	return pos() + QGraphicsPathItem::boundingRect().topLeft();
}

QRectF
FormPolyline::rectangle() const
{
	QRectF r = boundingRect();
	r.moveTopLeft( position() );

	return r;
}

void
FormPolyline::setRectangle( const QRectF & rect, bool pushUndoCommand )
{
	Q_UNUSED( pushUndoCommand )

	if( d->m_handles->checkConstraint( rect.size() ) )
		d->updateLines( d->boundingRect(), rect );
}

void
FormPolyline::handleMoved( const QPointF & delta, FormMoveHandle * handle )
{
	if( !d->m_handleMoved )
	{
		d->m_subsidiaryRect = d->m_resized;

		d->m_handleMoved = true;
	}

	if( handle == d->m_handles->m_move.data() )
	{
		moveBy( delta.x(), delta.y() );

		d->m_resized.moveTopLeft( d->m_resized.topLeft() + delta );

		const qreal w = (qreal) objectPen().width() / 2.0;

		d->m_handles->place( d->m_resized.adjusted( -12.0 - w, -12.0 - w,
			12.0 + w, 12.0 + w ) );
	}
	else if( handle == d->m_handles->m_topLeft.data() )
	{
		const QRectF r =
			d->m_resized.adjusted( delta.x(), delta.y(), 0.0, 0.0 );

		if( d->m_handles->checkConstraint( r.size() ) )
			d->updateLines( d->boundingRect(), r );
	}
	else if( handle == d->m_handles->m_top.data() )
	{
		const QRectF r =
			d->m_resized.adjusted( 0.0, delta.y(), 0.0, 0.0 );

		if( d->m_handles->checkConstraint( r.size() ) )
			d->updateLines( d->boundingRect(), r );
	}
	else if( handle == d->m_handles->m_topRight.data() )
	{
		const QRectF r =
			d->m_resized.adjusted( 0.0, delta.y(), delta.x(), 0.0 );

		if( d->m_handles->checkConstraint( r.size() ) )
			d->updateLines( d->boundingRect(), r );
	}
	else if( handle == d->m_handles->m_right.data() )
	{
		const QRectF r =
			d->m_resized.adjusted( 0.0, 0.0, delta.x(), 0.0 );

		if( d->m_handles->checkConstraint( r.size() ) )
			d->updateLines( d->boundingRect(), r );
	}
	else if( handle == d->m_handles->m_bottomRight.data() )
	{
		const QRectF r =
			d->m_resized.adjusted( 0.0, 0.0, delta.x(), delta.y() );

		if( d->m_handles->checkConstraint( r.size() ) )
			d->updateLines( d->boundingRect(), r );
	}
	else if( handle == d->m_handles->m_bottom.data() )
	{
		const QRectF r =
			d->m_resized.adjusted( 0.0, 0.0, 0.0, delta.y() );

		if( d->m_handles->checkConstraint( r.size() ) )
			d->updateLines( d->boundingRect(), r );
	}
	else if( handle == d->m_handles->m_bottomLeft.data() )
	{
		const QRectF r =
			d->m_resized.adjusted( delta.x(), 0.0, 0.0, delta.y() );

		if( d->m_handles->checkConstraint( r.size() ) )
			d->updateLines( d->boundingRect(), r );
	}
	else if( handle == d->m_handles->m_left.data() )
	{
		const QRectF r =
			d->m_resized.adjusted( delta.x(), 0.0, 0.0, 0.0 );

		if( d->m_handles->checkConstraint( r.size() ) )
			d->updateLines( d->boundingRect(), r );
	}

	form()->update();
}

void
FormPolyline::handleReleased( FormMoveHandle * handle )
{
	d->m_handleMoved = false;

	if( handle == d->m_handles->m_move.data() )
		form()->undoStack()->push( new UndoMove< FormPolyline > ( form(),
			objectId(), d->m_resized.topLeft() - d->m_subsidiaryRect.topLeft() ) );
	else
		form()->undoStack()->push( new UndoResize< FormPolyline > ( form(),
			objectId(), d->m_subsidiaryRect, d->m_resized ) );

	form()->emitChanged();
}

} /* namespace Core */

} /* namespace Prototyper */
