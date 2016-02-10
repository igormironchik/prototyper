
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
#include "form_resize_handle.hpp"

// Qt include.
#include <QPainter>
#include <QPainterPath>


namespace Prototyper {

namespace Core {

//
// FormResizeHandlePrivate
//

class FormResizeHandlePrivate
	:	public FormMoveHandlePrivate
{
public:
	FormResizeHandlePrivate( qreal halfSize, const QPointF & zero,
		qreal angle, FormWithHandle * object, FormResizeHandle * parent,
		const QCursor & c )
		:	FormMoveHandlePrivate( halfSize, zero, object, parent, c )
		,	m_angle( angle )
	{
	}

	~FormResizeHandlePrivate()
	{
	}

	//! Init.
	void init() Q_DECL_OVERRIDE;

	inline FormResizeHandle * q_func()
		{ return static_cast< FormResizeHandle* >( q ); }
	inline const FormResizeHandle * q_func() const
		{ return static_cast< const FormResizeHandle* >( q ); }

	//! Angle.
	qreal m_angle;
	//! Path.
	QPainterPath m_path;
}; // class FormResizeHandlePrivate

void
FormResizeHandlePrivate::init()
{
	FormMoveHandlePrivate::init();

	const qreal h = m_size * 0.6;

	m_path.moveTo( 0, -m_size );
	m_path.lineTo( m_size, -m_size + h );
	m_path.lineTo( m_size - h, -m_size + h );
	m_path.lineTo( m_size - h, m_size - h );
	m_path.lineTo( m_size, m_size - h );
	m_path.lineTo( 0, m_size );
	m_path.lineTo( -m_size, m_size - h );
	m_path.lineTo( -m_size + h, m_size - h );
	m_path.lineTo( -m_size + h, -m_size + h );
	m_path.lineTo( -m_size, -m_size + h );
	m_path.lineTo( 0, -m_size );
}


//
// FormResizeHandle
//

FormResizeHandle::FormResizeHandle( qreal halfSize, const QPointF & zero,
	qreal angle, FormWithHandle * object, QGraphicsItem * parent,
	const QCursor & c )
	:	FormMoveHandle( QScopedPointer< FormMoveHandlePrivate > (
			new FormResizeHandlePrivate( halfSize, zero, angle,
				object, this, c ) ), parent )
{
}

FormResizeHandle::~FormResizeHandle()
{
}

void
FormResizeHandle::paint( QPainter * painter,
	const QStyleOptionGraphicsItem * option,
	QWidget * widget )
{
	Q_UNUSED( widget )
	Q_UNUSED( option )

	FormResizeHandlePrivate * dd = d_func();

	painter->translate( dd->m_size, dd->m_size );
	painter->rotate( dd->m_angle );

	if( dd->m_hovered )
		painter->setBrush( Qt::red );
	else
		painter->setBrush( Qt::white );

	painter->setPen( Qt::black );

	painter->drawPath( dd->m_path );
}

} /* namespace Core */

} /* namespace Prototyper */
