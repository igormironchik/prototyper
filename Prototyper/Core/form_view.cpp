
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
#include "form_view.hpp"


namespace Prototyper {

namespace Core {

//
// FormViewPrivate
//

class FormViewPrivate {
public:
	FormViewPrivate( FormView * parent )
		:	q( parent )
	{
	}

	//! Init.
	void init();

	//! Parent.
	FormView * q;
}; // class FormViewPrivate

void
FormViewPrivate::init()
{
}


//
// FormView
//

FormView::FormView( QWidget * parent )
	:	QGraphicsView( parent )
	,	d( new FormViewPrivate( this ) )
{
	d->init();
}

FormView::~FormView()
{
}

} /* namespace Core */

} /* namespace Prototyper */
