
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


namespace Prototyper {

namespace Core {

//
// FormResizeHandlePrivate
//

class FormResizeHandlePrivate
	:	public FormMoveHandlePrivate
{
public:
	FormResizeHandlePrivate( FormObject * object, FormResizeHandle * parent )
		:	FormMoveHandlePrivate( object, parent )
	{
	}

	//! Init.
	void init() Q_DECL_OVERRIDE;

	inline FormResizeHandle * q_func()
		{ return static_cast< FormResizeHandle* >( q ); }
	inline const FormResizeHandle * q_func() const
		{ return static_cast< const FormResizeHandle* >( q ); }
}; // class FormResizeHandlePrivate

void
FormResizeHandlePrivate::init()
{
	FormMoveHandlePrivate::init();
}


//
// FormResizeHandle
//

FormResizeHandle::FormResizeHandle( FormObject * object,
	QGraphicsItem * parent )
	:	FormMoveHandle( new FormResizeHandlePrivate( object, this ), parent )
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
}

} /* namespace Core */

} /* namespace Prototyper */
