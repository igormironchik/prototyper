
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
#include "form_image_handles.hpp"
#include "form_aspect_ratio_handle.hpp"


namespace Prototyper {

namespace Core {

//
// FormImageHandlesPrivate
//

class FormImageHandlesPrivate
	:	public FormResizableProxyPrivate
{
public:
	FormImageHandlesPrivate( FormResizable * resizable,
		FormImageHandles * parent )
		:	FormResizableProxyPrivate( resizable, parent )
	{
	}

	~FormImageHandlesPrivate()
	{
	}

	//! Init.
	void init() Q_DECL_OVERRIDE;
	//! Place handles.
	void place( const QRectF & rect ) Q_DECL_OVERRIDE;

	//! Aspect ratio handle.
	AspectRatioHandle * m_aspectRatioHandle;
}; // class FormImageHandlesPrivate

void
FormImageHandlesPrivate::init()
{
	FormResizableProxyPrivate::init();

	m_aspectRatioHandle = new AspectRatioHandle( q );
}

void
FormImageHandlesPrivate::place( const QRectF & rect )
{
	FormResizableProxyPrivate::place( rect );

	m_aspectRatioHandle->setPos( m_handles.m_move->pos() +
		QPointF( m_handles.m_move->halfOfSize() * 2.0 + 6.0, 0.0 ) );
}


//
// FormImageHandles
//

FormImageHandles::FormImageHandles( FormResizable * resizable,
	QGraphicsItem * parent )
	:	FormResizableProxy( QScopedPointer< FormResizableProxyPrivate > (
			new FormImageHandlesPrivate( resizable, this ) ), parent )
{
}

FormImageHandles::~FormImageHandles()
{
}

bool
FormImageHandles::isKeepAspectRatio() const
{
	return d_ptr()->m_aspectRatioHandle->isKeepAspectRatio();
}

} /* namespace Core */

} /* namespace Prototyper */
