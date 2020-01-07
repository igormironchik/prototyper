
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
#include "project_widget_tab_bar.hpp"

// Qt include.
#include <QContextMenuEvent>
#include <QMenu>


namespace Prototyper {

namespace Core {

//
// ProjectTabBarPrivate
//

class ProjectTabBarPrivate {
public:
	explicit ProjectTabBarPrivate( ProjectTabBar * parent )
		:	q( parent )
		,	m_menuTabIndex( 0 )
	{
	}

	//! Init.
	void init();

	//! Parent.
	ProjectTabBar * q;
	//! Menu tab index.
	int m_menuTabIndex;
}; // class ProjectTabBarPrivate

void
ProjectTabBarPrivate::init()
{
}


//
// ProjectTabBar
//

ProjectTabBar::ProjectTabBar( QWidget * parent )
	:	QTabBar( parent )
	,	d( new ProjectTabBarPrivate( this ) )
{
	d->init();
}

ProjectTabBar::~ProjectTabBar()
{
}

void
ProjectTabBar::renameTab()
{
	emit formRenameRequest( tabText( d->m_menuTabIndex ) );
}

void
ProjectTabBar::addPage()
{
	emit formAddRequest();
}

void
ProjectTabBar::deleteForm()
{
	emit formDeleteRequest( tabText( d->m_menuTabIndex ) );
}

void
ProjectTabBar::contextMenuEvent( QContextMenuEvent * event )
{
	d->m_menuTabIndex = tabAt( event->pos() );

	QMenu menu;

	if( d->m_menuTabIndex >= 0 )
		menu.addAction( QIcon( QStringLiteral( ":/Core/img/edit-rename.png" ) ),
			tr( "Rename" ), this, &ProjectTabBar::renameTab );

	menu.addAction( QIcon( QStringLiteral( ":/Core/img/list-add.png" ) ),
		tr( "Add Page" ), this, &ProjectTabBar::addPage );

	if( d->m_menuTabIndex != 0 )
		menu.addAction( QIcon( QStringLiteral( ":/Core/img/edit-delete.png" ) ),
			tr( "Delete Form" ), this, &ProjectTabBar::deleteForm );

	menu.exec( event->globalPos() );

	event->accept();
}

} /* namespace Core */

} /* namespace Prototyper */
