
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

// C++ include.
#include <algorithm>


namespace Prototyper {

namespace Core {

//
// FormGroup
//

FormGroup::FormGroup()
{
}

FormGroup::~FormGroup()
{
}

FormGroup::FormGroup( const FormGroup & other )
	:	FormObject( other )
	,	m_objects( other.objects() )
{
}

FormGroup &
FormGroup::operator = ( const FormGroup & other )
{
	if( this != &other )
	{
		FormObject::operator = ( other );

		m_objects = other.objects();
	}

	return *this;
}

const QList< QSharedPointer< FormObject > > &
FormGroup::objects() const
{
	return m_objects;
}

QList< QSharedPointer< FormObject > > &
FormGroup::objects()
{
	return m_objects;
}

void
FormGroup::setMode( Mode m )
{
	FormObject::setMode( m );

	std::for_each( m_objects.begin(), m_objects.end(),
		[&] ( QSharedPointer< FormObject > & p ) { p->setMode( m ); } );
}

quint64
FormGroup::zValue() const
{
	QList< QSharedPointer< FormObject > >::ConstIterator it =
		std::min_element( m_objects.constBegin(), m_objects.constEnd(),
			[] ( const QSharedPointer< FormObject > & p1,
				const QSharedPointer< FormObject > & p2 )
					{ return p1->zValue() < p2->zValue(); } );

	if( it != m_objects.constEnd() && (*it)->zValue() < FormObject::zValue() )
		return (*it)->zValue();
	else
		return FormObject::zValue();
}

void
FormGroup::setZValue( quint64 z )
{
	const quint64 delta = z - zValue();

	std::for_each( m_objects.begin(), m_objects.end(),
		[&] ( QSharedPointer< FormObject > & p )
			{ p->setZValue( p->zValue() + delta ); } );
}

QRect
FormGroup::boundingRect() const
{
	QRect res;

	std::for_each( m_objects.constBegin(), m_objects.constEnd(),
		[&] ( const QSharedPointer< FormObject > & p )
			{ res = res.united( p->boundingRect() ); } );

	return res;
}

void
FormGroup::paint( QPainter * p ) const
{
	std::for_each( m_objects.constBegin(), m_objects.constEnd(),
		[&] ( const QSharedPointer< FormObject > & ptr )
			{ ptr->paint( p ); } );
}

} /* namespace Core */

} /* namespace Prototyper */
