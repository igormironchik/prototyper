
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


namespace Prototyper {

namespace Core {

//
// FormGroupPrivate
//

class FormGroupPrivate {
public:
	FormGroupPrivate( FormGroup * parent )
		:	q( parent )
	{
	}

	//! Init.
	void init();

	//! Parent.
	FormGroup * q;
}; // class FormGroupPrivate

void
FormGroupPrivate::init()
{
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

} /* namespace Core */

} /* namespace Prototyper */
