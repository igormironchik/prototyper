
/*!
	\file

	\author Igor Mironchik (igor.mironchik at gmail dot com).

	Copyright (c) 2016-2021 Igor Mironchik

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

#ifndef PROTOTYPER__CORE__FORM_RESIZE_HANDLE_HPP__INCLUDED
#define PROTOTYPER__CORE__FORM_RESIZE_HANDLE_HPP__INCLUDED

// Prototyper include.
#include "move_handle.hpp"


namespace Prototyper {

namespace Core {

//
// FormResizeHandle
//

class FormResizeHandlePrivate;

//! rESIZE handler on the form.
class FormResizeHandle
	:	public FormMoveHandle
{
public:
	FormResizeHandle( qreal halfSize, const QPointF & zero,
		qreal angle, FormWithHandle * object, QGraphicsItem * parent,
		Page * form, const QCursor & c = Qt::SizeAllCursor );
	~FormResizeHandle() override;

	void paint( QPainter * painter, const QStyleOptionGraphicsItem * option,
		QWidget * widget = 0 ) override;

private:
	Q_DISABLE_COPY( FormResizeHandle )

	inline FormResizeHandlePrivate * d_func()
		{ return reinterpret_cast< FormResizeHandlePrivate* > ( d.get() ); }
	inline const FormResizeHandlePrivate * d_func() const
		{ return reinterpret_cast< const FormResizeHandlePrivate* >( d.get() ); }
}; // class FormResizeHandler

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__FORM_RESIZE_HANDLE_HPP__INCLUDED
