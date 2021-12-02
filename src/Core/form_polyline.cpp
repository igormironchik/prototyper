
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
#include "form_polyline_handles.hpp"
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
#include <vector>


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
	{
	}

	//! Init.
	void init();
	//! Make path.
	void makePath();
	//! Update lines.
	void resize( const QRectF & oldR, const QRectF & newR );
	//! \return Bounding rect.
	QRectF boundingRect() const;
	//! Create edit handles.
	void createEditHandles();
	//! Clear edit handles.
	void clearEditHandles();
	//! \return Index of edit handle.
	std::size_t indexOfEditHandle( FormMoveHandle * h ) const;
	//! Show/hide edit nodes.
	void setVisibleEditNodes( bool on = true );

	//! Parent.
	FormPolyline * q;
	//! Lines.
	QVector< QLineF > m_lines;
	//! Polygon.
	QPolygonF m_polygon;
	//! Start handle.
	std::unique_ptr< FormMoveHandle > m_start;
	//! End handle.
	std::unique_ptr< FormMoveHandle > m_end;
	//! Closed?
	bool m_closed;
	//! Resize & move handles.
	std::unique_ptr< FormPolylineHandles > m_handles;
	//! Resized bounding rect.
	QRectF m_resized;
	//! Handles for editing lines.
	std::vector< std::unique_ptr< FormMoveHandle > > m_editHandles;
	//! Currently edited lines.
	QVector< QLineF > m_currentEditedLines;
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

	auto tmp = std::make_unique< FormPolylineHandles > ( q, q->parentItem(), q->page() );

	m_handles.swap( tmp );

	m_handles->handles()->setMinSize( QSizeF( c_minResizableSize, c_minResizableSize ) );
	m_handles->hide();

	q->setObjectPen( QPen( PageAction::instance()->strokeColor(), c_linePenWidth ),
		false );

	q->setObjectBrush( QBrush( PageAction::instance()->fillColor() ), false );

	FormPolyline::connect( m_handles.get(), &FormPolylineHandles::currentModeChanged,
		q, &FormPolyline::modeChanged );
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

	m_handles->setRect( r );

	m_resized = r;

	if( m_closed )
	{
		q->setBrush( q->objectBrush() );

		q->showHandles( false );
	}
}

void
FormPolylinePrivate::resize( const QRectF & oldR, const QRectF & newR )
{
	Q_UNUSED( oldR )

	const QRectF b = boundingRect();

	m_resized = newR;

	m_handles->setRect( m_resized );

	const qreal mx = b.width() / m_resized.width();
	const qreal my = b.height() / m_resized.height();

	const QPointF p = q->pos();

	QPainterPath path;
	QVector< QPointF > points;
	points.reserve( m_lines.size() * 2 );

	for( int i = 0; i < m_lines.size(); ++i )
	{
		QLineF & line = m_lines[ i ];
		line.setP1( QPointF(
			( line.p1().x() - b.x() ) / mx + m_resized.x() - p.x(),
			( line.p1().y() - b.y() ) / my + m_resized.y() - p.y() ) );
		line.setP2( QPointF(
			( line.p2().x() - b.x() ) / mx + m_resized.x() - p.x(),
			( line.p2().y() - b.y() ) / my + m_resized.y() - p.y() ) );

		points.append( line.p1() );
		points.append( line.p2() );

		if( i == 0 )
			path.moveTo( line.p1() );

		path.lineTo( line.p2() );
	}

	m_polygon = QPolygonF( points );

	q->setPath( path );
}

QRectF
FormPolylinePrivate::boundingRect() const
{
	return m_polygon.boundingRect();
}

void
FormPolylinePrivate::createEditHandles()
{
	for( int i = 0; i < m_lines.size(); ++i )
	{
		auto h = std::make_unique< FormMoveHandle >( c_halfHandleSize,
			QPointF( c_halfHandleSize, c_halfHandleSize ), q,
			q->parentItem(), q->page(), Qt::CrossCursor, true );

		h->setPos( m_lines[ i ].p1() - QPointF( c_halfHandleSize, c_halfHandleSize ) + q->pos() );

		h->setVisible( true );

		m_editHandles.push_back( std::move( h ) );
	}

	if( !m_closed )
	{
		auto h = std::make_unique< FormMoveHandle >( c_halfHandleSize,
			QPointF( c_halfHandleSize, c_halfHandleSize ), q,
			q->parentItem(), q->page(), Qt::CrossCursor, true );

		h->setPos( m_lines.back().p2() - QPointF( c_halfHandleSize, c_halfHandleSize ) + q->pos() );

		h->setVisible( true );

		m_editHandles.push_back( std::move( h ) );
	}
}

void
FormPolylinePrivate::clearEditHandles()
{
	m_editHandles.clear();
}

std::size_t
FormPolylinePrivate::indexOfEditHandle( FormMoveHandle * h ) const
{
	std::size_t idx = 0;

	for( const auto & hh : m_editHandles )
	{
		if( hh.get() == h )
			return idx;

		++idx;
	}

	return idx;
}

void
FormPolylinePrivate::setVisibleEditNodes( bool on )
{
	for( std::size_t i = 0; i < m_editHandles.size(); ++i )
		m_editHandles[ i ]->setVisible( on );
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


	d->m_handles->setRect( d->m_resized );

	d->resize( QRectF(), d->m_resized );
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

		QRectF r = d->boundingRect();
		r.moveTopLeft( pos() + r.topLeft() );
		d->m_handles->setRect( r );
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
		d->m_handles->show();
		d->setVisibleEditNodes( true );
	}
	else
	{
		d->m_handles->hide();
		d->setVisibleEditNodes( false );
	}
}

void
FormPolyline::setPosition( const QPointF & p, bool pushUndoCommand )
{
	FormObject::setPosition( p, pushUndoCommand );

	setPos( p - d->boundingRect().topLeft() );

	QRectF r = d->boundingRect();
	r.moveTopLeft( pos() + r.topLeft() );

	d->m_resized.moveTopLeft( r.topLeft() );

	d->m_handles->setRect( r );
}

QPointF
FormPolyline::position() const
{
	return pos() + d->boundingRect().topLeft();
}

QRectF
FormPolyline::rectangle() const
{
	QRectF r = d->boundingRect();
	r.moveTopLeft( position() );

	return r;
}

void
FormPolyline::setRectangle( const QRectF & rect, bool pushUndoCommand )
{
	FormObject::setRectangle( rect, pushUndoCommand );

	resize( rect );

	scene()->update();

	updatePropertiesValues();
}

void
FormPolyline::resize( const QRectF & rect )
{
	d->m_handles->setRect( rect );

	d->resize( d->boundingRect(), rect );

	page()->update();
}

void
FormPolyline::moveResizable( const QPointF & delta )
{
	moveBy( delta.x(), delta.y() );
}

void
FormPolyline::handleMoved( const QPointF & delta, FormMoveHandle * handle )
{
	const auto i = d->indexOfEditHandle( handle );

	if( i < d->m_editHandles.size() )
	{
		auto & l1 = ( i < d->m_lines.size() ? d->m_lines[ i ] : d->m_lines.back() );
		auto & l2 = ( i == 0 ? d->m_lines[ d->m_lines.size() - 1 ] : d->m_lines[ i - 1 ] );

		if( d->m_currentEditedLines.isEmpty() )
		{
			d->m_currentEditedLines.push_back( l1 );
			d->m_currentEditedLines.push_back( l2 );
		}

		if( i < d->m_lines.size() )
			l1.setP1( l1.p1() + delta );
		else
			l1.setP2( l1.p2() + delta );

		if( ( i == 0 && d->m_closed ) || ( i < d->m_lines.size() && i > 0 ) )
			l2.setP2( l2.p2() + delta );

		d->makePath();
	}
}

void
FormPolyline::handleReleased( FormMoveHandle * handle )
{
	const auto i = d->indexOfEditHandle( handle );

	if( i < d->m_editHandles.size() )
	{
		auto & l1 = ( i < d->m_lines.size() ? d->m_lines[ i ] : d->m_lines.back() );
		auto & l2 = ( i == 0 ? d->m_lines[ d->m_lines.size() - 1 ] : d->m_lines[ i - 1 ] );

		page()->undoStack()->push(
			new UndoEditPoly( page(),
				d->m_currentEditedLines.front(), d->m_currentEditedLines.back(),
				l1, l2, i, objectId() ) );
	}

	d->m_currentEditedLines.clear();
}

void
FormPolyline::moveNode( int i, const QLineF & newL1, const QLineF & newL2 )
{
	auto & l1 = ( i < d->m_lines.size() ? d->m_lines[ i ] : d->m_lines.back() );
	auto & l2 = ( i == 0 ? d->m_lines[ d->m_lines.size() - 1 ] : d->m_lines[ i - 1 ] );

	QPointF p;

	if( i < d->m_lines.size() )
	{
		l1.setP1( newL1.p1() );
		p = l1.p1();
	}
	else
	{
		l1.setP2( newL1.p2() );
		p = l1.p2();
	}

	if( ( i == 0 && d->m_closed ) || ( i < d->m_lines.size() && i > 0 ) )
		l2.setP2( newL2.p2() );

	d->makePath();

	if( !d->m_editHandles.empty() )
		d->m_editHandles[ i ]->setPos( p -
			QPointF( d->m_start->halfOfSize(), d->m_start->halfOfSize() ) + pos() );
}

FormObject *
FormPolyline::clone() const
{
	auto * o = new FormPolyline( page(), parentItem() );

	o->setCfg( cfg() );

	o->setObjectId( page()->nextId() );

	return o;
}

void
FormPolyline::modeChanged()
{
	if( d->m_handles->currentMode() == NodesEditResizeHandle::NodesEditMode )
	{
		d->m_handles->setHandlesVisible( false );

		d->createEditHandles();
	}
	else
	{
		d->m_handles->setHandlesVisible( true );

		d->clearEditHandles();
	}
}

} /* namespace Core */

} /* namespace Prototyper */
