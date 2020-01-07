
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
#include "form_object.hpp"
#include "form_move_handle.hpp"
#include "form_resize_handle.hpp"
#include "form_with_resize_and_move_handles.hpp"


namespace Prototyper {

namespace Core {

//
// WithResizeAndMoveHandles
//

WithResizeAndMoveHandles::WithResizeAndMoveHandles( FormWithHandle * object,
	QGraphicsItem * parent, Page * form )
	:	m_topLeft( new FormResizeHandle( 6.0, QPointF( 12.0, 12.0 ), -45.0,
			object, parent, form, Qt::SizeFDiagCursor ) )
	,	m_top( new FormResizeHandle( 6.0, QPointF( 6.0, 12.0 ), 0.0,
			object, parent, form, Qt::SizeVerCursor ) )
	,	m_topRight( new FormResizeHandle( 6.0, QPointF( 0.0, 12.0 ), 45.0,
			object, parent, form, Qt::SizeBDiagCursor ) )
	,	m_left( new FormResizeHandle( 6.0, QPointF( 12.0, 6.0 ), -90.0,
			object, parent, form, Qt::SizeHorCursor ) )
	,	m_bottomLeft( new FormResizeHandle( 6.0, QPointF( 12.0, 0.0 ), 45.0,
			object, parent, form, Qt::SizeBDiagCursor ) )
	,	m_bottom( new FormResizeHandle( 6.0, QPointF( 6.0, 0.0 ), 0.0,
			object, parent, form, Qt::SizeVerCursor ) )
	,	m_bottomRight( new FormResizeHandle( 6.0, QPointF( 0.0, 0.0 ), -45.0,
			object, parent, form, Qt::SizeFDiagCursor ) )
	,	m_right( new FormResizeHandle( 6.0, QPointF( 0.0, 6.0 ), 90,
			object, parent, form, Qt::SizeHorCursor ) )
	,	m_move( new FormMoveHandle( 3.0, QPointF( 3.0, 3.0 ),
			object, parent, form, Qt::SizeAllCursor ) )
	,	q( parent )
	,	m_min( 25.0, 25.0 )
{
}

WithResizeAndMoveHandles::~WithResizeAndMoveHandles()
{
}

void
WithResizeAndMoveHandles::place( const QRectF & r )
{
	m_topLeft->setPos( r.x(), r.y() );
	m_top->setPos( r.x() + r.width() / 2.0 -
		m_top->halfOfSize(), r.y() );
	m_topRight->setPos( r.x() + r.width() -
		m_topRight->halfOfSize() * 2.0, r.y() );
	m_left->setPos( r.x(),
		r.y() + r.height() / 2.0 -
			m_left->halfOfSize() );
	m_bottomLeft->setPos( r.x(),
		r.y() + r.height() -
			m_bottomLeft->halfOfSize() * 2.0 );
	m_bottom->setPos( r.x() + r.width() / 2.0 -
			m_bottom->halfOfSize(),
		r.y() + r.height() -
			m_bottom->halfOfSize() * 2.0 );
	m_bottomRight->setPos( r.x() + r.width() -
			m_bottomRight->halfOfSize() * 2.0,
		r.y() + r.height() -
			m_bottomRight->halfOfSize() * 2.0 );
	m_right->setPos( r.x() + r.width() -
			m_right->halfOfSize() * 2.0,
		r.y() + r.height() / 2.0 -
			m_right->halfOfSize() );
	m_move->setPos( r.x() + r.width() / 2.0 -
			m_move->halfOfSize(),
		r.y() + r.height() / 2.0 -
			m_move->halfOfSize() );
}

void
WithResizeAndMoveHandles::show()
{
	m_topLeft->show();
	m_top->show();
	m_topRight->show();
	m_left->show();
	m_bottomLeft->show();
	m_bottom->show();
	m_bottomRight->show();
	m_right->show();
	m_move->show();
}

void
WithResizeAndMoveHandles::hide()
{
	m_topLeft->hide();
	m_top->hide();
	m_topRight->hide();
	m_left->hide();
	m_bottomLeft->hide();
	m_bottom->hide();
	m_bottomRight->hide();
	m_right->hide();
	m_move->hide();
}

bool
WithResizeAndMoveHandles::checkConstraint( const QSizeF & s )
{
	return ( ( s.width() > m_min.width() || qAbs( s.width() - m_min.width() ) < 0.001 ) &&
		( s.height() > m_min.height() || qAbs( s.height() - m_min.height() ) < 0.001 ) );
}

void
WithResizeAndMoveHandles::setMinSize( const QSizeF & min )
{
	m_min = min;
}

void
WithResizeAndMoveHandles::setDeltaToZero( qreal delta )
{
	m_topLeft->setDeltaToZero( QPointF( delta, delta ) );
	m_top->setDeltaToZero( QPointF( 0.0, delta ) );
	m_topRight->setDeltaToZero( QPointF( -delta, delta ) );
	m_left->setDeltaToZero( QPointF( delta, 0.0 ) );
	m_bottomLeft->setDeltaToZero( QPointF( delta, -delta ) );
	m_bottom->setDeltaToZero( QPointF( 0.0, -delta ) );
	m_bottomRight->setDeltaToZero( QPointF( -delta, -delta ) );
	m_right->setDeltaToZero( QPointF( -delta, 0.0 ) );
}

} /* namespace Core */

} /* namespace Prototyper */
