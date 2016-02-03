
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
#include "tabs_list.hpp"

// Qt include.
#include <QListView>


namespace Prototyper {

namespace Core {

//
// TabsListPrivate
//

class TabsListPrivate {
public:
	TabsListPrivate( TabsList * parent )
		:	q( parent )
		,	m_view( 0 )
	{
	}

	//! Init.
	void init();

	//! Parent.
	TabsList * q;
	//! View.
	QListView * m_view;
}; // class TabsListPrivate

void
TabsListPrivate::init()
{
	m_view = new QListView( q );

	q->setWindowTitle( TabsList::tr( "Tabs" ) );

	q->setWidget( m_view );
}


//
// TabsList
//

TabsList::TabsList( QWidget * parent, Qt::WindowFlags f )
	:	QDockWidget( parent, f )
	,	d( new TabsListPrivate( this ) )
{
	d->init();
}

TabsList::~TabsList()
{
}

} /* namespace Core */

} /* namespace Prototyper */
