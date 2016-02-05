
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
#include "form_line_move_handle.hpp"
#include "form_line.hpp"


namespace Prototyper {

namespace Core {

//
// FormLineMoveHandlePrivate
//

class FormLineMoveHandlePrivate {
public:
	FormLineMoveHandlePrivate( FormLine * line, FormLineMoveHandle * parent )
		:	q( parent )
		,	m_line( line )
	{
	}

	//! Init.
	void init();

	//! Parent.
	FormLineMoveHandle * q;
	//! Line.
	FormLine * m_line;
}; // class FormLineMoveHandlePrivate

void
FormLineMoveHandlePrivate::init()
{

}


//
// FormLineMoveHandle
//

FormLineMoveHandle::FormLineMoveHandle( FormLine * line, QGraphicsItem * parent )
	:	FormMoveHandle( parent )
	,	d( new FormLineMoveHandlePrivate( line, this ) )
{
	d->init();
}

FormLineMoveHandle::~FormLineMoveHandle()
{

}

void
FormLineMoveHandle::moved( const QPointF & delta )
{
	d->m_line->handleMoved( delta, this );
}

} /* namespace Core */

} /* namespace Prototyper */
