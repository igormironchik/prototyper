
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
#include "top_gui.hpp"
#include "project_window.hpp"
#include "project_widget.hpp"

// Qt include.
#include <QListView>
#include <QStringListModel>
#include <QModelIndex>


namespace Prototyper {

namespace Core {

//
// TabsModel
//

//! Model for the tabs list.
class TabsModel
	:	public QStringListModel
{
public:
	TabsModel( QObject * parent )
		:	QStringListModel( parent )
	{
	}

	~TabsModel()
	{
	}

	Qt::ItemFlags flags( const QModelIndex & index ) const
		Q_DECL_OVERRIDE
	{
		if( index.isValid() )
			return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
		else
			return Qt::NoItemFlags;
	}
}; // class TabsModel


//
// TabsListPrivate
//

class TabsListPrivate {
public:
	TabsListPrivate( TabsList * parent )
		:	q( parent )
		,	m_view( 0 )
		,	m_model( 0 )
	{
	}

	//! Init.
	void init();

	//! Parent.
	TabsList * q;
	//! View.
	QListView * m_view;
	//! Model.
	TabsModel * m_model;
}; // class TabsListPrivate

void
TabsListPrivate::init()
{
	m_view = new QListView( q );

	m_model = new TabsModel( q );

	m_view->setModel( m_model );

	q->setWindowTitle( TabsList::tr( "Tabs" ) );

	q->setWidget( m_view );

	TabsList::connect( m_view, &QListView::doubleClicked,
		q, &TabsList::p_doubleClicked );
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

QStringListModel *
TabsList::model() const
{
	return d->m_model;
}

void
TabsList::p_doubleClicked( const QModelIndex & index )
{
	if( index.isValid() )
	{
		TopGui::instance()->projectWindow()->projectWidget()->activateTab(
			index.data().toString() );
	}
}

} /* namespace Core */

} /* namespace Prototyper */
