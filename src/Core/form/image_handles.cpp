
/*
	SPDX-FileCopyrightText: 2016-2024 Igor Mironchik <igor.mironchik@gmail.com>
	SPDX-License-Identifier: GPL-3.0-or-later
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
