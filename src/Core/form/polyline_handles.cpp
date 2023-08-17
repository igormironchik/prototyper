
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
#include "polyline_handles.hpp"
#include "resizable_private.hpp"
#include "../constants.hpp"


namespace Prototyper {

namespace Core {

//
// FormPolylineHandlesPrivate
//

class FormPolylineHandlesPrivate
	:	public FormResizableProxyPrivate
{
public:
	FormPolylineHandlesPrivate( FormResizable * resizable,
		FormPolylineHandles * parent, Page * form )
		:	FormResizableProxyPrivate( resizable, parent, form )
	{
	}

	~FormPolylineHandlesPrivate() override = default;

	//! Init.
	void init() Q_DECL_OVERRIDE;
	//! Place handles.
	void place( const QRectF & rect ) Q_DECL_OVERRIDE;

	inline const FormPolylineHandles * q_ptr() const
		{ return static_cast< const FormPolylineHandles* > ( q ); }
	inline FormPolylineHandles * q_ptr()
		{ return static_cast< FormPolylineHandles* > ( q ); }

	//! Aspect ratio handle.
	std::unique_ptr< NodesEditResizeHandle > m_modeHandle;
}; // class FormPolylineHandlesPrivate

void
FormPolylineHandlesPrivate::init()
{
	FormResizableProxyPrivate::init();

	m_modeHandle.reset( new NodesEditResizeHandle( q ) );

	FormPolylineHandles::connect( m_modeHandle.get(), &NodesEditResizeHandle::currentModeChanged,
		q_ptr(), &FormPolylineHandles::currentModeChanged );
}

void
FormPolylineHandlesPrivate::place( const QRectF & rect )
{
	FormResizableProxyPrivate::place( rect );

	m_modeHandle->setPos( rect.x() - c_halfHandleSize * c_halfDivider * c_halfDivider,
		rect.y() - c_halfHandleSize * c_halfDivider * c_halfDivider );

	q->parentItem()->update(
		rect.adjusted( -c_halfHandleSize * c_halfDivider * c_halfDivider * c_halfDivider,
		-c_halfHandleSize * c_halfDivider * c_halfDivider * c_halfDivider,
		c_halfHandleSize * c_halfDivider * c_halfDivider * c_halfDivider,
		c_halfHandleSize * c_halfDivider * c_halfDivider * c_halfDivider ) );
}


//
// FormPolylineHandles
//

FormPolylineHandles::FormPolylineHandles( FormResizable * resizable,
	QGraphicsItem * parent, Page * form )
	:	FormResizableProxy( std::make_unique< FormPolylineHandlesPrivate >( resizable, this, form ),
			parent )
{
}

FormPolylineHandles::~FormPolylineHandles() = default;

NodesEditResizeHandle::Mode
FormPolylineHandles::currentMode() const
{
	return d_ptr()->m_modeHandle->currentMode();
}

void
FormPolylineHandles::setCurrentMode( NodesEditResizeHandle::Mode m )
{
	d_ptr()->m_modeHandle->setCurrentMode( m );
}

QRectF
FormPolylineHandles::boundingRect() const
{
	return FormResizableProxy::boundingRect()
		.adjusted( -c_halfHandleSize * c_halfDivider * c_halfDivider,
			-c_halfHandleSize * c_halfDivider * c_halfDivider,
			0.0, 0.0 );
}

void
FormPolylineHandles::setHandlesVisible( bool on )
{
	if( on )
		d->m_handles->show();
	else
		d->m_handles->hide();
}

} /* namespace Core */

} /* namespace Prototyper */
