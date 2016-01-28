
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
#include "tools_window.hpp"
#include "tools_widget.hpp"
#include "top_gui.hpp"
#include "project_window.hpp"

// Qt include.
#include <QCloseEvent>


namespace Prototyper {

namespace Core {

//
// ToolsWindowPrivate
//

class ToolsWindowPrivate {
public:
	ToolsWindowPrivate( ToolsWindow * parent )
		:	q( parent )
		,	m_widget( 0 )
	{
	}

	//! Init.
	void init();

	//! Parent.
	ToolsWindow * q;
	//! Central widget.
	ToolsWidget * m_widget;
}; // class ToolsWindowPrivate

void
ToolsWindowPrivate::init()
{
	m_widget = new ToolsWidget( q );

	q->setCentralWidget( m_widget );

	q->setWindowTitle( ToolsWindow::tr( "Tools" ) );
}


//
// ToolsWindow
//

ToolsWindow::ToolsWindow( QWidget * parent, Qt::WindowFlags f )
	:	QMainWindow( parent, f )
	,	d( new ToolsWindowPrivate( this ) )
{
	d->init();
}

ToolsWindow::~ToolsWindow()
{
}

void
ToolsWindow::closeEvent( QCloseEvent * e )
{
	e->ignore();

	TopGui::instance()->projectWindow()->hideToolsWindow();
}

} /* namespace Core */

} /* namespace Prototyper */
