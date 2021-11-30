
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
#include "form_polyline.hpp"
#include "form_move_handle.hpp"
#include "form_actions.hpp"
#include "form_resize_handle.hpp"
#include "form_with_resize_and_move_handles.hpp"
#include "utils.hpp"
#include "page.hpp"
#include "form_undo_commands.hpp"
#include "constants.hpp"

// Qt include.
#include <QStyleOptionGraphicsItem>
#include <QPolygonF>
#include <QApplication>
#include <QPainter>
#include <QUndoStack>
#include <QGraphicsScene>

// C++ include.
#include <memory>


namespace Prototyper {

namespace Core {

//
// FormPolylinePrivate
//

class FormPolylinePrivate {
public:
	explicit FormPolylinePrivate( FormPolyline * parent )
		:	q( parent )
		,	m_start( nullptr )
		,	m_end( nullptr )
		,	m_closed( false )
		,	m_handles( nullptr )
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
	std::unique_ptr< FormMoveHandle > m_start;
	//! End handle.
	std::unique_ptr< FormMoveHandle > m_end;
	//! Closed?
	bool m_closed;
	//! Resize & move handles.
	std::unique_ptr< WithResizeAndMoveHandles > m_handles;
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
	m_start = std::make_unique< FormMoveHandle >( c_halfHandleSize,
		QPointF( c_halfHandleSize, c_halfHandleSize ), q,
		q->parentItem(), q->page() );
	m_start->ignoreMouseEvents( true );
	m_start->unsetCursor();

	m_end = std::make_unique< FormMoveHandle >( c_halfHandleSize,
		QPointF( c_halfHandleSize, c_halfHandleSize ), q,
		q->parentItem(), q->page() );
	m_end->ignoreMouseEvents( true );
	m_end->unsetCursor();

	m_start->hide();
	m_end->hide();

	auto tmp = std::make_unique< WithResizeAndMoveHandles> ( q, q->parentItem(), q->page() );

	m_handles.swap( tmp );

	m_handles->setMinSize( QSizeF( c_minResizableSize, c_minResizableSize ) );

	m_handles->setDeltaToZero( 1.0 );

	m_handles->hide();

	q->setObjectPen( QPen( PageAction::instance()->strokeColor(), c_linePenWidth ),
		false );

	q->setObjectBrush( QBrush( PageAction::instance()->fillColor() ), false );
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

	const qreal w = (qreal) q->objectPen().width() / c_halfDivider;

	m_handles->place( m_resized.adjusted( -c_halfHandleSize * c_halfDivider * c_halfDivider - w,
		-c_halfHandleSize * c_halfDivider * c_halfDivider - w,
		c_halfHandleSize * c_halfDivider * c_halfDivider + w,
		c_halfHandleSize * c_halfDivider * c_halfDivider + w ) );

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

FormPolyline::FormPolyline( Page * page, QGraphicsItem * parent )
	:	QGraphicsPathItem( parent )
	,	FormObject( FormObject::PolylineType, page )
	,	d( new FormPolylinePrivate( this ) )
{
	d->init();
}

FormPolyline::~FormPolyline() = default;

Cfg::Polyline
FormPolyline::cfg() const
{
	Cfg::Polyline c;

	c.set_objectId( objectId() );

	Cfg::Point p;
	p.set_x( MmPx::instance().toMmX( pos().x() ) );
	p.set_y( MmPx::instance().toMmY( pos().y() ) );

	c.set_pos( p );

	foreach( const QLineF & l, d->m_lines )
	{
		Cfg::Point p1;
		p1.set_x( MmPx::instance().toMmX( l.p1().x() ) );
		p1.set_y( MmPx::instance().toMmY( l.p1().y() ) );

		Cfg::Point p2;
		p2.set_x( MmPx::instance().toMmX( l.p2().x() ) );
		p2.set_y( MmPx::instance().toMmY( l.p2().y() ) );

		Cfg::Line line;
		line.set_p1( p1 );
		line.set_p2( p2 );

		line.set_pen( Cfg::pen( objectPen() ) );

		c.line().push_back( line );
	}

	c.set_pen( Cfg::pen( objectPen() ) );

	c.set_brush( Cfg::brush( objectBrush() ) );

	Cfg::Size s;
	s.set_width( MmPx::instance().toMmX( d->m_resized.width() ) );
	s.set_height( MmPx::instance().toMmY( d->m_resized.height() ) );

	c.set_size( s );

	c.set_z( zValue() );

	return c;
}

void
FormPolyline::setCfg( const Cfg::Polyline & c )
{
	setObjectId( c.objectId() );

	d->m_lines.clear();

	setPos( QPointF( MmPx::instance().fromMmX( c.pos().x() ),
		MmPx::instance().fromMmY( c.pos().y() ) ) );

	foreach( const Cfg::Line & l, c.line() )
	{
		QLineF line;
		line.setP1( QPointF( MmPx::instance().fromMmX( l.p1().x() ),
			MmPx::instance().fromMmY( l.p1().y() ) ) );
		line.setP2( QPointF( MmPx::instance().fromMmX( l.p2().x() ),
			MmPx::instance().fromMmY( l.p2().y() ) ) );

		appendLine( line );
	}

	setObjectPen( Cfg::fromPen( c.pen() ), false );

	setObjectBrush( Cfg::fromBrush( c.brush() ), false );

	setZValue( c.z() );

	const QRectF b = d->boundingRect();

	d->m_resized = QRectF( MmPx::instance().fromMmX( c.pos().x() ) + b.x(),
		MmPx::instance().fromMmY( c.pos().y() ) + b.y(),
		MmPx::instance().fromMmX( c.size().width() ),
		MmPx::instance().fromMmY( c.size().height() ) );

	const qreal w = (qreal) objectPen().width() / c_halfDivider;

	d->m_handles->place( d->m_resized.adjusted(
		-c_halfHandleSize * c_halfDivider * c_halfDivider - w,
		-c_halfHandleSize * c_halfDivider * c_halfDivider - w,
		c_halfHandleSize * c_halfDivider * c_halfDivider + w,
		c_halfHandleSize * c_halfDivider * c_halfDivider + w ) );

	d->updateLines( QRectF(), d->m_resized );
}

void
FormPolyline::draw( QPainter * painter, const Cfg::Polyline & cfg, qreal dpi )
{
	QList< QLineF > lines;

	foreach( const Cfg::Line & l, cfg.line() )
	{
		QLineF line;
		line.setP1( QPointF( MmPx::instance().fromMm( l.p1().x(), dpi ),
			MmPx::instance().fromMm( l.p1().y(), dpi ) ) );
		line.setP2( QPointF( MmPx::instance().fromMm( l.p2().x(), dpi ),
			MmPx::instance().fromMm( l.p2().y(), dpi ) ) );

		lines.append( line );
	}

	QVector< QPointF > points;
	points.reserve( lines.size() * 2 );

	for( const auto & line : qAsConst( lines ) )
	{
		points.append( line.p1() );
		points.append( line.p2() );
	}

	QPolygonF polygon( points );

	const QRectF b = polygon.boundingRect();

	const qreal mx = b.width() / MmPx::instance().fromMm( cfg.size().width(), dpi );
	const qreal my = b.height() / MmPx::instance().fromMm( cfg.size().height(), dpi );

	const QPointF p( MmPx::instance().fromMm( cfg.pos().x(), dpi ),
		MmPx::instance().fromMm( cfg.pos().y(), dpi ) );

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

	painter->setPen( Cfg::fromPen( cfg.pen(), dpi ) );

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

	if( d->m_end->contains( d->m_end->mapFromScene( p ) ) )
	{
		intersected = true;

		return d->m_end->pos() +
			QPointF( d->m_end->halfOfSize(), d->m_end->halfOfSize() ) + pos();
	}

	intersected = false;

	return p;
}

void
FormPolyline::setObjectPen( const QPen & p, bool pushUndoCommand )
{
	QPen newPen = p;
	newPen.setWidth( 2 );

	FormObject::setObjectPen( newPen, pushUndoCommand );

	setPen( newPen );

	update();
}

void
FormPolyline::setObjectBrush( const QBrush & b, bool pushUndoCommand )
{
	if( d->m_closed )
		setBrush( b );

	FormObject::setObjectBrush( b, pushUndoCommand );

	update();
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

	d->m_end->handleMouseMove( p );
}

void
FormPolyline::paint( QPainter * painter, const QStyleOptionGraphicsItem * option,
	QWidget * widget )
{
	QGraphicsPathItem::paint( painter, option, widget );

	if( isSelected() && !group() )
	{
		const qreal w = (qreal) objectPen().width() / c_halfDivider;

		d->m_handles->place( d->m_resized.adjusted(
			-c_halfHandleSize * c_halfDivider * c_halfDivider - w,
			-c_halfHandleSize * c_halfDivider * c_halfDivider - w,
			c_halfHandleSize * c_halfDivider * c_halfDivider + w,
			c_halfHandleSize * c_halfDivider * c_halfDivider + w ) );

		d->m_handles->show();
	}
	else
		d->m_handles->hide();
}

void
FormPolyline::setPosition( const QPointF & p, bool pushUndoCommand )
{
	FormObject::setPosition( p, pushUndoCommand );

	setPos( p - d->boundingRect().topLeft() );

	QRectF r = boundingRect();
	r.moveTopLeft( pos() + r.topLeft() );

	d->m_resized.moveTopLeft( r.topLeft() );

	const qreal w = (qreal) objectPen().width() / c_halfDivider;
	r.adjust(
		-c_halfHandleSize * c_halfDivider * c_halfDivider - w,
		-c_halfHandleSize * c_halfDivider * c_halfDivider - w,
		c_halfHandleSize * c_halfDivider * c_halfDivider + w,
		c_halfHandleSize * c_halfDivider * c_halfDivider + w );

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
	FormObject::setRectangle( rect, pushUndoCommand );

	if( d->m_handles->checkConstraint( rect.size() ) )
	{
		d->updateLines( d->boundingRect(), rect );

		scene()->update();
	}
}

void
FormPolyline::handleMoved( const QPointF & delta, FormMoveHandle * handle )
{
	if( !d->m_handleMoved )
	{
		d->m_subsidiaryRect = d->m_resized;

		d->m_handleMoved = true;
	}

	if( handle == d->m_handles->m_move.get() )
	{
		moveBy( delta.x(), delta.y() );

		d->m_resized.moveTopLeft( d->m_resized.topLeft() + delta );

		const qreal w = (qreal) objectPen().width() / c_halfDivider;

		d->m_handles->place( d->m_resized.adjusted(
			-c_halfHandleSize * c_halfDivider * c_halfDivider - w,
			-c_halfHandleSize * c_halfDivider * c_halfDivider - w,
			c_halfHandleSize * c_halfDivider * c_halfDivider + w,
			c_halfHandleSize * c_halfDivider * c_halfDivider + w ) );
	}
	else if( handle == d->m_handles->m_topLeft.get() )
	{
		const QRectF r =
			d->m_resized.adjusted( delta.x(), delta.y(), 0.0, 0.0 );

		if( d->m_handles->checkConstraint( r.size() ) )
			d->updateLines( d->boundingRect(), r );
	}
	else if( handle == d->m_handles->m_top.get() )
	{
		const QRectF r =
			d->m_resized.adjusted( 0.0, delta.y(), 0.0, 0.0 );

		if( d->m_handles->checkConstraint( r.size() ) )
			d->updateLines( d->boundingRect(), r );
	}
	else if( handle == d->m_handles->m_topRight.get() )
	{
		const QRectF r =
			d->m_resized.adjusted( 0.0, delta.y(), delta.x(), 0.0 );

		if( d->m_handles->checkConstraint( r.size() ) )
			d->updateLines( d->boundingRect(), r );
	}
	else if( handle == d->m_handles->m_right.get() )
	{
		const QRectF r =
			d->m_resized.adjusted( 0.0, 0.0, delta.x(), 0.0 );

		if( d->m_handles->checkConstraint( r.size() ) )
			d->updateLines( d->boundingRect(), r );
	}
	else if( handle == d->m_handles->m_bottomRight.get() )
	{
		const QRectF r =
			d->m_resized.adjusted( 0.0, 0.0, delta.x(), delta.y() );

		if( d->m_handles->checkConstraint( r.size() ) )
			d->updateLines( d->boundingRect(), r );
	}
	else if( handle == d->m_handles->m_bottom.get() )
	{
		const QRectF r =
			d->m_resized.adjusted( 0.0, 0.0, 0.0, delta.y() );

		if( d->m_handles->checkConstraint( r.size() ) )
			d->updateLines( d->boundingRect(), r );
	}
	else if( handle == d->m_handles->m_bottomLeft.get() )
	{
		const QRectF r =
			d->m_resized.adjusted( delta.x(), 0.0, 0.0, delta.y() );

		if( d->m_handles->checkConstraint( r.size() ) )
			d->updateLines( d->boundingRect(), r );
	}
	else if( handle == d->m_handles->m_left.get() )
	{
		const QRectF r =
			d->m_resized.adjusted( delta.x(), 0.0, 0.0, 0.0 );

		if( d->m_handles->checkConstraint( r.size() ) )
			d->updateLines( d->boundingRect(), r );
	}

	page()->update();
}

void
FormPolyline::handleReleased( FormMoveHandle * handle )
{
	d->m_handleMoved = false;

	if( handle == d->m_handles->m_move.get() )
		page()->undoStack()->push( new UndoMove( page(),
			objectId(), d->m_resized.topLeft() - d->m_subsidiaryRect.topLeft() ) );
	else
		page()->undoStack()->push( new UndoResize( page(),
			objectId(), d->m_subsidiaryRect, d->m_resized ) );

	updatePropertiesValues();

	page()->emitChanged();
}

FormObject *
FormPolyline::clone() const
{
	auto * o = new FormPolyline( page(), parentItem() );

	o->setCfg( cfg() );

	o->setObjectId( page()->nextId() );

	return o;
}

} /* namespace Core */

} /* namespace Prototyper */
