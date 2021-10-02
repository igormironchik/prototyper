
/*!
	\file

	\author Igor Mironchik (igor.mironchik at gmail dot com).

	Copyright (c) 2016-2020 Igor Mironchik

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

#ifndef PROTOTYPER__CORE__FORM_RESIZABLE_PRIVATE_HPP__INCLUDED
#define PROTOTYPER__CORE__FORM_RESIZABLE_PRIVATE_HPP__INCLUDED

// C++ include.
#include <memory>

// Prototyper include.
#include "form_move_handle.hpp"
#include "form_with_resize_and_move_handles.hpp"


namespace Prototyper {

namespace Core {

class FormResizable;

//
// FormResizableProxyPrivate
//

class FormResizableProxy;

class FormResizableProxyPrivate {
public:
	FormResizableProxyPrivate( FormResizable * resizable,
		FormResizableProxy * parent, Page * form );

	virtual ~FormResizableProxyPrivate();

	//! Init.
	virtual void init();
	//! Place handles.
	virtual void place( const QRectF & rect );

	//! Parent.
	FormResizableProxy * q;
	//! Object.
	FormResizable * m_object;
	//! Rect.
	QRectF m_rect;
	//! Handles.
	std::unique_ptr< WithResizeAndMoveHandles > m_handles;
	//! Form.
	Page * m_form;
	//! Subsidiary rect.
	QRectF m_subsidiaryRect;
	//! Is hadnle moved.
	bool m_handleMoved;
}; // class FormResizableProxyPrivate;

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__FORM_RESIZABLE_PRIVATE_HPP__INCLUDED
