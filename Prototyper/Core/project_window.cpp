
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
#include <QStandardPaths>
#include <QToolBar>
#include <QActionGroup>


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
		,	m_grid( 0 )
		,	m_gridStep( 0 )
		,	m_formToolBar( 0 )
		,	m_formToolBarGroup( 0 )
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
	//! Show/hide grid action.
	QAction * m_grid;
	//! Grid step action.
	QAction * m_gridStep;
	//! Cfg.
	Cfg::Project m_cfg;
	//! File name.
	QString m_fileName;
	//! Form toolbar.
	QToolBar * m_formToolBar;
	//! Form toolbar group.
	QActionGroup * m_formToolBarGroup;
}; // class ProjectWindowPrivate

void
ProjectWindowPrivate::init()
{
	m_widget = new ProjectWidget( m_cfg, q );

	q->addToolBar( Qt::TopToolBarArea, m_widget->descriptionTab()->toolBar() );

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

//	m_toolsAction = view->addAction( ProjectWindow::tr( "Tools" ) );
//	m_toolsAction->setCheckable( true );
//	m_toolsAction->setChecked( true );
//	m_toolsAction->setShortcutContext( Qt::ApplicationShortcut );
//	m_toolsAction->setShortcut( ProjectWindow::tr( "Alt+T" ) );

	QMenu * form = q->menuBar()->addMenu( ProjectWindow::tr( "F&orm" ) );
	m_grid = form->addAction(
		QIcon( ":/Core/img/view-grid.png" ),
		ProjectWindow::tr( "Show Grid" ) );
	m_grid->setShortcutContext( Qt::ApplicationShortcut );
	m_grid->setShortcut( ProjectWindow::tr( "Alt+G" ) );
	m_grid->setCheckable( true );
	m_grid->setChecked( true );

	m_gridStep = form->addAction(
		QIcon( ":/Core/img/measure.png" ),
		ProjectWindow::tr( "Grid Step" ) );

	QAction * newForm = new QAction( q );
	newForm->setShortcutContext( Qt::ApplicationShortcut );
	newForm->setShortcut( ProjectWindow::tr( "Ctrl+T" ) );
	q->addAction( newForm );

	m_formToolBar = new QToolBar( ProjectWindow::tr( "Form Tools" ), q );
	m_formToolBarGroup = new QActionGroup( q );
	m_formToolBarGroup->setExclusive( true );

	QAction * select = m_formToolBar->addAction(
		QIcon( ":/Core/img/edit-select.png" ),
		ProjectWindow::tr( "Select" ) );
	select->setCheckable( true );
	m_formToolBarGroup->addAction( select );

	QAction * move = m_formToolBar->addAction(
		QIcon( ":/Core/img/transform-move.png" ),
		ProjectWindow::tr( "Move" ) );
	move->setCheckable( true );
	m_formToolBarGroup->addAction( move );

	QAction * drawPolyline = m_formToolBar->addAction(
		QIcon( ":/Core/img/draw-polyline.png" ),
		ProjectWindow::tr( "Draw Polyline" ) );
	drawPolyline->setCheckable( true );
	m_formToolBarGroup->addAction( drawPolyline );

	QAction * insertText = m_formToolBar->addAction(
		QIcon( ":/Core/img/insert-text.png" ),
		ProjectWindow::tr( "Insert Text" ) );
	insertText->setCheckable( true );
	m_formToolBarGroup->addAction( insertText );

	select->setChecked( true );

	QAction * insertImage = m_formToolBar->addAction(
		QIcon( ":/Core/img/insert-image.png" ),
		ProjectWindow::tr( "Insert Image" ) );

	m_formToolBar->addSeparator();

	QAction * group = m_formToolBar->addAction(
		QIcon( ":/Core/img/merge.png" ),
		ProjectWindow::tr( "Group" ) );

	QAction * ungroup = m_formToolBar->addAction(
		QIcon( ":/Core/img/split.png" ),
		ProjectWindow::tr( "Ungroup" ) );

	m_formToolBar->addSeparator();

	QAction * strokeColor = m_formToolBar->addAction(
		QIcon( ":/Core/img/format-stroke-color.png" ),
		ProjectWindow::tr( "Line Color" ) );

	QAction * fillColor = m_formToolBar->addAction(
		QIcon( ":/Core/img/fill-color.png" ),
		ProjectWindow::tr( "Fill Color" ) );

	q->addToolBar( Qt::LeftToolBarArea, m_formToolBar );

	m_formToolBar->hide();

	ProjectWindow::connect( m_propsAction, &QAction::toggled,
		q, &ProjectWindow::p_showHidePropertiesWindow );
//	ProjectWindow::connect( m_toolsAction, &QAction::toggled,
//		q, &ProjectWindow::p_showHideToolsWindow );
	ProjectWindow::connect( quitAction, &QAction::triggered,
		q, &ProjectWindow::p_quit );
	ProjectWindow::connect( m_grid, &QAction::toggled,
		q, &ProjectWindow::p_showHideGrid );
	ProjectWindow::connect( m_gridStep, &QAction::triggered,
		q, &ProjectWindow::p_setGridStep );
	ProjectWindow::connect( newForm, &QAction::triggered,
		m_widget, &ProjectWidget::addForm );
	ProjectWindow::connect( newProject, &QAction::triggered,
		q, &ProjectWindow::p_newProject );
	ProjectWindow::connect( openProject, &QAction::triggered,
		q, &ProjectWindow::p_openProject );
	ProjectWindow::connect( m_saveProject, &QAction::triggered,
		q, &ProjectWindow::p_saveProject );
	ProjectWindow::connect( saveProjectAs, &QAction::triggered,
		q, &ProjectWindow::p_saveProjectAs );
	ProjectWindow::connect( m_widget, &ProjectWidget::changed,
		q, &ProjectWindow::p_projectChanged );
	ProjectWindow::connect( select, &QAction::triggered,
		q, &ProjectWindow::p_select );
	ProjectWindow::connect( move, &QAction::triggered,
		q, &ProjectWindow::p_move );
	ProjectWindow::connect( drawPolyline, &QAction::triggered,
		q, &ProjectWindow::p_drawPolyline );
	ProjectWindow::connect( insertText, &QAction::triggered,
		q, &ProjectWindow::p_insertText );
	ProjectWindow::connect( insertImage, &QAction::triggered,
		q, &ProjectWindow::p_insertImage );
	ProjectWindow::connect( group, &QAction::triggered,
		q, &ProjectWindow::p_group );
	ProjectWindow::connect( ungroup, &QAction::triggered,
		q, &ProjectWindow::p_ungroup );
	ProjectWindow::connect( strokeColor, &QAction::triggered,
		q, &ProjectWindow::p_strokeColor );
	ProjectWindow::connect( fillColor, &QAction::triggered,
		q, &ProjectWindow::p_fillColor );
	ProjectWindow::connect( m_widget->tabs(), &QTabWidget::currentChanged,
		q, &ProjectWindow::p_tabChanged );
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

QAction *
ProjectWindow::showHideGridAction() const
{
	return d->m_grid;
}

QAction *
ProjectWindow::gridStepAction() const
{
	return d->m_gridStep;
}

void
ProjectWindow::hidePropsWindow()
{
	d->m_propsAction->setChecked( false );
}

void
ProjectWindow::hideToolsWindow()
{
//	d->m_toolsAction->setChecked( false );
}

void
ProjectWindow::readProject( const QString & fileName )
{
	try {
		Cfg::TagProject tag;

		QtConfFile::readQtConfFile( tag, fileName,
			QTextCodec::codecForName( "UTF-8" ) );

		p_newProject();

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

	p_quit();
}

void
ProjectWindow::p_showHidePropertiesWindow( bool show )
{
	if( show )
		TopGui::instance()->propsWindow()->show();
	else
		TopGui::instance()->propsWindow()->hide();
}

void
ProjectWindow::p_showHideToolsWindow( bool show )
{
	if( show )
		TopGui::instance()->toolsWindow()->show();
	else
		TopGui::instance()->toolsWindow()->hide();
}

void
ProjectWindow::p_quit()
{
	if( isWindowModified() )
	{
		QMessageBox::StandardButton btn =
			QMessageBox::question( this, tr( "Do You Want to Save Project..." ),
				tr( "Do you want to save project?" ),
				QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes );

		if( btn == QMessageBox::Yes )
			p_saveProjectImpl();
	}

	TopGui::instance()->saveCfg( 0 );

	QApplication::quit();
}

void
ProjectWindow::p_showHideGrid( bool show )
{
	Form::GridMode mode = ( show ? Form::ShowGrid : Form::NoGrid );

	d->m_cfg.setShowGrid( show );

	foreach( FormView * view, d->m_widget->forms() )
		view->form()->setGridMode( mode );
}

void
ProjectWindow::setGridStep( int step, bool forAll )
{
	const int index = d->m_widget->tabs()->currentIndex();

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
ProjectWindow::p_setGridStep()
{
	const int index = d->m_widget->tabs()->currentIndex();

	int step = d->m_cfg.defaultGridStep();
	bool forAll = true;

	if( index > 0 )
		step = d->m_widget->forms().at( index - 1 )->form()->gridStep();

	GridStepDlg dlg( step, forAll, this );

	setGridStep( step, forAll );
}

void
ProjectWindow::p_openProject()
{
	const QString fileName =
		QFileDialog::getOpenFileName( this, tr( "Select Project to Open..." ),
			QStandardPaths::standardLocations(
				QStandardPaths::DocumentsLocation ).first(),
			tr( "Prototyper Project (*.prototyper)" ) );

	if( !fileName.isEmpty() )
		readProject( fileName );
}

void
ProjectWindow::p_newProject()
{
	if( isWindowModified() )
	{
		QMessageBox::StandardButton btn =
			QMessageBox::question( this, tr( "Project Modified..." ),
				tr( "Project modified.\nDo you want to save it?" ),
				QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes );

		if( btn == QMessageBox::Yes )
			p_saveProjectImpl();
	}

	d->m_widget->newProject();

	setWindowTitle( tr( "Prototyper - Unsaved[*]" ) );

	setWindowModified( false );

	d->m_saveProject->setEnabled( true );
}

void
ProjectWindow::p_saveProjectImpl( const QString & fileName )
{
	static const QString ext = QLatin1String( ".prototyper" );

	if( !fileName.isEmpty() )
		d->m_fileName = fileName;

	if( !d->m_fileName.isEmpty() )
	{
		Cfg::ProjectDesc desc;
		desc.setText( d->m_widget->descriptionTab()->editor()->text() );
		desc.setTabName( d->m_widget->tabs()->tabText( 0 ) );

		d->m_cfg.setDescription( desc );

		if( !d->m_fileName.endsWith( ext ) )
			d->m_fileName.append( ext );

		QList< Cfg::Form > forms;

		for( int i = 0; i < d->m_widget->forms().size(); ++i )
		{
			const FormView * view = d->m_widget->forms().at( i );

			Cfg::Form f;
			f.setGridStep( view->form()->gridStep() );
			Cfg::Size s;
			s.setWidth( view->form()->size().width() );
			s.setHeight( view->form()->size().height() );
			f.setSize( s );
			f.setTabName( d->m_widget->tabs()->tabText( i + 1 ) );

			forms.append( f );
		}

		d->m_cfg.setForm( forms );

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
		p_saveProjectAs();
}

void
ProjectWindow::p_saveProject()
{
	p_saveProjectImpl();
}

void
ProjectWindow::p_saveProjectAs()
{
	const QString fileName = QFileDialog::getSaveFileName( this,
		tr( "Select File to Save Project..." ),
		QStandardPaths::standardLocations(
			QStandardPaths::DocumentsLocation ).first(),
		tr( "Prototyper Project (*.prototyper)" ) );

	if( !fileName.isEmpty() )
	{
		setWindowTitle( tr( "Prototyper - %1[*]" )
			.arg( QFileInfo( fileName ).baseName() ) );

		p_saveProjectImpl( fileName );
	}
}

void
ProjectWindow::p_projectChanged()
{
	d->m_saveProject->setEnabled( true );

	setWindowModified( true );
}

void
ProjectWindow::p_drawPolyline()
{

}

void
ProjectWindow::p_insertText()
{

}

void
ProjectWindow::p_insertImage()
{

}

void
ProjectWindow::p_group()
{

}

void
ProjectWindow::p_ungroup()
{

}

void
ProjectWindow::p_select()
{

}

void
ProjectWindow::p_move()
{

}

void
ProjectWindow::p_fillColor()
{

}

void
ProjectWindow::p_strokeColor()
{

}

void
ProjectWindow::p_tabChanged( int index )
{
	if( index > 0 )
	{
		d->m_formToolBar->show();
		d->m_widget->descriptionTab()->toolBar()->hide();
	}
	else
	{
		d->m_formToolBar->hide();
		d->m_widget->descriptionTab()->toolBar()->show();
	}
}

} /* namespace Core */

} /* namespace Prototyper */
