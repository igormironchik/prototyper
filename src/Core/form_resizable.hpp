
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

#ifndef PROTOTYPER__CORE__FORM_RESIZABLE_HPP__INCLUDED
#define PROTOTYPER__CORE__FORM_RESIZABLE_HPP__INCLUDED

// Qt include.
#include <QGraphicsObject>

// C++ include.
#include <memory>

// Prototyper include.
#include "form_move_handle.hpp"
#include "form_with_resize_and_move_handles.hpp"


namespace Prototyper {

namespace Core {

//
// FormResizable
//

//! Base class for resizable objects.
class FormResizable {
public:
	FormResizable();
	virtual ~FormResizable();

protected:
	friend class FormResizableProxy;

	//! Resize.
	virtual void resize( const QRectF & rect );
	//! Move resizable.
	virtual void moveResizable( const QPointF & delta );

private:
	Q_DISABLE_COPY( FormResizable )
}; // class FormResizable


class FormResizableProxy;
class FormResizableProxyPrivate;


//
// FormResizableProxy
//

//! Proxy for resizable object. This proxy has resize and move handles
//! and on resize draw new rectangle but don't resize actual FormResizable.
//! And only on release do actual resize.
class FormResizableProxy
	:	public QGraphicsObject
	,	public FormWithHandle
{
public:
	FormResizableProxy( FormResizable * resizable,
		QGraphicsItem * parent, Page * form );
	~FormResizableProxy() override;

	//! Set rect.
	void setRect( const QRectF & rect );

	//! Set min size.
	void setMinSize( const QSizeF & min );

	QRectF boundingRect() const override;

	void paint( QPainter * painter, const QStyleOptionGraphicsItem * option,
		QWidget * widget = 0 ) override;

	//! \return Pointer to handles.
	WithResizeAndMoveHandles * handles() const;

protected:
	FormResizableProxy( std::unique_ptr< FormResizableProxyPrivate > && dd,
		QGraphicsItem * parent );

protected:
	//! Handle moved.
	void handleMoved( const QPointF & delta, FormMoveHandle * handle ) override;
	//! Handle released.
	void handleReleased( FormMoveHandle * handle ) override;

protected:
	std::unique_ptr< FormResizableProxyPrivate > d;

private:
	Q_DISABLE_COPY( FormResizableProxy )
}; // class FormResizableProxy

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__FORM_RESIZABLE_HPP__INCLUDED
