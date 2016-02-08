
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
#include "form_group.hpp"
#include "form_move_handle.hpp"

// Qt include.
#include <QPainter>
#include <QStyleOptionGraphicsItem>


namespace Prototyper {

namespace Core {

//
// FormGroupPrivate
//

class FormGroupPrivate {
public:
	FormGroupPrivate( FormGroup * parent )
		:	q( parent )
		,	m_handle( 0 )
	{
	}

	//! Init.
	void init();

	//! Parent.
	FormGroup * q;
	//! Handles.
	FormMoveHandle * m_handle;
}; // class FormGroupPrivate

void
FormGroupPrivate::init()
{
	m_handle = new FormMoveHandle( 3.0, QPointF( 3.0, 3.0 ), q,
		q->parentItem() );
	m_handle->setZValue( 999 );
	m_handle->hide();
}


//
// FormGroup
//

FormGroup::FormGroup( QGraphicsItem * parent )
	:	QGraphicsItemGroup( parent )
	,	d( new FormGroupPrivate( this ) )
{
	d->init();
}

FormGroup::~FormGroup()
{
}

QRectF
FormGroup::boundingRect() const
{
	return QGraphicsItemGroup::boundingRect();
}

void
FormGroup::paint( QPainter * painter, const QStyleOptionGraphicsItem * option,
	QWidget * widget )
{
	Q_UNUSED( widget )

	if( isSelected() && !group() )
	{
		painter->setPen( QPen( Qt::gray, 1.0, Qt::DashLine ) );

		painter->setBrush( Qt::NoBrush );

		painter->drawRect( option->rect );

		const QPointF p( option->rect.x() + option->rect.width() / 2.0 -
				 d->m_handle->halfOfSize(),
			 option->rect.y() + option->rect.height() / 2.0 -
				 d->m_handle->halfOfSize() );

		d->m_handle->setPos( mapToParent( p ) );

		d->m_handle->show();
	}
	else
		d->m_handle->hide();
}

void
FormGroup::handleMoved( const QPointF & delta, FormMoveHandle * handle )
{
	if( handle == d->m_handle )
		moveBy( delta.x(), delta.y() );
}

} /* namespace Core */

} /* namespace Prototyper */
