
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

#ifndef PROTOTYPER__CORE__FORM_GROUP_HPP__INCLUDED
#define PROTOTYPER__CORE__FORM_GROUP_HPP__INCLUDED

// Prototyper include.
#include "form_object.hpp"

// Qt include.
#include <QList>
#include <QSharedPointer>


namespace Prototyper {

namespace Core {

//
// FormGroup
//

//! Group of objects on the form.
class FormGroup
	:	public FormObject
{
public:
	FormGroup();
	~FormGroup();

	FormGroup( const FormGroup & other );
	FormGroup & operator = ( const FormGroup & other );

	//! \return List of all objects in the group.
	const QList< QSharedPointer< FormObject > > & objects() const;
	//! \return List of all objects in the group.
	QList< QSharedPointer< FormObject > > & objects();

	//! Set mode.
	void setMode( Mode m ) Q_DECL_OVERRIDE;

	//! \return Z-order.
	quint64 zValue() const Q_DECL_OVERRIDE;
	//! Set Z-order.
	void setZValue( quint64 z ) Q_DECL_OVERRIDE;

	//! \return Bounding rect.
	QRect boundingRect() const Q_DECL_OVERRIDE;

	//! Paint object.
	void paint( QPainter * p ) const Q_DECL_OVERRIDE;

private:
	QList< QSharedPointer< FormObject > > m_objects;
}; // class FormGroup

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__FORM_GROUP_HPP__INCLUDED
