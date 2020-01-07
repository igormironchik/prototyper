
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
#include "form_rect_placer.hpp"

// Qt include.
#include <QPainter>
#include <QStyleOptionGraphicsItem>


namespace Prototyper {

namespace Core {

//
// FormRectPlacerPrivate
//

class FormRectPlacerPrivate {
public:
	explicit FormRectPlacerPrivate( FormRectPlacer * parent )
		:	q( parent )
		,	m_rect( 0.0, 0.0, 0.0, 0.0 )
		,	m_startPos( 0.0, 0.0 )
		,	m_width( 0.0 )
		,	m_height( 0.0 )
	{
	}

	//! Init.
	void init();

	//! Parent.
	FormRectPlacer * q;
	//! Rect.
	QRectF m_rect;
	//! Start pos.
	QPointF m_startPos;
	//! Width.
	qreal m_width;
	//! Height.
	qreal m_height;
}; // class FormRectPlacerPrivate

void
FormRectPlacerPrivate::init()
{
}


//
// FormRectPlacer
//

FormRectPlacer::FormRectPlacer( QGraphicsItem * parent )
	:	QGraphicsItem( parent )
	,	d( 0 )
{
	QScopedPointer< FormRectPlacerPrivate > tmp(
		new FormRectPlacerPrivate( this ) );

	tmp->init();

	d.swap( tmp );
}

FormRectPlacer::~FormRectPlacer()
{
}

void
FormRectPlacer::setStartPos( const QPointF & pos )
{
	d->m_startPos = pos;
	d->m_rect = QRectF( pos.x(), pos.y(), 0.0, 0.0 );

	update();
}

const QRectF &
FormRectPlacer::rect() const
{
	return d->m_rect;
}

void
FormRectPlacer::setEndPos( const QPointF & pos )
{
	const QRectF tmp = d->m_rect.adjusted( -20.0, -20.0, 20.0, 20.0 );

	d->m_width = pos.x() - d->m_startPos.x();
	d->m_height = pos.y() - d->m_startPos.y();

	d->m_rect = QRectF( d->m_startPos.x(), d->m_startPos.y(), d->m_width, d->m_height );

	update();

	parentItem()->update( tmp );
}

QRectF
FormRectPlacer::boundingRect() const
{
	if( !d.isNull() )
		return d->m_rect;
	else
		return QRectF();
}

void
FormRectPlacer::paint( QPainter * painter,
	const QStyleOptionGraphicsItem * option,
	QWidget * widget )
{
	Q_UNUSED( widget )

	painter->setPen( QPen( Qt::gray, 1.0, Qt::DashLine ) );
	painter->setBrush( Qt::NoBrush );

	painter->drawRect( option->rect );
}

} /* namespace Core */

} /* namespace Prototyper */
