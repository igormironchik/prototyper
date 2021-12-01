
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

#ifndef PROTOTYPER__CORE__FORM_MOVE_HANDLER_PRIVATE_HPP__INCLUDED
#define PROTOTYPER__CORE__FORM_MOVE_HANDLER_PRIVATE_HPP__INCLUDED

// Qt include.
#include <QGraphicsObject>
#include <QCursor>


namespace Prototyper {

namespace Core {

class FormMoveHandle;
class Page;
class FormWithHandle;


//
// FormMoveHandlePrivate
//

class FormMoveHandlePrivate {
public:
	FormMoveHandlePrivate( qreal halfSize, const QPointF & zero,
		FormWithHandle * object, FormMoveHandle * parent, Page * form,
		const QCursor & c );
	virtual ~FormMoveHandlePrivate();

	//! Init.
	virtual void init();

	//! Parent.
	FormMoveHandle * q;
	//! Object.
	FormWithHandle * m_object;
	//! Half of the size.
	qreal m_size;
	//! Hovered.
	bool m_hovered;
	//! Pressed.
	bool m_pressed;
	//! Mouse point.
	QPointF m_pos;
	//! Ignore mouse events?
	bool m_ignoreMouse;
	//! Zero point.
	QPointF m_zero;
	//! Touch delta.
	QPointF m_touchDelta;
	//! Cursor.
	QCursor m_cursor;
	//! Form.
	Page * m_form;
}; // class FormMoveHandlerPrivate

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__FORM_MOVE_HANDLER_PRIVATE_HPP__INCLUDED
