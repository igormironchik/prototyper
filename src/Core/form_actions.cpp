
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
#include "form_actions.hpp"
#include "page.hpp"

// Qt include.
#include <QApplication>
#include <QColor>


namespace Prototyper {

namespace Core {

//
// PageActionPrivate
//

class PageActionPrivate {
public:
	explicit PageActionPrivate( PageAction * parent )
		:	q( parent )
		,	m_mode( PageAction::Select )
		,	m_page( 0 )
		,	m_fillColor( Qt::transparent )
		,	m_strokeColor( Qt::black )
		,	m_snap( true )
		,	m_flags( 0 )
	{
	}

	//! Parent.
	PageAction * q;
	//! Mode.
	PageAction::Mode m_mode;
	//! Page.
	Page * m_page;
	//! Fill color.
	QColor m_fillColor;
	//! Stroke color.
	QColor m_strokeColor;
	//! Is snap enabled?
	bool m_snap;
	//! Flags.
	PageAction::Flags m_flags;
}; // class FormActionPrivate


//
// PageAction
//

PageAction::PageAction()
	:	d( new PageActionPrivate( this ) )
{
}

PageAction::~PageAction()
{
}

static PageAction * pageActionInstance = 0;

void
PageAction::cleanup()
{
	delete pageActionInstance;

	pageActionInstance = 0;
}

PageAction *
PageAction::instance()
{
	if( !pageActionInstance )
	{
		pageActionInstance = new PageAction;

		qAddPostRoutine( &PageAction::cleanup );
	}

	return pageActionInstance;
}

const PageAction::Flags &
PageAction::flags() const
{
	return d->m_flags;
}

void
PageAction::setFlags( const Flags & f )
{
	d->m_flags = f;
}

void
PageAction::setFlag( Flag f )
{
	d->m_flags |= f;
}

bool
PageAction::testFlag( Flag f ) const
{
	return d->m_flags.testFlag( f );
}

void
PageAction::clearFlag( Flag f )
{
	d->m_flags &= ~Flags( f );
}

PageAction::Mode
PageAction::mode() const
{
	return d->m_mode;
}

void
PageAction::setMode( Mode m )
{
	d->m_mode = m;
}

Page *
PageAction::page() const
{
	return d->m_page;
}

void
PageAction::setPage( Page * f )
{
	d->m_page = f;
}

const QColor &
PageAction::fillColor() const
{
	return d->m_fillColor;
}

void
PageAction::setFillColor( const QColor & c )
{
	d->m_fillColor = c;
}

const QColor &
PageAction::strokeColor() const
{
	return d->m_strokeColor;
}

void
PageAction::setStrokeColor( const QColor & c )
{
	d->m_strokeColor = c;
}

bool
PageAction::isSnapEnabled() const
{
	return d->m_snap;
}

void
PageAction::enableSnap( bool on )
{
	d->m_snap = on;
}

} /* namespace Core */

} /*  namespace Prototyper */
