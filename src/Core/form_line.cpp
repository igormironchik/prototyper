
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
#include "form_line.hpp"
#include "form_move_handle.hpp"
#include "form_actions.hpp"
#include "page.hpp"
#include "form_grid_snap.hpp"
#include "utils.hpp"
#include "form_undo_commands.hpp"
#include "constants.hpp"

// Qt include.
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QApplication>
#include <QGraphicsScene>
#include <QUndoStack>


namespace Prototyper {

namespace Core {

//
// FormLinePrivate
//

class FormLinePrivate {
public:
	explicit FormLinePrivate( FormLine * parent )
		:	q( parent )
		,	m_h1( nullptr )
		,	m_h2( nullptr )
		,	m_move( nullptr )
		,	m_showHandles( false )
		,	m_handleMoved( false )
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
	std::unique_ptr< FormMoveHandle > m_h1;
	//! Second handle.
	std::unique_ptr< FormMoveHandle > m_h2;
	//! Move handler.
	std::unique_ptr< FormMoveHandle > m_move;
	//! Show handles?
	bool m_showHandles;
	//! Old position.
	QPointF m_oldPos;
	//! Is handle moved?
	bool m_handleMoved;
	//! Old line.
	QLineF m_oldLine;
}; // class FormLinePrivate

void
FormLinePrivate::init()
{
	createHandles();

	m_h1->hide();
	m_h2->hide();
	m_move->hide();

	q->setObjectPen( QPen( PageAction::instance()->strokeColor(), c_linePenWidth ), false );
}

void
FormLinePrivate::placeChild()
{
	const QLineF l = q->line();

	const QPointF p = q->pos();

	m_h1->setPos( l.p1().x() - m_h1->halfOfSize() + p.x(),
		l.p1().y() - m_h1->halfOfSize() + p.y() );

	m_h2->setPos( l.p2().x() - m_h2->halfOfSize() + p.x(),
		l.p2().y() - m_h2->halfOfSize() + p.y() );

	m_move->setPos(
		( l.p1().x() + l.p2().x() ) / c_halfDivider - m_move->halfOfSize() + p.x(),
		( l.p1().y() + l.p2().y() ) / c_halfDivider - m_move->halfOfSize() + p.y() );
}

void
FormLinePrivate::createHandles()
{
	m_h1.reset( new FormMoveHandle( c_halfHandleSize,
		QPointF( c_halfHandleSize, c_halfHandleSize ), q, q->parentItem(),
		q->page(), Qt::CrossCursor ) );

	m_h2.reset( new FormMoveHandle( c_halfHandleSize,
		QPointF( c_halfHandleSize, c_halfHandleSize ),q, q->parentItem(),
		q->page(), Qt::CrossCursor ) );

	m_move.reset( new FormMoveHandle( c_halfHandleSize,
		QPointF( c_halfHandleSize, c_halfHandleSize ), q, q->parentItem(),
		q->page(), Qt::SizeAllCursor ) );
}


//
// FormLine
//

FormLine::FormLine( Page * page, QGraphicsItem * parent )
	:	QGraphicsLineItem( parent )
	,	FormObject( FormObject::LineType, page, 0 )
	,	d( new FormLinePrivate( this ) )
{
	d->init();
}

FormLine::~FormLine() = default;

Cfg::Line
FormLine::cfg() const
{
	const QLineF l = line();

	Cfg::Line c;

	Cfg::Point p1;
	p1.set_x( MmPx::instance().toMmX( l.p1().x() ) );
	p1.set_y( MmPx::instance().toMmY( l.p1().y() ) );

	Cfg::Point p2;
	p2.set_x( MmPx::instance().toMmX( l.p2().x() ) );
	p2.set_y( MmPx::instance().toMmY( l.p2().y() ) );

	c.p1() = p1;
	c.p2() = p2;

	Cfg::Point p;
	p.set_x( pos().x() );
	p.set_y( pos().y() );

	c.pos() = p;

	c.set_objectId( objectId() );

	c.set_pen( Cfg::pen( objectPen() ) );

	c.set_z( zValue() );

	return c;
}

void
FormLine::setCfg( const Cfg::Line & c )
{
	QLineF line;
	line.setP1( QPointF( MmPx::instance().fromMmX( c.p1().x() ),
		MmPx::instance().fromMmY( c.p1().y() ) ) );
	line.setP2( QPointF( MmPx::instance().fromMmX( c.p2().x() ),
		MmPx::instance().fromMmY( c.p2().y() ) ) );

	setLine( line );

	d->placeChild();

	setPos( QPointF( c.pos().x(), c.pos().y() ) );

	setObjectId( c.objectId() );

	setObjectPen( Cfg::fromPen( c.pen() ), false );

	setZValue( c.z() );
}

QRectF
FormLine::boundingRect() const
{
	return QGraphicsLineItem::boundingRect();
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
FormLine::setObjectPen( const QPen & p, bool pushUndoCommand )
{
	QPen newPen = p;
	newPen.setWidth( 2 );

	FormObject::setObjectPen( newPen, pushUndoCommand );

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

		return line().p1() + pos();
	}

	if( d->m_h2->contains( d->m_h2->mapFromScene( point ) ) )
	{
		intersected = true;

		intersectedEnds = true;

		return line().p2() + pos();
	}

	if( d->m_move->contains( d->m_move->mapFromScene( point ) ) )
	{
		const QLineF l = line();

		intersected = true;

		return QPointF( ( l.p1().x() + l.p2().x() ) / c_halfDivider,
			( l.p1().y() + l.p2().y() ) / c_halfDivider ) + pos();
	}

	intersected = false;

	return point;
}

bool
FormLine::handleMouseMoveInHandles( const QPointF & point )
{
	if( d->m_h1->handleMouseMove( point ) )
		return true;

	if( d->m_h2->handleMouseMove( point ) )
		return true;

	if( d->m_move->handleMouseMove( point ) )
		return true;

	return false;
}

void
FormLine::setPosition( const QPointF & pos, bool pushUndoCommand )
{
	FormObject::setPosition( pos, pushUndoCommand );

	setPos( pos - QGraphicsLineItem::boundingRect().topLeft() );

	d->placeChild();
}

QPointF
FormLine::position() const
{
	return pos() + QGraphicsLineItem::boundingRect().topLeft();
}

QRectF
FormLine::rectangle() const
{
	QRectF r = boundingRect();
	r.moveTopLeft( position() );

	return r;
}

void
FormLine::setRectangle( const QRectF & rect, bool pushUndoCommand )
{
	Q_UNUSED( rect )
	Q_UNUSED( pushUndoCommand )
}

void
FormLine::handleMoved( const QPointF & delta, FormMoveHandle * handle )
{
	if( isSelected() )
	{
		if( !d->m_handleMoved )
		{
			d->m_oldPos = position();

			d->m_oldLine = line();

			d->m_handleMoved = true;
		}

		const QLineF l = line();

		if( handle == d->m_h1.get() )
			setLine( QLineF( l.p1() + delta, l.p2() ) );
		else if( handle == d->m_h2.get() )
			setLine( QLineF( l.p1(), l.p2() + delta ) );
		else if( handle == d->m_move.get() )
			moveBy( delta.x(), delta.y() );
	}
}

void
FormLine::handleReleased( FormMoveHandle * handle )
{
	d->m_handleMoved = false;

	if( isSelected() )
	{
		if( handle == d->m_move.get() )
			page()->undoStack()->push( new UndoMove( page(),
				objectId(), position() - d->m_oldPos ) );
		else
			page()->undoStack()->push( new UndoChangeLine( page(), objectId(),
				d->m_oldLine, line() ) );

		page()->emitChanged();
	}

	updatePropertiesValues();
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

void
FormLine::placeHandles()
{
	d->placeChild();
}

FormObject *
FormLine::clone() const
{
	auto * o = new FormLine( page(), parentItem() );

	o->setCfg( cfg() );

	o->setObjectId( page()->nextId() );

	return o;
}

} /* namespace Core */

} /* namespace Prototyper */
