
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
#include "grid_step_dlg.hpp"
#include "project_cfg.hpp"
#include "session_cfg.hpp"
#include "project_description_tab.hpp"
#include "text_editor.hpp"

// Qt include.
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QCloseEvent>
#include <QApplication>
#include <QMessageBox>
#include <QFileDialog>


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
		,	m_saveProject( 0 )
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
	//! Save project action.
	QAction * m_saveProject;
	//! Cfg.
	Cfg::Project m_cfg;
	//! File name.
	QString m_fileName;
}; // class ProjectWindowPrivate

void
ProjectWindowPrivate::init()
{
	m_widget = new ProjectWidget( m_cfg, q );

	q->setCentralWidget( m_widget );

	q->setWindowTitle( ProjectWindow::tr( "Prototyper - Unsaved[*]" ) );

	QMenu * file = q->menuBar()->addMenu( ProjectWindow::tr( "&File" ) );

	QAction * newProject = file->addAction(
		QIcon( ":/Core/img/document-new.png" ),
		ProjectWindow::tr( "New Project" ) );
	newProject->setShortcutContext( Qt::ApplicationShortcut );
	newProject->setShortcut( ProjectWindow::tr( "Ctrl+N" ) );

	QAction * openProject = file->addAction(
		QIcon( ":/Core/img/document-open.png" ),
		ProjectWindow::tr( "Open Project" ) );
	openProject->setShortcutContext( Qt::ApplicationShortcut );
	openProject->setShortcut( ProjectWindow::tr( "Ctrl+O" ) );

	file->addSeparator();

	m_saveProject = file->addAction(
		QIcon( ":/Core/img/document-save.png" ),
		ProjectWindow::tr( "Save Project" ) );
	m_saveProject->setShortcutContext( Qt::ApplicationShortcut );
	m_saveProject->setShortcut( ProjectWindow::tr( "Ctrl+S" ) );

	QAction * saveProjectAs = file->addAction(
		QIcon( ":/Core/img/document-save-as.png" ),
		ProjectWindow::tr( "Save Project As" ) );

	file->addSeparator();

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
	ProjectWindow::connect( newProject, &QAction::triggered,
		q, &ProjectWindow::newProject );
	ProjectWindow::connect( openProject, &QAction::triggered,
		q, &ProjectWindow::openProject );
	ProjectWindow::connect( m_saveProject, &QAction::triggered,
		q, &ProjectWindow::saveProject );
	ProjectWindow::connect( saveProjectAs, &QAction::triggered,
		q, &ProjectWindow::saveProjectAs );
	ProjectWindow::connect( m_widget, &ProjectWidget::changed,
		q, &ProjectWindow::projectChanged );
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

ProjectWidget *
ProjectWindow::projectWidget() const
{
	return d->m_widget;
}

const QString &
ProjectWindow::projectFileName() const
{
	return d->m_fileName;
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
ProjectWindow::readProject( const QString & fileName )
{
	try {
		Cfg::TagProject tag;

		QtConfFile::readQtConfFile( tag, fileName,
			QTextCodec::codecForName( "UTF-8" ) );

		newProject();

		d->m_fileName = fileName;

		d->m_widget->setProject( tag.getCfg() );

		setWindowModified( false );

		setWindowTitle( tr( "Prototyper - %1[*]" )
			.arg( QFileInfo( fileName ).baseName() ) );
	}
	catch( const QtConfFile::Exception & x )
	{
		QMessageBox::warning( this, tr( "Unable to Read Project..." ),
			tr( "Unable to read project.\n%1" ).arg( x.whatAsQString() ) );
	}
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
	if( isWindowModified() )
	{
		QMessageBox::StandardButton btn =
			QMessageBox::question( this, tr( "Do You Want to Save Project..." ),
				tr( "Do you want to save project?" ),
				QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes );

		if( btn == QMessageBox::Yes )
			saveProjectImpl();
	}

	TopGui::instance()->saveCfg( 0 );

	QApplication::quit();
}

void
ProjectWindow::showHideGrid( bool show )
{
	Form::GridMode mode = ( show ? Form::ShowGrid : Form::NoGrid );

	d->m_cfg.setShowGrid( show );

	foreach( FormView * view, d->m_widget->forms() )
		view->form()->setGridMode( mode );
}

void
ProjectWindow::setGridStep()
{
	const int index = d->m_widget->tabs()->currentIndex();

	int step = d->m_cfg.defaultGridStep();
	bool forAll = true;

	if( index > 0 )
		step = d->m_widget->forms().at( index - 1 )->form()->gridStep();

	GridStepDlg dlg( step, forAll, this );

	if( dlg.exec() == QDialog::Accepted )
	{
		if( dlg.applyForAllForms() )
		{
			d->m_cfg.setDefaultGridStep( dlg.gridStep() );

			foreach( FormView * view, d->m_widget->forms() )
				view->form()->setGridStep( dlg.gridStep() );
		}
		else if( index > 0 )
			d->m_widget->forms()[ index - 1 ]->form()->setGridStep(
				dlg.gridStep() );
	}
}

void
ProjectWindow::openProject()
{
	const QString fileName =
		QFileDialog::getOpenFileName( this, tr( "Select Project to Open..." ),
			QString(), tr( "Prototyper Project (*.prototyper)" ) );

	if( !fileName.isEmpty() )
		readProject( fileName );
}

void
ProjectWindow::newProject()
{
	if( isWindowModified() )
	{
		QMessageBox::StandardButton btn =
			QMessageBox::question( this, tr( "Project Modified..." ),
				tr( "Project modified.\nDo you want to save it?" ),
				QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes );

		if( btn == QMessageBox::Yes )
			saveProjectImpl();
	}

	d->m_widget->newProject();

	setWindowTitle( tr( "Prototyper - Unsaved[*]" ) );

	setWindowModified( false );

	d->m_saveProject->setEnabled( true );
}

void
ProjectWindow::saveProjectImpl( const QString & fileName )
{
	if( !fileName.isEmpty() )
		d->m_fileName = fileName;

	if( !d->m_fileName.isEmpty() )
	{
		d->m_cfg.setDescription(
			d->m_widget->descriptionTab()->editor()->text() );

		try {
			Cfg::TagProject tag( d->m_cfg );

			QtConfFile::writeQtConfFile( tag, d->m_fileName,
				QTextCodec::codecForName( "UTF-8" ) );
		}
		catch( const QtConfFile::Exception & x )
		{
			QMessageBox::warning( this, tr( "Unable to Save Project..." ),
				tr( "Unable to save project.\n%1" ).arg( x.whatAsQString() ) );

			return;
		}

		d->m_saveProject->setEnabled( false );

		setWindowModified( false );
	}
	else
		saveProjectAs();
}

void
ProjectWindow::saveProject()
{
	saveProjectImpl();
}

void
ProjectWindow::saveProjectAs()
{
	const QString fileName = QFileDialog::getSaveFileName( this,
		tr( "Select File to Save Project..." ),
		QString(),
		tr( "Prototyper Project (*.prototyper)" ) );

	if( !fileName.isEmpty() )
	{
		setWindowTitle( tr( "Prototyper - %1[*]" )
			.arg( QFileInfo( fileName ).baseName() ) );

		saveProjectImpl( fileName );
	}
}

void
ProjectWindow::projectChanged()
{
	d->m_saveProject->setEnabled( true );

	setWindowModified( true );
}

} /* namespace Core */

} /* namespace Prototyper */
