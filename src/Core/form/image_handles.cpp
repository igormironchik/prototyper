
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

// Prototyper include.
#include "image_handles.hpp"
#include "aspect_ratio_handle.hpp"
#include "resizable_private.hpp"
#include "../constants.hpp"


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
		FormImageHandles * parent, Page * form )
		:	FormResizableProxyPrivate( resizable, parent, form )
	{
	}

	~FormImageHandlesPrivate() override = default;

	//! Init.
	void init() Q_DECL_OVERRIDE;
	//! Place handles.
	void place( const QRectF & rect ) Q_DECL_OVERRIDE;

	//! Aspect ratio handle.
	std::unique_ptr< AspectRatioHandle > m_aspectRatioHandle;
}; // class FormImageHandlesPrivate

void
FormImageHandlesPrivate::init()
{
	FormResizableProxyPrivate::init();

	m_aspectRatioHandle.reset( new AspectRatioHandle( q ) );
}

void
FormImageHandlesPrivate::place( const QRectF & rect )
{
	FormResizableProxyPrivate::place( rect );

	m_aspectRatioHandle->setPos( rect.x() - c_halfHandleSize * c_halfDivider * c_halfDivider,
		rect.y() - c_halfHandleSize * c_halfDivider * c_halfDivider );

	q->parentItem()->update(
		rect.adjusted( -c_halfHandleSize * c_halfDivider * c_halfDivider * c_halfDivider,
		-c_halfHandleSize * c_halfDivider * c_halfDivider * c_halfDivider,
		c_halfHandleSize * c_halfDivider * c_halfDivider * c_halfDivider,
		c_halfHandleSize * c_halfDivider * c_halfDivider * c_halfDivider ) );
}


//
// FormImageHandles
//

FormImageHandles::FormImageHandles( FormResizable * resizable,
	QGraphicsItem * parent, Page * form )
	:	FormResizableProxy( std::make_unique< FormImageHandlesPrivate >( resizable, this, form ),
			parent )
{
}

FormImageHandles::~FormImageHandles() = default;

bool
FormImageHandles::isKeepAspectRatio() const
{
	return d_ptr()->m_aspectRatioHandle->isKeepAspectRatio();
}

void
FormImageHandles::setKeepAspectRatio( bool on )
{
	d_ptr()->m_aspectRatioHandle->setKeepAspectRatio( on );
}

QRectF
FormImageHandles::boundingRect() const
{
	return FormResizableProxy::boundingRect()
		.adjusted( -c_halfHandleSize * c_halfDivider * c_halfDivider,
			-c_halfHandleSize * c_halfDivider * c_halfDivider,
			0.0, 0.0 );
}

} /* namespace Core */

} /* namespace Prototyper */
