
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

#ifndef PROTOTYPER__CORE__FORM_POLYLINE_HANDLES_HPP__INCLUDED
#define PROTOTYPER__CORE__FORM_POLYLINE_HANDLES_HPP__INCLUDED

// Prototyper include.
#include "resizable.hpp"
#include "nodes_edit_resize_handle.hpp"


namespace Prototyper {

namespace Core {

//
// FormPolylineHandles
//

class FormPolylineHandlesPrivate;

//! Polyline handles.
class FormPolylineHandles final
	:	public FormResizableProxy
{
	Q_OBJECT

signals:
	//! Current mode changed.
	void currentModeChanged();

public:
	FormPolylineHandles( FormResizable * resizable,
		QGraphicsItem * parent, Page * form );
	~FormPolylineHandles() override;

	//! \return Current mode.
	NodesEditResizeHandle::Mode currentMode() const;
	//! Set mode.
	void setCurrentMode( NodesEditResizeHandle::Mode m );

	QRectF boundingRect() const override;

public slots:
	//! Hide/show resize handles.
	void setHandlesVisible( bool on = true );

private:
	inline const FormPolylineHandlesPrivate * d_ptr() const
		{ return reinterpret_cast< const FormPolylineHandlesPrivate* > ( d.get() ); }
	inline FormPolylineHandlesPrivate * d_ptr()
		{ return reinterpret_cast< FormPolylineHandlesPrivate* > ( d.get() ); }

private:
	Q_DISABLE_COPY( FormPolylineHandles )
}; // class FormPolylineHandles

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__FORM_POLYLINE_HANDLES_HPP__INCLUDED
