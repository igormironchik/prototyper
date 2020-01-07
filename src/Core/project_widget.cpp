
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
#include "project_widget.hpp"
#include "project_description_tab.hpp"
#include "project_widget_tab_bar.hpp"
#include "name_dlg.hpp"
#include "text_editor.hpp"
#include "page_view.hpp"
#include "project_cfg.hpp"
#include "page_scene.hpp"
#include "page.hpp"
#include "top_gui.hpp"
#include "project_window.hpp"
#include "tabs_list.hpp"
#include "project_window.hpp"
#include "utils.hpp"

// Qt include.
#include <QTabWidget>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QStringListModel>
#include <QApplication>
#include <QAction>
#include <QUndoStack>
#include <QUndoGroup>


namespace Prototyper {

namespace Core {

//
// TabWidget
//

//! Tabs widget.
class TabWidget
	:	public QTabWidget
{
public:
	explicit TabWidget( QWidget * parent = 0 )
		:	QTabWidget( parent )
	{
	}

	void setTabBar( QTabBar * bar )
	{
		QTabWidget::setTabBar( bar );
	}
}; // class TabWidget


//
// ProjectWidgetPrivate
//

class ProjectWidgetPrivate {
public:
	ProjectWidgetPrivate( Cfg::Project & cfg, ProjectWidget * parent, ProjectWindow * w )
		:	q( parent )
		,	m_window( w )
		,	m_cfg( cfg )
		,	m_tabs( 0 )
		,	m_desc( 0 )
		,	m_tabBar( 0 )
		,	m_undoGroup( 0 )
		,	m_isTabRenamed( false )
	{
	}

	//! Init.
	void init();
	//! New project.
	void newProject();
	//! Add page.
	void addPage( Cfg::Page & cfg, bool showGrid );

	//! Parent.
	ProjectWidget * q;
	//! Window.
	ProjectWindow * m_window;
	//! Cfg.
	Cfg::Project & m_cfg;
	//! Tabs.
	TabWidget * m_tabs;
	//! Desc tab.
	ProjectDescTab * m_desc;
	//! Tab bar.
	ProjectTabBar * m_tabBar;
	//! Tab's names.
	QStringList m_tabNames;
	//! Forms.
	QList< PageView* > m_forms;
	//! Undo group.
	QUndoGroup * m_undoGroup;
	//! Is tab renamed?
	bool m_isTabRenamed;
}; // class ProjectWidgetPrivate

void
ProjectWidgetPrivate::init()
{
	QVBoxLayout * layout = new QVBoxLayout( q );

	m_tabs = new TabWidget( q );

	layout->addWidget( m_tabs );

	m_tabBar = new ProjectTabBar( m_tabs );

	m_tabs->setTabBar( m_tabBar );
	m_tabs->setMovable( false );
	m_tabs->setTabPosition( QTabWidget::South );

	m_tabNames.append( ProjectWidget::tr( "Description" ) );
	m_desc = new ProjectDescTab( m_tabs );
	m_tabs->addTab( m_desc, m_tabNames.first() );

	m_undoGroup = new QUndoGroup( q );

	ProjectWidget::connect( m_tabBar, &ProjectTabBar::formRenameRequest,
		q, &ProjectWidget::renameTab );
	ProjectWidget::connect( m_tabBar, &ProjectTabBar::formAddRequest,
		q, &ProjectWidget::addPage );
	ProjectWidget::connect( m_tabBar, &ProjectTabBar::formDeleteRequest,
		q, &ProjectWidget::deletePage );
	ProjectWidget::connect( m_desc->editor(), &TextEditor::changed,
		q, &ProjectWidget::changed );
	ProjectWidget::connect( m_tabs, &QTabWidget::currentChanged,
		q, &ProjectWidget::tabChanged );
}

void
ProjectWidgetPrivate::newProject()
{
	for( int i = 1; i < m_tabNames.size(); )
	{
		QWidget * tab = m_tabs->widget( i );

		m_tabs->removeTab( i );

		m_tabNames.removeAt( i );

		ProjectWidget::disconnect( tab, 0, 0, 0 );

		m_undoGroup->removeStack( m_forms.at( i - 1 )->page()->undoStack() );

		m_forms.at( i - 1 )->page()->undoStack()->deleteLater();

		tab->deleteLater();
	}

	m_forms.clear();

	static const QString projectDescTabName =
		ProjectWidget::tr( "Description" );

	m_tabs->setTabText( 0, projectDescTabName );

	m_tabNames[ 0 ] = projectDescTabName;

	m_desc->editor()->reset();

	m_cfg = Cfg::Project();

	m_cfg.description().set_tabName( projectDescTabName );

	TopGui::instance()->projectWindow()->tabsList()->model()->
		setStringList( m_tabNames );

	q->cleanUndoGroup();
}

void
ProjectWidgetPrivate::addPage( Cfg::Page & cfg,
	bool showGrid )
{
	PageView * form = new PageView( cfg, m_tabs );

	ProjectWidget::connect( form, &PageView::zoomChanged,
		m_window, &ProjectWindow::zoomChanged );
	ProjectWidget::connect( form->formScene(), &PageScene::selectionChanged,
		m_window, &ProjectWindow::selectionChanged );

	form->page()->setGridMode( showGrid ?
		ShowGrid : NoGrid );

	m_tabNames.append( cfg.tabName() );

	m_tabs->addTab( form, cfg.tabName() );

	m_forms.append( form );

	ProjectWidget::connect( form->formScene(), &PageScene::changed,
		q, &ProjectWidget::changed );
	ProjectWidget::connect( form->page(), &Page::changed,
		q, &ProjectWidget::changed );

	emit q->pageAdded( form );
}


//
// ProjectWidget
//

ProjectWidget::ProjectWidget( Cfg::Project & cfg,
	ProjectWindow * parent, Qt::WindowFlags f )
	:	QWidget( parent, f )
	,	d( new ProjectWidgetPrivate( cfg, this, parent ) )
{
	d->init();
}

ProjectWidget::~ProjectWidget()
{
}

const QList< PageView* > &
ProjectWidget::pages() const
{
	return d->m_forms;
}

void
ProjectWidget::enableSelection( bool on )
{
	foreach( PageView * view, d->m_forms )
		view->enableSelection( on );
}

QStringList
ProjectWidget::pagesNames() const
{
	QStringList res = d->m_tabNames;
	res.removeFirst();

	return res;
}

QString
ProjectWidget::projectTabName() const
{
	return d->m_tabNames.front();
}

QTabWidget *
ProjectWidget::tabs() const
{
	return d->m_tabs;
}

ProjectDescTab *
ProjectWidget::descriptionTab() const
{
	return d->m_desc;
}

void
ProjectWidget::setProject( const Cfg::Project & cfg )
{
	d->newProject();

	QApplication::processEvents();

	d->m_cfg = cfg;

	d->m_desc->editor()->setText( d->m_cfg.description().text() );

	d->m_tabs->setTabText( 0, d->m_cfg.description().tabName() );

	d->m_tabNames[ 0 ] = d->m_cfg.description().tabName();

	auto it = d->m_cfg.page().begin();
	auto last = d->m_cfg.page().end();

	for( ; it != last; ++it )
		d->addPage( *it, d->m_cfg.showGrid() );

	TopGui::instance()->projectWindow()->tabsList()->model()->
		setStringList( d->m_tabNames );

	cleanUndoGroup();
}

QUndoGroup *
ProjectWidget::undoGroup() const
{
	return d->m_undoGroup;
}

void
ProjectWidget::cleanUndoGroup()
{
	foreach( QUndoStack * s, d->m_undoGroup->stacks() )
		s->setClean();
}

void
ProjectWidget::addPage()
{
	Cfg::Page cfg;
	cfg.set_gridStep( d->m_cfg.defaultGridStep() );
	cfg.size().set_width( 210.0 );
	cfg.size().set_height( 297.0 );
	cfg.set_tabName( tr( "Page %1" ).arg( QString::number( d->m_tabs->count() ) ) );

	d->addPage( cfg, d->m_cfg.showGrid() );

	d->m_tabs->setCurrentIndex( d->m_tabs->count() - 1 );

	TopGui::instance()->projectWindow()->tabsList()->model()->
		setStringList( d->m_tabNames );

	emit changed();
}

void
ProjectWidget::renameTab( const QString & oldName )
{
	if( d->m_tabNames.contains( oldName ) )
	{
		const int index = d->m_tabNames.indexOf( oldName );

		QStringList names = d->m_tabNames;

		if( index - 1 >= 0 )
			names << d->m_forms.at( index - 1 )->page()->ids();

		NameDlg dlg( names,
			( index == 0 ? tr( "Enter New Project Tab Name..." ) : tr( "Enter New Page Name..." ) ),
			oldName, this );

		if( dlg.exec() == QDialog::Accepted )
		{
			const QString tmpOldName = oldName;

			d->m_tabs->setTabText( index, dlg.name() );

			d->m_tabNames[ index ] = dlg.name();

			if( index > 0 )
				d->m_forms[ index - 1 ]->page()->renameForm( dlg.name() );
			else
				d->m_cfg.description().set_tabName( dlg.name() );

			TopGui::instance()->projectWindow()->tabsList()->model()->
				setStringList( d->m_tabNames );

			setTabRenamed();

			emit changed();
		}
	}
}

void
ProjectWidget::deletePage( const QString & name )
{
	if( d->m_tabNames.contains( name ) )
	{
		QMessageBox::StandardButton btn =
			QMessageBox::question( this, tr( "Form Deletion..." ),
				tr( "You are about to delete form \"%1\".\n"
					"Are you sure?" ).arg( name ) );

		if( btn == QMessageBox::Yes )
		{
			const int index = d->m_tabNames.indexOf( name );

			d->m_undoGroup->removeStack(
				d->m_forms.at( index - 1 )->page()->undoStack() );

			d->m_forms.at( index - 1 )->page()->undoStack()->deleteLater();

			QWidget * tab = d->m_tabs->widget( index );

			d->m_tabNames.removeAt( index );

			PageView * form = d->m_forms.at( index - 1 );

			d->m_forms.removeAt( index - 1 );

			d->m_tabs->removeTab( index );

			disconnect( tab, 0, 0, 0 );

			tab->deleteLater();

			TopGui::instance()->projectWindow()->tabsList()->model()->
				setStringList( d->m_tabNames );

			emit changed();

			emit pageDeleted( form );
		}
	}
}

void
ProjectWidget::newProject()
{
	d->newProject();
}

void
ProjectWidget::activateTab( const QString & tabName )
{
	if( d->m_tabNames.contains( tabName ) )
		d->m_tabs->setCurrentIndex( d->m_tabNames.indexOf( tabName ) );
}

void
ProjectWidget::tabChanged( int index )
{
	if( index > 0 )
	{
		d->m_undoGroup->setActiveStack(
			d->m_forms.at( index - 1 )->page()->undoStack() );
	}
	else
		d->m_undoGroup->setActiveStack( Q_NULLPTR );
}

bool
ProjectWidget::isTabRenamed() const
{
	return d->m_isTabRenamed;
}

void
ProjectWidget::setTabRenamed( bool on )
{
	d->m_isTabRenamed = on;
}

} /* namespace Core */

} /* namespace Prototyper */
