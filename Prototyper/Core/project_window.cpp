
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
#include "project_window.hpp"
#include "project_widget.hpp"
#include "top_gui.hpp"
#include "props_window.hpp"
#include "tools_window.hpp"
#include "form.hpp"
#include "form_view.hpp"
#include "form_scene.hpp"

// Qt include.
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QCloseEvent>
#include <QApplication>


namespace Prototyper {

namespace Core {

//
// ProjectWindowPrivate
//

class ProjectWindowPrivate {
public:
	ProjectWindowPrivate( ProjectWindow * parent )
		:	q( parent )
		,	m_widget( 0 )
		,	m_propsAction( 0 )
		,	m_toolsAction( 0 )
	{
	}

	//! Init.
	void init();

	//! Parent.
	ProjectWindow * q;
	//! Central widget.
	ProjectWidget * m_widget;
	//! Properties action.
	QAction * m_propsAction;
	//! Tools action.
	QAction * m_toolsAction;
}; // class ProjectWindowPrivate

void
ProjectWindowPrivate::init()
{
	m_widget = new ProjectWidget( q );

	q->setCentralWidget( m_widget );

	q->setWindowTitle( QLatin1String( "Prototyper" ) );

	QMenu * file = q->menuBar()->addMenu( ProjectWindow::tr( "&File" ) );
	QAction * quitAction = file->addAction(
		QIcon( ":/Core/img/application-exit.png" ),
		ProjectWindow::tr( "Quit" ) );
	quitAction->setShortcutContext( Qt::ApplicationShortcut );
	quitAction->setShortcut( ProjectWindow::tr( "Ctrl+Q" ) );

	QMenu * view = q->menuBar()->addMenu( ProjectWindow::tr( "&View" ) );

	m_propsAction = view->addAction( ProjectWindow::tr( "Properties" ) );
	m_propsAction->setCheckable( true );
	m_propsAction->setChecked( true );
	m_propsAction->setShortcutContext( Qt::ApplicationShortcut );
	m_propsAction->setShortcut( ProjectWindow::tr( "Alt+P" ) );

	m_toolsAction = view->addAction( ProjectWindow::tr( "Tools" ) );
	m_toolsAction->setCheckable( true );
	m_toolsAction->setChecked( true );
	m_toolsAction->setShortcutContext( Qt::ApplicationShortcut );
	m_toolsAction->setShortcut( ProjectWindow::tr( "Alt+T" ) );

	QMenu * form = q->menuBar()->addMenu( ProjectWindow::tr( "F&orm" ) );
	QAction * grid = form->addAction(
		QIcon( ":/Core/img/view-grid.png" ),
		ProjectWindow::tr( "Show Grid" ) );
	grid->setShortcutContext( Qt::ApplicationShortcut );
	grid->setShortcut( ProjectWindow::tr( "Alt+G" ) );
	grid->setCheckable( true );
	grid->setChecked( true );

	QAction * gridStep = form->addAction(
		QIcon( ":/Core/img/measure.png" ),
		ProjectWindow::tr( "Grid Step" ) );

	QAction * newForm = new QAction( q );
	newForm->setShortcutContext( Qt::ApplicationShortcut );
	newForm->setShortcut( ProjectWindow::tr( "Ctrl+T" ) );
	q->addAction( newForm );

	ProjectWindow::connect( m_propsAction, &QAction::toggled,
		q, &ProjectWindow::showHidePropertiesWindow );
	ProjectWindow::connect( m_toolsAction, &QAction::toggled,
		q, &ProjectWindow::showHideToolsWindow );
	ProjectWindow::connect( quitAction, &QAction::triggered,
		q, &ProjectWindow::quit );
	ProjectWindow::connect( grid, &QAction::toggled,
		q, &ProjectWindow::showHideGrid );
	ProjectWindow::connect( gridStep, &QAction::triggered,
		q, &ProjectWindow::setGridStep );
	ProjectWindow::connect( newForm, &QAction::triggered,
		m_widget, &ProjectWidget::addForm );
}


//
// ProjectWindow
//

ProjectWindow::ProjectWindow( QWidget * parent, Qt::WindowFlags f )
	:	QMainWindow( parent, f )
	,	d( new ProjectWindowPrivate( this ) )
{
	d->init();
}

ProjectWindow::~ProjectWindow()
{
}

void
ProjectWindow::hidePropsWindow()
{
	d->m_propsAction->setChecked( false );
}

void
ProjectWindow::hideToolsWindow()
{
	d->m_toolsAction->setChecked( false );
}

void
ProjectWindow::closeEvent( QCloseEvent * e )
{
	e->accept();

	quit();
}

void
ProjectWindow::showHidePropertiesWindow( bool show )
{
	if( show )
		TopGui::instance()->propsWindow()->show();
	else
		TopGui::instance()->propsWindow()->hide();
}

void
ProjectWindow::showHideToolsWindow( bool show )
{
	if( show )
		TopGui::instance()->toolsWindow()->show();
	else
		TopGui::instance()->toolsWindow()->hide();
}

void
ProjectWindow::quit()
{
	TopGui::instance()->saveCfg( this );

	QApplication::quit();
}

void
ProjectWindow::showHideGrid( bool show )
{
	Form::GridMode mode = ( show ? Form::ShowGrid : Form::NoGrid );

	foreach( FormView * view, d->m_widget->forms() )
		view->form()->setGridMode( mode );
}

void
ProjectWindow::setGridStep()
{

}

} /* namespace Core */

} /* namespace Prototyper */
