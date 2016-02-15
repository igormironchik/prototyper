
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
#include "form_grid_snap.hpp"

// Qt include.
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneMouseEvent>


namespace Prototyper {

namespace Core {

//
// GridSnapPrivate
//

class GridSnapPrivate {
public:
	GridSnapPrivate( GridSnap * parent )
		:	q( parent )
		,	m_step( 10.0 )
		,	m_size( 5 )
	{
	}

	//! Init.
	void init();
	//! Calc snap pos.
	void calc();
	//! Calc snap pos.
	inline QPointF calc( const QPointF & p );

	//! Parent.
	GridSnap * q;
	//! Grid step.
	int m_step;
	//! Pos.
	QPointF m_pos;
	//! Size.
	const int m_size;
}; // class GridSnapPrivate

void
GridSnapPrivate::init()
{
	q->setZValue( 999999999999 );
}

QPointF
GridSnapPrivate::calc( const QPointF & p )
{
	return QPointF( (int) p.x() / m_step * m_step +
			qRound( (qreal) ( (int) p.x() % m_step ) / (qreal) m_step ) * m_step,
		(int) p.y() / m_step * m_step +
			qRound( (qreal) ( (int) p.y() % m_step ) / (qreal) m_step ) * m_step );
}

void
GridSnapPrivate::calc()
{
	m_pos = calc( m_pos );

	q->setPos( m_pos - QPointF( m_size, m_size ) );
}


//
// GridSnap
//

GridSnap::GridSnap( QGraphicsItem * parent )
	:	QGraphicsItem( parent )
	,	d( 0 )
{
	QScopedPointer< GridSnapPrivate > tmp( new GridSnapPrivate( this ) );

	tmp->init();

	d.swap( tmp );
}

GridSnap::~GridSnap()
{
}

const QPointF &
GridSnap::snapPos() const
{
	return d->m_pos;
}

void
GridSnap::setSnapPos( const QPointF & p )
{
	d->m_pos = p;

	d->calc();
}

void
GridSnap::setGridStep( int step )
{
	d->m_step = step;

	d->calc();
}

QPointF
GridSnap::snapped( const QPointF & p ) const
{
	return d->calc( p );
}

QRectF
GridSnap::boundingRect() const
{
	if( !d.isNull() )
		return QRectF( 0, 0, d->m_size * 2, d->m_size * 2 );
	else
		return QRectF();
}

void
GridSnap::paint( QPainter * painter, const QStyleOptionGraphicsItem * option,
	QWidget * widget )
{
	Q_UNUSED( widget )

	painter->setPen( Qt::gray );
	painter->drawLine( option->rect.x(), option->rect.y(),
		option->rect.x() + option->rect.width(),
		option->rect.y() + option->rect.height() );
	painter->drawLine( option->rect.x() + option->rect.width(),
		option->rect.y(), option->rect.x(),
		option->rect.y() + option->rect.height() );
}

void
GridSnap::mouseMoveEvent( QGraphicsSceneMouseEvent * event )
{
	event->ignore();
}

void
GridSnap::mousePressEvent( QGraphicsSceneMouseEvent * event )
{
	event->ignore();
}

void
GridSnap::mouseReleaseEvent( QGraphicsSceneMouseEvent * event )
{
	event->ignore();
}

} /* namespace Core */

} /* namespace Prototyper */
