
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
#include "top_gui.hpp"
#include "project_window.hpp"
#include "props_window.hpp"
#include "tools_window.hpp"

// Qt include.
#include <QApplication>
#include <QDesktopWidget>


namespace Prototyper {

namespace Core {

//
// TopGuiPrivate
//

class TopGuiPrivate {
public:
	TopGuiPrivate( TopGui * parent )
		:	q( parent )
		,	m_projectWindow( 0 )
		,	m_propsWindow( 0 )
		,	m_toolsWindow( 0 )
	{
	}

	~TopGuiPrivate()
	{
		delete m_projectWindow;
		delete m_propsWindow;
		delete m_toolsWindow;
	}

	//! Init.
	void init();
	//! Place windows by default.
	void placeByDefault();

	//! Parent.
	TopGui * q;
	//! Project window.
	ProjectWindow * m_projectWindow;
	//! Properties window.
	PropsWindow * m_propsWindow;
	//! Tools window.
	ToolsWindow * m_toolsWindow;
}; // class TopGuiPrivate

void
TopGuiPrivate::init()
{
	m_projectWindow = new ProjectWindow;
	m_propsWindow = new PropsWindow;
	m_toolsWindow = new ToolsWindow;

	placeByDefault();
}

void
TopGuiPrivate::placeByDefault()
{
	const QRect r = QApplication::desktop()->availableGeometry();

	const int fw = m_toolsWindow->geometry().x() - m_toolsWindow->x();
	int hh = m_toolsWindow->geometry().y() - m_toolsWindow->y();
	const int offset = 10;
	const int w25 = r.width() / 4;

	if( !hh ) hh = offset * 4;

	const int tw = w25 - offset * 2 - fw * 2;
	const int h = r.height() - offset * 2 - hh - fw;

	m_toolsWindow->move( r.x() + offset, r.y() + offset );
	m_toolsWindow->resize( tw, h );

	m_projectWindow->move( r.x() + w25 + offset, r.y() + offset );
	m_projectWindow->resize( tw + w25, h );

	m_propsWindow->move( r.x() + w25 * 3 + offset, r.y() + offset );
	m_propsWindow->resize( tw, h );
}


//
// TopGui
//

TopGui::TopGui()
	:	d( new TopGuiPrivate( this ) )
{
	d->init();
}

TopGui::~TopGui()
{
}

static TopGui * topGuiInstance;

void
TopGui::cleanup()
{
	if( topGuiInstance )
	{
		delete topGuiInstance;

		topGuiInstance = 0;
	}
}

TopGui *
TopGui::instance()
{
	if( !topGuiInstance )
	{
		topGuiInstance = new TopGui;

		qAddPostRoutine( TopGui::cleanup );
	}

	return topGuiInstance;
}

ProjectWindow *
TopGui::projectWindow()
{
	return d->m_projectWindow;
}

ToolsWindow *
TopGui::toolsWindow()
{
	return d->m_toolsWindow;
}

PropsWindow *
TopGui::propsWindow()
{
	return d->m_propsWindow;
}

} /* namespace Core */

} /* namespace Prototyper */
