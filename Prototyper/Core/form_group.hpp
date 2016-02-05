
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

// Qt include.
#include <QGraphicsItemGroup>
#include <QScopedPointer>

// Prototyper include.
#include "form_object.hpp"


namespace Prototyper {

namespace Core {

//
// FormGroup
//

class FormGroupPrivate;

class FormGroup
	:	public QGraphicsItemGroup
	,	public FormObject
{
public:
	FormGroup( QGraphicsItem * parent = 0 );
	~FormGroup();

private:
	Q_DISABLE_COPY( FormGroup )

	QScopedPointer< FormGroupPrivate > d;
}; // class FormGroup

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__FORM_GROUP_HPP__INCLUDED