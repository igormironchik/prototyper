
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

#ifndef PROTOTYPER__CORE__WITH_RESIZE_AND_MOVE_HANDLES_HPP__INCLUDED
#define PROTOTYPER__CORE__WITH_RESIZE_AND_MOVE_HANDLES_HPP__INCLUDED

// Qt include.
#include <QtGlobal>

QT_BEGIN_NAMESPACE
class QGraphicsItem;
QT_END_NAMESPACE


namespace Prototyper {

namespace Core {

class FormObject;
class FormMoveHandle;
class FormResizeHandle;

//
// WithResizeAndMoveHandles
//

//! Storage of resize and move handles.
class WithResizeAndMoveHandles {
public:
	WithResizeAndMoveHandles( FormObject * object );
	~WithResizeAndMoveHandles();

	//! Place handles.
	void place( const QRectF & r );
	//! Show handles.
	void show();
	//! Hide handles.
	void hide();

	//! Top-left resize.
	FormResizeHandle * m_topLeft;
	//! Top resize.
	FormResizeHandle * m_top;
	//! Top-right resize.
	FormResizeHandle * m_topRight;
	//! Left resize.
	FormResizeHandle * m_left;
	//! Left-bottom resize.
	FormResizeHandle * m_bottomLeft;
	//! Bottom resize.
	FormResizeHandle * m_bottom;
	//! Right-bottom resize.
	FormResizeHandle * m_bottomRight;
	//! Right resize.
	FormResizeHandle * m_right;
	//! Move handle.
	FormMoveHandle * m_move;
	//! Parent.
	QGraphicsItem * q;
}; // class WithResizeAndMoveHandles

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__WITH_RESIZE_AND_MOVE_HANDLES_HPP__INCLUDED
