
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
#include "form_hierarchy_widget.hpp"
#include "form_hierarchy_model.hpp"
#include "form_object.hpp"
#include "top_gui.hpp"
#include "project_window.hpp"
#include "project_widget.hpp"
#include "form_view.hpp"
#include "form.hpp"
#include "form_actions.hpp"
#include "form_scene.hpp"

// Qt include.
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QApplication>
#include <QItemSelectionModel>
#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>


namespace Prototyper {

namespace Core {

//
// FormHierarchyView
//

FormHierarchyView::FormHierarchyView( QWidget * parent )
	:	QTreeView( parent )
{
	setSelectionMode( QAbstractItemView::ExtendedSelection );
}

FormHierarchyView::~FormHierarchyView()
{
}

void
FormHierarchyView::changeId()
{
	const QModelIndex index = currentIndex();

	if( index.isValid() )
	{
		FormObject * obj = static_cast< FormObject* >
			( index.internalPointer() );

		obj->form()->renameObject( obj );

		update( index );
	}
}

void
FormHierarchyView::selectionChanged( const QItemSelection & selected,
	const QItemSelection & deselected )
{
	QTreeView::selectionChanged( selected, deselected );

	foreach( FormView * view,
		TopGui::instance()->projectWindow()->projectWidget()->forms() )
	{
		view->scene()->clearSelection();
	}

	QApplication::processEvents();

	foreach( const QModelIndex & index, selectionModel()->selectedIndexes() )
	{
		if( index.isValid() )
		{
			QGraphicsItem * item = dynamic_cast< QGraphicsItem* >
				( static_cast< FormObject* > ( index.internalPointer() ) );

			if( item )
				item->setSelected( true );
		}
	}
}

void
FormHierarchyView::contextMenuEvent( QContextMenuEvent * e )
{
	if( currentIndex().isValid() )
	{
		QMenu menu;

		menu.addAction( QIcon( ":/Core/img/edit-rename.png" ),
			tr( "Rename" ), this, SLOT( changeId() ) );

		menu.exec( e->globalPos() );

		e->accept();
	}
	else
		e->ignore();
}

void
FormHierarchyView::keyPressEvent( QKeyEvent* e )
{
	if( e->key() == Qt::Key_Delete )
	{
		if( selectionModel()->hasSelection() )
		{
			FormScene * s = dynamic_cast< FormScene* >
				( FormAction::instance()->form()->scene() );

			if( s )
				s->deleteSelected();
		}

		e->accept();
	}
	else
		QTreeView::keyPressEvent( e );
}


//
// FormHierarchyWidgetPrivate
//

class FormHierarchyWidgetPrivate {
public:
	FormHierarchyWidgetPrivate( FormHierarchyWidget * parent )
		:	q( parent )
		,	m_view( 0 )
		,	m_model( 0 )
	{
	}

	//! Init.
	void init();

	//! Parent.
	FormHierarchyWidget * q;
	//! Tree.
	FormHierarchyView * m_view;
	//! Model.
	FormHierarchyModel * m_model;
}; // class FormHierarchyWidgetPrivate

void
FormHierarchyWidgetPrivate::init()
{
	m_view = new FormHierarchyView( q );

	m_view->setHeaderHidden( true );

	m_model = new FormHierarchyModel( q );

	m_view->setModel( m_model );

	q->setWindowTitle( FormHierarchyWidget::tr( "Form's Hierarchy" ) );

	q->setWidget( m_view );
}


//
// FormHierarchyWidget
//

FormHierarchyWidget::FormHierarchyWidget( QWidget * parent, Qt::WindowFlags f )
	:	QDockWidget( parent, f )
	,	d( new FormHierarchyWidgetPrivate( this ) )
{
	d->init();
}

FormHierarchyWidget::~FormHierarchyWidget()
{
}

FormHierarchyView *
FormHierarchyWidget::view() const
{
	return d->m_view;
}

FormHierarchyModel *
FormHierarchyWidget::model() const
{
	return d->m_model;
}

} /* namespace Core */

} /* namespace Prototyper */
