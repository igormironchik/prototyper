
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
#include <QStringListModel>
#include <QModelIndex>
#include <QKeyEvent>
#include <QAction>
#include <QMenu>


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

private:
	Q_DISABLE_COPY( TabsModel )
}; // class TabsModel


//
// TabsViewPrivate
//

class TabsViewPrivate {
public:
	TabsViewPrivate( TabsView * parent )
		:	q( parent )
		,	m_activate( 0 )
	{
	}

	//! Init.
	void init();

	//! Parent.
	TabsView * q;
	//! Activate tab action.
	QAction * m_activate;
	//! Index under cursor.
	QModelIndex m_index;
}; // class TabsViewPrivate

void
TabsViewPrivate::init()
{
	m_activate = new QAction( TabsView::tr( "Activate Tab" ), q );

	TabsView::connect( m_activate, &QAction::triggered,
		q, &TabsView::p_activateTab );
}


//
// TabsView
//

TabsView::TabsView( QWidget * parent )
	:	QListView( parent )
	,	d( new TabsViewPrivate( this ) )
{
	d->init();
}

TabsView::~TabsView()
{
}

void
TabsView::keyPressEvent( QKeyEvent * event )
{
	if( event->key() == Qt::Key_Return ||
		event->key() == Qt::Key_Enter )
			emit enterPressed( currentIndex() );

	QListView::keyPressEvent( event );
}

void
TabsView::contextMenuEvent( QContextMenuEvent * event )
{
	d->m_index = indexAt( event->pos() );

	QMenu menu;

	if( d->m_index.isValid() )
	{
		menu.addAction( d->m_activate );

		if( d->m_index.row() >= 0 )
			menu.addAction( QIcon( ":/Core/img/edit-rename.png" ),
				tr( "Rename" ), this, SLOT( p_renameTab() ) );

		menu.addAction( QIcon( ":/Core/img/list-add.png" ),
			tr( "Add Form" ), this, SLOT( p_addForm() ) );

		if( d->m_index.row() != 0 )
			menu.addAction( QIcon( ":/Core/img/edit-delete.png" ),
				tr( "Delete Form" ), this, SLOT( p_deleteForm() ) );
	}
	else
		menu.addAction( QIcon( ":/Core/img/list-add.png" ),
			tr( "Add Form" ), this, SLOT( p_addForm() ) );

	menu.exec( event->globalPos() );

	event->accept();
}

void
TabsView::p_activateTab()
{
	emit activateTab( d->m_index );
}

void
TabsView::p_renameTab()
{
	TopGui::instance()->projectWindow()->projectWidget()->renameTab(
		d->m_index.data().toString() );
}

void
TabsView::p_addForm()
{
	TopGui::instance()->projectWindow()->projectWidget()->addForm();
}

void
TabsView::p_deleteForm()
{
	TopGui::instance()->projectWindow()->projectWidget()->deleteForm(
		d->m_index.data().toString() );
}


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
	TabsView * m_view;
	//! Model.
	TabsModel * m_model;
}; // class TabsListPrivate

void
TabsListPrivate::init()
{
	m_view = new TabsView( q );

	m_model = new TabsModel( q );

	m_view->setModel( m_model );

	q->setWindowTitle( TabsList::tr( "Tabs" ) );

	q->setWidget( m_view );

	TabsList::connect( m_view, &QListView::doubleClicked,
		q, &TabsList::p_activateTab );
	TabsList::connect( m_view, &TabsView::enterPressed,
		q, &TabsList::p_activateTab );
	TabsList::connect( m_view, &TabsView::activateTab,
		q, &TabsList::p_activateTab );
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
TabsList::p_activateTab( const QModelIndex & index )
{
	if( index.isValid() )
	{
		TopGui::instance()->projectWindow()->projectWidget()->activateTab(
			index.data().toString() );
	}
}

} /* namespace Core */

} /* namespace Prototyper */
