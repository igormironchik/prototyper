
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
#include "form_view.hpp"
#include "project_cfg.hpp"
#include "form_scene.hpp"
#include "form.hpp"

// Qt include.
#include <QTabWidget>
#include <QVBoxLayout>
#include <QMessageBox>


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
	TabWidget( QWidget * parent = 0 )
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
	ProjectWidgetPrivate( Cfg::Project & cfg, ProjectWidget * parent )
		:	q( parent )
		,	m_cfg( cfg )
		,	m_tabs( 0 )
		,	m_desc( 0 )
		,	m_tabBar( 0 )
	{
	}

	//! Init.
	void init();
	//! New project.
	void newProject();
	//! Add form.
	void addForm( const QString & tabName, int gridStep, bool showGrid,
		const QSize & size = QSize( 800, 600 ) );

	//! Parent.
	ProjectWidget * q;
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
	QList< FormView* > m_forms;
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

	m_tabNames.append( ProjectWidget::tr( "Project" ) );
	m_desc = new ProjectDescTab( m_tabs );
	m_tabs->addTab( m_desc, m_tabNames.first() );

	ProjectWidget::connect( m_tabBar, &ProjectTabBar::formRenameRequest,
		q, &ProjectWidget::renameTab );
	ProjectWidget::connect( m_tabBar, &ProjectTabBar::formAddRequest,
		q, &ProjectWidget::addForm );
	ProjectWidget::connect( m_tabBar, &ProjectTabBar::formDeleteRequest,
		q, &ProjectWidget::deleteForm );
	ProjectWidget::connect( m_desc->editor(), &TextEditor::changed,
		q, &ProjectWidget::changed );
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

		tab->deleteLater();
	}

	m_forms.clear();

	static const QString projectDescTabName =
		ProjectWidget::tr( "Project" );

	m_tabs->setTabText( 0, projectDescTabName );

	m_tabNames[ 0 ] = projectDescTabName;

	m_desc->editor()->reset();

	m_cfg = Cfg::Project();
}

void
ProjectWidgetPrivate::addForm( const QString & tabName,
	int gridStep, bool showGrid, const QSize & size )
{
	FormView * form = new FormView( m_tabs );
	form->form()->setGridStep( gridStep );
	form->form()->setGridMode( showGrid ?
		Form::ShowGrid : Form::NoGrid );
	form->form()->setSize( size );

	m_tabNames.append( tabName );

	m_tabs->addTab( form, tabName );

	m_forms.append( form );

	ProjectWidget::connect( form->formScene(), &FormScene::changed,
		q, &ProjectWidget::changed );
}


//
// ProjectWidget
//

ProjectWidget::ProjectWidget( Cfg::Project & cfg,
	QWidget * parent, Qt::WindowFlags f )
	:	QWidget( parent, f )
	,	d( new ProjectWidgetPrivate( cfg, this ) )
{
	d->init();
}

ProjectWidget::~ProjectWidget()
{
}

const QList< FormView* > &
ProjectWidget::forms() const
{
	return d->m_forms;
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

	d->m_cfg = cfg;

	d->m_desc->editor()->setText( d->m_cfg.description().text() );

	d->m_tabs->setTabText( 0, d->m_cfg.description().tabName() );

	foreach( const Cfg::Form & f, d->m_cfg.form() )
		d->addForm( f.tabName(), f.gridStep(), d->m_cfg.showGrid(),
			QSize( f.size().width(), f.size().height() ) );
}

void
ProjectWidget::addForm()
{
	NameDlg dlg( d->m_tabNames, this );

	if( dlg.exec() == QDialog::Accepted )
	{
		d->addForm( dlg.name(), d->m_cfg.defaultGridStep(),
			d->m_cfg.showGrid() );

		d->m_tabs->setCurrentIndex( d->m_tabs->count() - 1 );

		emit changed();
	}
}

void
ProjectWidget::renameTab( const QString & oldName )
{
	if( d->m_tabNames.contains( oldName ) )
	{
		NameDlg dlg( d->m_tabNames, this );

		if( dlg.exec() == QDialog::Accepted )
		{
			const int index = d->m_tabNames.indexOf( oldName );

			d->m_tabs->setTabText( index, dlg.name() );

			d->m_tabNames[ index ] = dlg.name();

			emit changed();
		}
	}
}

void
ProjectWidget::deleteForm( const QString & name )
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

			QWidget * tab = d->m_tabs->widget( index );

			d->m_tabs->removeTab( index );

			d->m_tabNames.removeAt( index );

			d->m_forms.removeAt( index );

			disconnect( tab, 0, 0, 0 );

			tab->deleteLater();

			emit changed();
		}
	}
}

void
ProjectWidget::newProject()
{
	d->newProject();
}

} /* namespace Core */

} /* namespace Prototyper */
