
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
#include "form_object.hpp"


namespace Prototyper {

namespace Core {

//
// FormObject
//

FormObject::FormObject()
	:	m_mode( Normal )
	,	m_z( 0 )
	,	m_id( 0 )
{
}

FormObject::~FormObject()
{
}

FormObject::FormObject( const FormObject & other )
	:	m_mode( other.mode() )
	,	m_z( other.zValue() )
	,	m_id( other.id() )
{
}

FormObject &
FormObject::operator = ( const FormObject & other )
{
	if( this != &other )
	{
		m_mode = other.mode();
		m_z = other.zValue();
		m_id = other.id();
	}

	return *this;
}

quint64
FormObject::id()const
{
	return m_id;
}

void
FormObject::setId( quint64 i )
{
	m_id = i;
}

FormObject::Mode
FormObject::mode() const
{
	return m_mode;
}

void
FormObject::setMode( Mode m )
{
	m_mode = m;
}

quint64
FormObject::zValue() const
{
	return m_z;
}

void
FormObject::setZValue( quint64 z )
{
	m_z = z;
}

} /* namespace Core */

} /* namespace Prototyper */
