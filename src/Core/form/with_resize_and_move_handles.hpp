
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

#ifndef PROTOTYPER__CORE__FORM_WITH_RESIZE_AND_MOVE_HANDLES_HPP__INCLUDED
#define PROTOTYPER__CORE__FORM_WITH_RESIZE_AND_MOVE_HANDLES_HPP__INCLUDED

// Qt include.
#include <QtGlobal>
#include <QSizeF>

// C++ include.
#include <memory>

QT_BEGIN_NAMESPACE
class QGraphicsItem;
class QSizeF;
class QRectF;
QT_END_NAMESPACE


namespace Prototyper {

namespace Core {

class FormWithHandle;
class FormMoveHandle;
class FormResizeHandle;
class Page;

//
// WithResizeAndMoveHandles
//

//! Storage of resize and move handles.
class WithResizeAndMoveHandles final {
public:
	WithResizeAndMoveHandles( FormWithHandle * object,
		QGraphicsItem * parent, Page * form );
	~WithResizeAndMoveHandles();

	//! Place handles.
	void place( const QRectF & r );
	//! Show handles.
	void show();
	//! Hide handles.
	void hide();
	//! Check constraint.
	bool checkConstraint( const QSizeF & s );
	//! Set min size.
	void setMinSize( const QSizeF & min );

	//! Top-left resize.
	std::unique_ptr< FormResizeHandle > m_topLeft;
	//! Top resize.
	std::unique_ptr< FormResizeHandle > m_top;
	//! Top-right resize.
	std::unique_ptr< FormResizeHandle > m_topRight;
	//! Left resize.
	std::unique_ptr< FormResizeHandle > m_left;
	//! Left-bottom resize.
	std::unique_ptr< FormResizeHandle > m_bottomLeft;
	//! Bottom resize.
	std::unique_ptr< FormResizeHandle > m_bottom;
	//! Right-bottom resize.
	std::unique_ptr< FormResizeHandle > m_bottomRight;
	//! Right resize.
	std::unique_ptr< FormResizeHandle > m_right;
	//! Move handle.
	std::unique_ptr< FormMoveHandle > m_move;
	//! Parent.
	QGraphicsItem * q;
	//! Min size.
	QSizeF m_min;
}; // class WithResizeAndMoveHandles

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__FORM_WITH_RESIZE_AND_MOVE_HANDLES_HPP__INCLUDED
