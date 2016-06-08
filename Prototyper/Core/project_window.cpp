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
#include "form.hpp"
#include "form_view.hpp"
#include "form_scene.hpp"
#include "grid_step_dlg.hpp"
#include "project_cfg.hpp"
#include "session_cfg.hpp"
#include "project_description_tab.hpp"
#include "text_editor.hpp"
#include "form_actions.hpp"
#include "form_hierarchy_widget.hpp"
#include "tabs_list.hpp"
#include "form_object.hpp"
#include "form_text.hpp"
#include "desc_window.hpp"
#include "pdf_exporter.hpp"
#include "html_exporter.hpp"

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
#include <QImage>
#include <QColorDialog>
#include <QMimeData>
#include <QDrag>


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
		,	m_saveProject( 0 )
		,	m_grid( 0 )
		,	m_gridStep( 0 )
		,	m_formToolBar( 0 )
		,	m_formToolBarGroup( 0 )
		,	m_formHierarchy( 0 )
		,	m_tabsList( 0 )
		,	m_drawLine( 0 )
		,	m_select( 0 )
		,	m_desc( 0 )
	{
	}

	//! Init.
	void init();
	//! Set flag to all items on the form.
	void setFlag( FormView * view, QGraphicsItem::GraphicsItemFlag f,
		bool enabled = true );
	//! Set flag to all children.
	void setFlag( const QList< QGraphicsItem* > & children,
		QGraphicsItem::GraphicsItemFlag f, bool enabled );
	//! Enable/disbale editing.
	void enableEditing( FormView * view, bool on = true );
	//! Enable/disable editing.
	void enableEditing( const QList< QGraphicsItem* > & children,
		bool on );
	//! Update cfg.
	void updateCfg();

	//! Parent.
	ProjectWindow * q;
	//! Central widget.
	ProjectWidget * m_widget;
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
	//! Form's hierarchy.
	FormHierarchyWidget * m_formHierarchy;
	//! Tabs list.
	TabsList * m_tabsList;
	//! Draw line action.
	QAction * m_drawLine;
	//! Select action.
	QAction * m_select;
	//! Descriptions window.
	QScopedPointer< DescWindow > m_desc;
}; // class ProjectWindowPrivate

void
ProjectWindowPrivate::init()
{
	m_widget = new ProjectWidget( m_cfg, q );

	m_desc.reset( new DescWindow );

	q->addToolBar( Qt::LeftToolBarArea, m_widget->descriptionTab()->toolBar() );

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

	QMenu * exportMenu = file->addMenu(
		QIcon( ":/Core/img/document-export.png" ),
		ProjectWindow::tr( "Export To" ) );

	QAction * exportToPdf = exportMenu->addAction(
		QIcon( ":/Core/img/application-pdf.png" ),
		ProjectWindow::tr( "PDF" ) );

	QAction * exportToHtml = exportMenu->addAction(
		QIcon( ":/Core/img/text-html.png" ),
		ProjectWindow::tr( "HTML" ) );

	file->addSeparator();

	QAction * quitAction = file->addAction(
		QIcon( ":/Core/img/application-exit.png" ),
		ProjectWindow::tr( "Quit" ) );
	quitAction->setShortcutContext( Qt::ApplicationShortcut );
	quitAction->setShortcut( ProjectWindow::tr( "Ctrl+Q" ) );

	QMenu * view = q->menuBar()->addMenu( ProjectWindow::tr( "&View" ) );

	m_formHierarchy = new FormHierarchyWidget( q );

	q->addDockWidget( Qt::RightDockWidgetArea, m_formHierarchy );

	view->addAction( m_formHierarchy->toggleViewAction() );
	m_formHierarchy->toggleViewAction()->setShortcutContext(
		Qt::ApplicationShortcut );
	m_formHierarchy->toggleViewAction()->setShortcut(
		ProjectWindow::tr( "Ctrl+Alt+H" ) );

	m_tabsList = new TabsList( q );

	q->addDockWidget( Qt::RightDockWidgetArea, m_tabsList );

	view->addAction( m_tabsList->toggleViewAction() );
	m_tabsList->toggleViewAction()->setShortcutContext(
		Qt::ApplicationShortcut );
	m_tabsList->toggleViewAction()->setShortcut(
		ProjectWindow::tr( "Ctrl+Alt+T" ) );

	QAction * newForm = new QAction( QIcon( ":/Core/img/list-add.png" ),
		ProjectWindow::tr( "Add Form" ), q );
	newForm->setShortcutContext( Qt::ApplicationShortcut );
	newForm->setShortcut( ProjectWindow::tr( "Ctrl+T" ) );
	q->addAction( newForm );

	m_formToolBar = new QToolBar( ProjectWindow::tr( "Form Tools" ), q );
	m_formToolBarGroup = new QActionGroup( q );
	m_formToolBarGroup->setExclusive( true );

	m_select = m_formToolBar->addAction(
		QIcon( ":/Core/img/edit-select.png" ),
		ProjectWindow::tr( "Select" ) );
	m_select->setCheckable( true );
	m_formToolBarGroup->addAction( m_select );
	m_select->setShortcutContext( Qt::ApplicationShortcut );
	m_select->setShortcut( ProjectWindow::tr( "Alt+S" ) );

	m_drawLine = m_formToolBar->addAction(
		QIcon( ":/Core/img/draw-freehand.png" ),
		ProjectWindow::tr( "Draw Line" ) );
	m_drawLine->setCheckable( true );
	m_formToolBarGroup->addAction( m_drawLine );
	m_drawLine->setShortcutContext( Qt::ApplicationShortcut );
	m_drawLine->setShortcut( ProjectWindow::tr( "Alt+L" ) );

	QAction * drawPolyLine = m_formToolBar->addAction(
		QIcon( ":/Core/img/draw-polyline.png" ),
		ProjectWindow::tr( "Draw Line" ) );
	drawPolyLine->setCheckable( true );
	drawPolyLine->setShortcutContext( Qt::ApplicationShortcut );
	drawPolyLine->setShortcut( ProjectWindow::tr( "Alt+P" ) );

	QAction * drawRect = m_formToolBar->addAction(
		QIcon( ":/Core/img/draw-rectangle.png" ),
		ProjectWindow::tr( "Draw Rect" ) );
	drawRect->setCheckable( true );
	m_formToolBarGroup->addAction( drawRect );
	drawRect->setShortcutContext( Qt::ApplicationShortcut );
	drawRect->setShortcut( ProjectWindow::tr( "Alt+R" ) );

	QAction * insertText = m_formToolBar->addAction(
		QIcon( ":/Core/img/insert-text.png" ),
		ProjectWindow::tr( "Insert Text" ) );
	insertText->setCheckable( true );
	m_formToolBarGroup->addAction( insertText );
	insertText->setShortcutContext( Qt::ApplicationShortcut );
	insertText->setShortcut( ProjectWindow::tr( "Alt+T" ) );

	m_select->setChecked( true );
	m_widget->enableSelection( true );

	QAction * insertImage = m_formToolBar->addAction(
		QIcon( ":/Core/img/insert-image.png" ),
		ProjectWindow::tr( "Insert Image" ) );
	insertImage->setShortcutContext( Qt::ApplicationShortcut );
	insertImage->setShortcut( ProjectWindow::tr( "Alt+I" ) );

	m_formToolBar->addSeparator();

	QAction * group = m_formToolBar->addAction(
		QIcon( ":/Core/img/merge.png" ),
		ProjectWindow::tr( "Group" ) );
	group->setShortcutContext( Qt::ApplicationShortcut );
	group->setShortcut( ProjectWindow::tr( "Ctrl+G" ) );

	QAction * ungroup = m_formToolBar->addAction(
		QIcon( ":/Core/img/split.png" ),
		ProjectWindow::tr( "Ungroup" ) );
	ungroup->setShortcutContext( Qt::ApplicationShortcut );
	ungroup->setShortcut( ProjectWindow::tr( "Ctrl+U" ) );

	m_formToolBar->addSeparator();

	QAction * strokeColor = m_formToolBar->addAction(
		QIcon( ":/Core/img/format-stroke-color.png" ),
		ProjectWindow::tr( "Line Color" ) );

//	QAction * fillColor = m_formToolBar->addAction(
//		QIcon( ":/Core/img/fill-color.png" ),
//		ProjectWindow::tr( "Fill Color" ) );

	q->addToolBar( Qt::LeftToolBarArea, m_formToolBar );

	m_formToolBar->hide();

	QMenu * form = q->menuBar()->addMenu( ProjectWindow::tr( "F&orm" ) );
	m_grid = form->addAction(
		QIcon( ":/Core/img/view-grid.png" ),
		ProjectWindow::tr( "Show Grid" ) );
	m_grid->setShortcutContext( Qt::ApplicationShortcut );
	m_grid->setShortcut( ProjectWindow::tr( "Ctrl+Alt+G" ) );
	m_grid->setCheckable( true );
	m_grid->setChecked( true );

	m_gridStep = form->addAction(
		QIcon( ":/Core/img/measure.png" ),
		ProjectWindow::tr( "Grid Step" ) );

	QAction * snapGrid = form->addAction(
		QIcon( ":/Core/img/snap-intersection.png" ),
		ProjectWindow::tr( "Snap Grid" ) );
	snapGrid->setShortcutContext( Qt::ApplicationShortcut );
	snapGrid->setShortcut( ProjectWindow::tr( "Alt+G" ) );
	snapGrid->setCheckable( true );
	snapGrid->setChecked( true );

	form->addSeparator();

	form->addAction( m_select );
	form->addAction( m_drawLine );
	form->addAction( drawPolyLine );
	form->addAction( drawRect );
	form->addAction( insertText );
	form->addAction( insertImage );

	form->addSeparator();

	form->addAction( group );
	form->addAction( ungroup );

	form->addSeparator();

	form->addAction( newForm );

	QMenu * help = q->menuBar()->addMenu( ProjectWindow::tr( "&Help" ) );
	QAction * about = help->addAction( ProjectWindow::tr( "About" ) );
	QAction * aboutQt = help->addAction( ProjectWindow::tr( "About Qt" ) );

	ProjectWindow::connect( quitAction, &QAction::triggered,
		q, &ProjectWindow::p_quit );
	ProjectWindow::connect( m_grid, &QAction::toggled,
		q, &ProjectWindow::p_showHideGrid );
	ProjectWindow::connect( snapGrid, &QAction::toggled,
		q, &ProjectWindow::p_snapGrid );
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
	ProjectWindow::connect( m_select, &QAction::triggered,
		q, &ProjectWindow::p_select );
	ProjectWindow::connect( m_drawLine, &QAction::triggered,
		q, &ProjectWindow::p_drawLine );
	ProjectWindow::connect( drawRect, &QAction::triggered,
		q, &ProjectWindow::p_drawRect );
	ProjectWindow::connect( drawPolyLine, &QAction::toggled,
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
//	ProjectWindow::connect( fillColor, &QAction::triggered,
//		q, &ProjectWindow::p_fillColor );
	ProjectWindow::connect( m_widget->tabs(), &QTabWidget::currentChanged,
		q, &ProjectWindow::p_tabChanged );
	ProjectWindow::connect( m_desc.data(), &DescWindow::changed,
		q, &ProjectWindow::p_projectChanged );
	ProjectWindow::connect( m_formHierarchy, &FormHierarchyWidget::changed,
		q, &ProjectWindow::p_projectChanged );
	ProjectWindow::connect( exportToPdf, &QAction::triggered,
		q, &ProjectWindow::p_exportToPDf );
	ProjectWindow::connect( exportToHtml, &QAction::triggered,
		q, &ProjectWindow::p_exportToHtml );
	ProjectWindow::connect( about, &QAction::triggered,
		q, &ProjectWindow::p_about );
	ProjectWindow::connect( aboutQt, &QAction::triggered,
		q, &ProjectWindow::p_aboutQt );
}

void
ProjectWindowPrivate::setFlag( FormView * view,
	QGraphicsItem::GraphicsItemFlag f, bool enabled )
{
	setFlag( view->form()->childItems(), f, enabled );
}

void
ProjectWindowPrivate::setFlag( const QList< QGraphicsItem* > & children,
	QGraphicsItem::GraphicsItemFlag f, bool enabled )
{
	foreach( QGraphicsItem * item, children )
	{
		setFlag( item->childItems(), f, enabled );

		FormObject * tmp = dynamic_cast< FormObject* > ( item );

		if( tmp )
			item->setFlag( f, enabled );
	}
}

void
ProjectWindowPrivate::enableEditing( FormView * view, bool on )
{
	enableEditing( view->form()->childItems(), on );
}

void
ProjectWindowPrivate::enableEditing( const QList< QGraphicsItem* > & children,
	bool on )
{
	foreach( QGraphicsItem * item, children )
	{
		FormText * text = dynamic_cast< FormText* > ( item );

		if( text )
		{
			text->enableEditing( on );
			text->clearSelection();
		}
	}
}

void
ProjectWindowPrivate::updateCfg()
{
	m_cfg.description().setText(
		m_widget->descriptionTab()->editor()->text() );

	for( int i = 0; i < m_widget->forms().size(); ++i )
		m_cfg.form()[ i ] = m_widget->forms().at( i )->form()->cfg();
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

DescWindow *
ProjectWindow::descWindow() const
{
	return d->m_desc.data();
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

FormHierarchyWidget *
ProjectWindow::formHierarchy() const
{
	return d->m_formHierarchy;
}

TabsList *
ProjectWindow::tabsList() const
{
	return d->m_tabsList;
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

		d->m_select->trigger();
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
ProjectWindow::postConstruction()
{
	d->m_formHierarchy->postConstruction();
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

	setWindowModified( true );
}

void
ProjectWindow::p_snapGrid( bool on )
{
	foreach( FormView * view, d->m_widget->forms() )
		view->form()->enableSnap( on );

	FormAction::instance()->enableSnap( on );
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

			setWindowModified( true );
		}
		else if( index > 0 )
		{
			d->m_widget->forms()[ index - 1 ]->form()->setGridStep(
				dlg.gridStep() );

			setWindowModified( true );
		}
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

	d->m_fileName.clear();

	setWindowTitle( tr( "Prototyper - Unsaved[*]" ) );

	setWindowModified( false );

	d->m_select->trigger();
}

void
ProjectWindow::p_saveProjectImpl( const QString & fileName )
{
	static const QString ext = QLatin1String( ".prototyper" );

	if( !fileName.isEmpty() )
		d->m_fileName = fileName;

	if( !d->m_fileName.isEmpty() )
	{
		d->updateCfg();

		if( !d->m_fileName.endsWith( ext ) )
			d->m_fileName.append( ext );

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
	setWindowModified( true );
}

void
ProjectWindow::p_drawLine()
{
	FormAction::instance()->setMode( FormAction::DrawLine );

	d->m_widget->enableSelection( false );

	foreach( FormView * v, d->m_widget->forms() )
	{
		v->form()->setCursor( Qt::CrossCursor );

		v->form()->switchToLineDrawingMode();

		d->setFlag( v, QGraphicsItem::ItemIsSelectable, false );

		d->enableEditing( v, false );
	}
}

void
ProjectWindow::p_drawRect()
{
	FormAction::instance()->setMode( FormAction::DrawRect );

	d->m_widget->enableSelection( false );

	foreach( FormView * v, d->m_widget->forms() )
	{
		v->form()->setCursor( Qt::CrossCursor );

		v->form()->switchToLineDrawingMode();

		d->setFlag( v, QGraphicsItem::ItemIsSelectable, false );

		d->enableEditing( v, false );
	}
}

void
ProjectWindow::p_drawPolyline( bool on )
{
	if( on )
	{
		d->m_drawLine->trigger();

		FormAction::instance()->setFlag( FormAction::Polyline );
	}
	else
		FormAction::instance()->clearFlag( FormAction::Polyline );
}

void
ProjectWindow::p_insertText()
{
	FormAction::instance()->setMode( FormAction::InsertText );

	d->m_widget->enableSelection( false );

	foreach( FormView * v, d->m_widget->forms() )
	{
		v->form()->setCursor( Qt::CrossCursor );

		d->setFlag( v, QGraphicsItem::ItemIsSelectable, false );

		d->enableEditing( v, true );
	}
}

void
ProjectWindow::p_insertImage()
{
	const QString fileName =
		QFileDialog::getOpenFileName( this, tr( "Select Image" ),
			QStandardPaths::standardLocations(
				QStandardPaths::PicturesLocation ).first(),
			tr( "Image Files (*.png *.jpg *.jpeg *.bmp)" ), 0,
			QFileDialog::DontUseNativeDialog );

	QApplication::processEvents();

	if( !fileName.isEmpty() )
	{
		QImage image( fileName );

		if( !image.isNull() )
		{
			QApplication::processEvents();

			QDrag * drag = new QDrag( this );
			QMimeData * mimeData = new QMimeData;

			QPixmap p;
			QSize s = image.size();

			if( s.width() > 50 || s.height() > 50 )
			{
				s = s.boundedTo( QSize( 50, 50 ) );
				p = QPixmap::fromImage(
					image.scaled( s, Qt::KeepAspectRatio, Qt::SmoothTransformation ) );
			}
			else
				p = QPixmap::fromImage( image );

			mimeData->setImageData( image );
			drag->setMimeData( mimeData );
			drag->setPixmap( p );

			QApplication::processEvents();

			drag->exec();
		}
		else
			QMessageBox::warning( this, tr( "Wrong Image..." ),
				tr( "Failed to load image from \"%1\"." ).arg( fileName ) );
	}
}

void
ProjectWindow::p_group()
{
	FormAction::instance()->form()->group();
}

void
ProjectWindow::p_ungroup()
{
	FormAction::instance()->form()->ungroup();
}

void
ProjectWindow::p_select()
{
	FormAction::instance()->setMode( FormAction::Select );

	d->m_widget->enableSelection( true );

	foreach( FormView * v, d->m_widget->forms() )
	{
		v->form()->setCursor( Qt::ArrowCursor );

		d->setFlag( v, QGraphicsItem::ItemIsSelectable, true );

		d->enableEditing( v, false );

		v->form()->switchToSelectMode();
	}
}

void
ProjectWindow::p_fillColor()
{
	const QColor c = QColorDialog::getColor(
		FormAction::instance()->fillColor(),
		this, tr( "Select Fill Color..." ),
		QColorDialog::ShowAlphaChannel );

	if( c.isValid() )
	{
		FormAction::instance()->setFillColor( c );

		if( FormAction::instance()->form() )
		{
			QList< QGraphicsItem* > selected =
				FormAction::instance()->form()->scene()->selectedItems();

			if( !selected.isEmpty() )
			{
				foreach( QGraphicsItem * item, selected )
				{
					FormObject * obj = dynamic_cast< FormObject* > ( item );

					if( obj )
						obj->setObjectBrush( QBrush( c ) );
				}
			}
		}
	}
}

void
ProjectWindow::p_strokeColor()
{
	const QColor c = QColorDialog::getColor(
		FormAction::instance()->strokeColor(),
		this, tr( "Select Stroke Color..." ),
		QColorDialog::ShowAlphaChannel );

	if( c.isValid() )
	{
		FormAction::instance()->setStrokeColor( c );

		if( FormAction::instance()->form() )
		{
			QList< QGraphicsItem* > selected =
				FormAction::instance()->form()->scene()->selectedItems();

			if( !selected.isEmpty() )
			{
				foreach( QGraphicsItem * item, selected )
				{
					FormObject * obj = dynamic_cast< FormObject* > ( item );

					if( obj )
						obj->setObjectPen( QPen( c, 2.0 ) );
				}
			}
		}
	}
}

void
ProjectWindow::p_tabChanged( int index )
{
	if( index > 0 )
	{
		d->m_formToolBar->show();
		d->m_widget->descriptionTab()->toolBar()->hide();

		FormAction::instance()->setForm(
			d->m_widget->forms().at( index - 1 )->form() );
	}
	else
	{
		d->m_formToolBar->hide();
		d->m_widget->descriptionTab()->toolBar()->show();

		FormAction::instance()->setForm( 0 );
	}
}

void
ProjectWindow::p_exportToPDf()
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

	QString fileName = QFileDialog::getSaveFileName( this,
		tr( "Select file to export project..." ),
		QStandardPaths::standardLocations(
			QStandardPaths::DocumentsLocation ).first(),
		tr( "PDF (*.pdf)" ) );

	if( !fileName.endsWith( QLatin1String( ".pdf" ), Qt::CaseInsensitive ) )
		fileName.append( QLatin1String( ".pdf" ) );

	if( !fileName.isEmpty() )
	{
		d->updateCfg();

		PdfExporter exporter( d->m_cfg );

		exporter.exportToDoc( fileName );
	}
}

void
ProjectWindow::p_exportToHtml()
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

	QString fileName = QFileDialog::getSaveFileName( this,
		tr( "Select file to export project..." ),
		QStandardPaths::standardLocations(
			QStandardPaths::DocumentsLocation ).first(),
		tr( "HTML (*.htm *.html)" ) );

	if( !fileName.endsWith( QLatin1String( ".htm" ), Qt::CaseInsensitive ) &&
		!fileName.endsWith( QLatin1String( ".html" ), Qt::CaseInsensitive ) )
		fileName.append( QLatin1String( ".html" ) );

	if( !fileName.isEmpty() )
	{
		d->updateCfg();

		HtmlExporter exporter( d->m_cfg );

		exporter.exportToDoc( fileName );
	}
}

void
ProjectWindow::p_about()
{
	QMessageBox::about( this, tr( "About Prototyper" ),
		tr( "Prototyper - Simple UI prototyping software.\n\n"
			"Author - Igor Mironchik (igor.mironchik at gmail dot com).\n\n"
			"Copyright (c) 2016 Igor Mironchik.\n\n"
			"Licensed under GNU GPL 3.0." ) );
}

void
ProjectWindow::p_aboutQt()
{
	QMessageBox::aboutQt( this );
}

} /* namespace Core */

} /* namespace Prototyper */
