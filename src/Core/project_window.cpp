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
#include "page.hpp"
#include "page_view.hpp"
#include "page_scene.hpp"
#include "grid_step_dlg.hpp"
#include "project_cfg.hpp"
#include "session_cfg.hpp"
#include "project_description_tab.hpp"
#include "text_editor.hpp"
#include "form_actions.hpp"
#include "tabs_list.hpp"
#include "form_object.hpp"
#include "form_text.hpp"
#include "pdf_exporter.hpp"
#include "html_exporter.hpp"
#include "svg_exporter.hpp"

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
#include <QUndoStack>
#include <QUndoGroup>
#include <QTextCodec>
#include <QTextStream>
#include <QFile>
#include <QStringListModel>


namespace Prototyper {

namespace Core {

//
// ProjectWindowPrivate
//

class ProjectWindowPrivate {
public:
	explicit ProjectWindowPrivate( ProjectWindow * parent )
		:	q( parent )
		,	m_widget( Q_NULLPTR )
		,	m_saveProject( Q_NULLPTR )
		,	m_grid( Q_NULLPTR )
		,	m_gridStep( Q_NULLPTR )
		,	m_formToolBar( Q_NULLPTR )
		,	m_stdItemsToolBar( Q_NULLPTR )
		,	m_formToolBarGroup( Q_NULLPTR )
		,	m_tabsList( Q_NULLPTR )
		,	m_drawLine( Q_NULLPTR )
		,	m_select( Q_NULLPTR )
		,	m_drawPolyLine( Q_NULLPTR )
		,	m_undoAction( Q_NULLPTR )
		,	m_redoAction( Q_NULLPTR )
		,	m_strokeColor( nullptr )
		,	m_group( nullptr )
		,	m_ungroup( nullptr )
		,	m_alignHorizLeft( nullptr )
		,	m_alignHorizCenter( nullptr )
		,	m_alignHorizRight( nullptr )
		,	m_alignVertTop( nullptr )
		,	m_alignVertCenter( nullptr )
		,	m_alignVertBottom( nullptr )
	{
	}

	//! Init.
	void init();
	//! Set flag to all items on the form.
	void setFlag( PageView * view, QGraphicsItem::GraphicsItemFlag f,
		bool enabled = true );
	//! Set flag to all children.
	void setFlag( const QList< QGraphicsItem* > & children,
		QGraphicsItem::GraphicsItemFlag f, bool enabled );
	//! Enable/disbale editing.
	void enableEditing( PageView * view, bool on = true );
	//! Enable/disable editing.
	void enableEditing( const QList< QGraphicsItem* > & children,
		bool on );
	//! Update cfg.
	void updateCfg();
	//! Prepare to draw with rect placer.
	void prepareDrawingWithRectPlacer( bool editable = false );

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
	//! Standard items' toolbar.
	QToolBar * m_stdItemsToolBar;
	//! Form toolbar group.
	QActionGroup * m_formToolBarGroup;
	//! Tabs list.
	TabsList * m_tabsList;
	//! Draw line action.
	QAction * m_drawLine;
	//! Select action.
	QAction * m_select;
	//! Draw polyline action.
	QAction * m_drawPolyLine;
	//! Undo action.
	QAction * m_undoAction;
	//! Redo action.
	QAction * m_redoAction;
	//! Stroke color action.
	QAction * m_strokeColor;
	//! Group action.
	QAction * m_group;
	//! Ungroup action.
	QAction * m_ungroup;
	QAction * m_alignHorizLeft;
	QAction * m_alignHorizCenter;
	QAction * m_alignHorizRight;
	QAction * m_alignVertTop;
	QAction * m_alignVertCenter;
	QAction * m_alignVertBottom;
	//! Added forms.
	QList< PageView* > m_addedForms;
	//! Deleted forms.
	QList< PageView* > m_deletedForms;
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

	QMenu * exportMenu = file->addMenu(
		QIcon( ":/Core/img/document-export.png" ),
		ProjectWindow::tr( "Export To" ) );

	QAction * exportToPdf = exportMenu->addAction(
		QIcon( ":/Core/img/application-pdf.png" ),
		ProjectWindow::tr( "PDF" ) );

	QAction * exportToHtml = exportMenu->addAction(
		QIcon( ":/Core/img/text-html.png" ),
		ProjectWindow::tr( "HTML" ) );

	QAction * exportToSvg = exportMenu->addAction(
		QIcon( ":/Core/img/image-svg+xml.png" ),
		ProjectWindow::tr( "SVG Images" ) );

	file->addSeparator();

	QAction * quitAction = file->addAction(
		QIcon( ":/Core/img/application-exit.png" ),
		ProjectWindow::tr( "Quit" ) );
	quitAction->setShortcutContext( Qt::ApplicationShortcut );
	quitAction->setShortcut( ProjectWindow::tr( "Ctrl+Q" ) );

	QMenu * view = q->menuBar()->addMenu( ProjectWindow::tr( "&Dock" ) );

	m_tabsList = new TabsList( q );
	m_tabsList->setObjectName( QLatin1String( "m_tabsList" ) );

	m_tabsList->model()->setStringList( QStringList() << m_widget->projectTabName() );

	q->addDockWidget( Qt::LeftDockWidgetArea, m_tabsList );

	view->addAction( m_tabsList->toggleViewAction() );
	m_tabsList->toggleViewAction()->setShortcutContext(
		Qt::ApplicationShortcut );
	m_tabsList->toggleViewAction()->setShortcut(
		ProjectWindow::tr( "Ctrl+Alt+T" ) );

	QAction * newForm = new QAction( QIcon( ":/Core/img/list-add.png" ),
		ProjectWindow::tr( "Add Page" ), q );
	newForm->setShortcutContext( Qt::ApplicationShortcut );
	newForm->setShortcut( ProjectWindow::tr( "Ctrl+T" ) );
	q->addAction( newForm );

	m_formToolBar = new QToolBar( ProjectWindow::tr( "Drawing Tools" ), q );
	m_formToolBar->setObjectName( QLatin1String( "m_formToolBar" ) );

	m_stdItemsToolBar = new QToolBar( ProjectWindow::tr( "Standard Items" ), q );
	m_stdItemsToolBar->setObjectName( QLatin1String( "m_stdItemsToolBar" ) );

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

	m_drawPolyLine = m_formToolBar->addAction(
		QIcon( ":/Core/img/draw-polyline.png" ),
		ProjectWindow::tr( "Draw Polyline" ) );
	m_drawPolyLine->setCheckable( true );
	m_drawPolyLine->setShortcutContext( Qt::ApplicationShortcut );
	m_drawPolyLine->setShortcut( ProjectWindow::tr( "Alt+P" ) );

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

	QAction * insertImage = m_formToolBar->addAction(
		QIcon( ":/Core/img/insert-image.png" ),
		ProjectWindow::tr( "Insert Image" ) );
	insertImage->setShortcutContext( Qt::ApplicationShortcut );
	insertImage->setShortcut( ProjectWindow::tr( "Alt+I" ) );

	m_formToolBar->addSeparator();

	m_group = m_formToolBar->addAction(
		QIcon( ":/Core/img/merge.png" ),
		ProjectWindow::tr( "Group" ) );
	m_group->setShortcutContext( Qt::ApplicationShortcut );
	m_group->setShortcut( ProjectWindow::tr( "Ctrl+G" ) );

	m_ungroup = m_formToolBar->addAction(
		QIcon( ":/Core/img/split.png" ),
		ProjectWindow::tr( "Ungroup" ) );
	m_ungroup->setShortcutContext( Qt::ApplicationShortcut );
	m_ungroup->setShortcut( ProjectWindow::tr( "Ctrl+U" ) );

	m_formToolBar->addSeparator();

	m_strokeColor = m_formToolBar->addAction(
		QIcon( ":/Core/img/format-stroke-color.png" ),
		ProjectWindow::tr( "Line Color" ) );

//	QAction * fillColor = m_formToolBar->addAction(
//		QIcon( ":/Core/img/fill-color.png" ),
//		ProjectWindow::tr( "Fill Color" ) );

	QAction * drawButton = m_stdItemsToolBar->addAction(
		QIcon( ":/Core/img/draw-pushbutton.png" ),
		ProjectWindow::tr( "Draw Button" ) );
	drawButton->setCheckable( true );
	m_formToolBarGroup->addAction( drawButton );
	drawButton->setShortcutContext( Qt::ApplicationShortcut );
	drawButton->setShortcut( ProjectWindow::tr( "Alt+B" ) );

	QAction * drawComboBox = m_stdItemsToolBar->addAction(
		QIcon( ":/Core/img/draw-combobox.png" ),
		ProjectWindow::tr( "Draw ComboBox" ) );
	drawComboBox->setCheckable( true );
	m_formToolBarGroup->addAction( drawComboBox );
	drawComboBox->setShortcutContext( Qt::ApplicationShortcut );
	drawComboBox->setShortcut( ProjectWindow::tr( "Alt+C" ) );

	QAction * drawRadioButton = m_stdItemsToolBar->addAction(
		QIcon( ":/Core/img/draw-radiobutton.png" ),
		ProjectWindow::tr( "Draw Radio Button" ) );
	drawRadioButton->setCheckable( true );
	m_formToolBarGroup->addAction( drawRadioButton );
	drawRadioButton->setShortcutContext( Qt::ApplicationShortcut );
	drawRadioButton->setShortcut( ProjectWindow::tr( "Alt+R" ) );

	QAction * drawCheckBox = m_stdItemsToolBar->addAction(
		QIcon( ":/Core/img/draw-checkbox.png" ),
		ProjectWindow::tr( "Draw Check Box" ) );
	drawCheckBox->setCheckable( true );
	m_formToolBarGroup->addAction( drawCheckBox );
	drawCheckBox->setShortcutContext( Qt::ApplicationShortcut );
	drawCheckBox->setShortcut( ProjectWindow::tr( "Alt+K" ) );

	QAction * drawHSlider = m_stdItemsToolBar->addAction(
		QIcon( ":/Core/img/draw-hslider.png" ),
		ProjectWindow::tr( "Draw Horizontal Slider" ) );
	drawHSlider->setCheckable( true );
	m_formToolBarGroup->addAction( drawHSlider );
	drawHSlider->setShortcutContext( Qt::ApplicationShortcut );
	drawHSlider->setShortcut( ProjectWindow::tr( "Alt+H" ) );

	QAction * drawVSlider = m_stdItemsToolBar->addAction(
		QIcon( ":/Core/img/draw-vslider.png" ),
		ProjectWindow::tr( "Draw Vertical Slider" ) );
	drawVSlider->setCheckable( true );
	m_formToolBarGroup->addAction( drawVSlider );
	drawVSlider->setShortcutContext( Qt::ApplicationShortcut );
	drawVSlider->setShortcut( ProjectWindow::tr( "Alt+V" ) );

	QAction * drawSpinbox = m_stdItemsToolBar->addAction(
		QIcon( ":/Core/img/draw-spinbox.png" ),
		ProjectWindow::tr( "Draw Spinbox" ) );
	drawSpinbox->setCheckable( true );
	m_formToolBarGroup->addAction( drawSpinbox );
	drawSpinbox->setShortcutContext( Qt::ApplicationShortcut );
	drawSpinbox->setShortcut( ProjectWindow::tr( "Alt+S" ) );

	m_formToolBar->addSeparator();

	m_alignHorizLeft = m_formToolBar->addAction(
		QIcon( ":/Core/img/align-horizontal-left.png" ),
		ProjectWindow::tr( "Align Horizontal Left" ) );
	m_alignHorizLeft->setShortcutContext( Qt::ApplicationShortcut );
	m_alignHorizLeft->setShortcut( ProjectWindow::tr( "Ctrl+Alt+L" ) );

	m_alignHorizCenter = m_formToolBar->addAction(
		QIcon( ":/Core/img/align-horizontal-center.png" ),
		ProjectWindow::tr( "Align Horizontal Center" ) );
	m_alignHorizCenter->setShortcutContext( Qt::ApplicationShortcut );
	m_alignHorizCenter->setShortcut( ProjectWindow::tr( "Ctrl+Alt+C" ) );

	m_alignHorizRight = m_formToolBar->addAction(
		QIcon( ":/Core/img/align-horizontal-right.png" ),
		ProjectWindow::tr( "Align Horizontal Right" ) );
	m_alignHorizRight->setShortcutContext( Qt::ApplicationShortcut );
	m_alignHorizRight->setShortcut( ProjectWindow::tr( "Ctrl+Alt+R" ) );

	m_alignVertTop = m_formToolBar->addAction(
		QIcon( ":/Core/img/align-vertical-top.png" ),
		ProjectWindow::tr( "Align Vertical Top" ) );
	m_alignVertTop->setShortcutContext( Qt::ApplicationShortcut );
	m_alignVertTop->setShortcut( ProjectWindow::tr( "Ctrl+Alt+T" ) );

	m_alignVertCenter = m_formToolBar->addAction(
		QIcon( ":/Core/img/align-vertical-center.png" ),
		ProjectWindow::tr( "Align Vertical Center" ) );
	m_alignVertCenter->setShortcutContext( Qt::ApplicationShortcut );
	m_alignVertCenter->setShortcut( ProjectWindow::tr( "Ctrl+Alt+V" ) );

	m_alignVertBottom = m_formToolBar->addAction(
		QIcon( ":/Core/img/align-vertical-bottom.png" ),
		ProjectWindow::tr( "Align Vertical Bottom" ) );
	m_alignVertBottom->setShortcutContext( Qt::ApplicationShortcut );
	m_alignVertBottom->setShortcut( ProjectWindow::tr( "Ctrl+Alt+B" ) );

	q->addToolBar( Qt::TopToolBarArea, m_formToolBar );

	q->addToolBar( Qt::TopToolBarArea, m_stdItemsToolBar );

	m_formToolBar->hide();

	m_stdItemsToolBar->hide();

	QMenu * form = q->menuBar()->addMenu( ProjectWindow::tr( "D&rawing" ) );

	form->addAction( newForm );

	form->addSeparator();

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
	form->addAction( m_drawPolyLine );
	form->addAction( drawRect );
	form->addAction( insertText );
	form->addAction( insertImage );
	form->addAction( drawButton );
	form->addAction( drawComboBox );
	form->addAction( drawRadioButton );
	form->addAction( drawCheckBox );
	form->addAction( drawHSlider );
	form->addAction( drawVSlider );
	form->addAction( drawSpinbox );

	form->addSeparator();

	form->addAction( m_alignHorizLeft );
	form->addAction( m_alignHorizCenter );
	form->addAction( m_alignHorizRight );
	form->addAction( m_alignVertTop );
	form->addAction( m_alignVertCenter );
	form->addAction( m_alignVertBottom );

	form->addSeparator();

	form->addAction( m_group );
	form->addAction( m_ungroup );

	form->addSeparator();

	QAction * undoAction = m_widget->undoGroup()->createUndoAction( q );
	undoAction->setShortcutContext( Qt::ApplicationShortcut );
	undoAction->setShortcut( ProjectWindow::tr( "Ctrl+Z" ) );
	undoAction->setIcon( QIcon( ":/Core/img/edit-undo.png" ) );
	form->addAction( undoAction );

	QAction * redoAction = m_widget->undoGroup()->createRedoAction( q );
	redoAction->setShortcutContext( Qt::ApplicationShortcut );
	redoAction->setShortcut( ProjectWindow::tr( "Ctrl+Y" ) );
	redoAction->setIcon( QIcon( ":/Core/img/edit-redo.png" ) );
	form->addAction( redoAction );

	QMenu * help = q->menuBar()->addMenu( ProjectWindow::tr( "&Help" ) );
	QAction * about = help->addAction( QIcon( ":/Core/img/prototyper.png" ),
		ProjectWindow::tr( "About" ) );
	QAction * aboutQt = help->addAction( QIcon( ":/Core/img/qt.png" ),
		ProjectWindow::tr( "About Qt" ) );

	ProjectWindow::connect( quitAction, &QAction::triggered,
		q, &ProjectWindow::p_quit );
	ProjectWindow::connect( m_grid, &QAction::toggled,
		q, &ProjectWindow::p_showHideGrid );
	ProjectWindow::connect( snapGrid, &QAction::toggled,
		q, &ProjectWindow::p_snapGrid );
	ProjectWindow::connect( m_gridStep, &QAction::triggered,
		q, &ProjectWindow::p_setGridStep );
	ProjectWindow::connect( newForm, &QAction::triggered,
		m_widget, &ProjectWidget::addPage );
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
	ProjectWindow::connect( m_select, &QAction::toggled,
		q, &ProjectWindow::p_select );
	ProjectWindow::connect( m_drawLine, &QAction::triggered,
		q, &ProjectWindow::p_drawLine );
	ProjectWindow::connect( drawRect, &QAction::triggered,
		q, &ProjectWindow::p_drawRect );
	ProjectWindow::connect( m_drawPolyLine, &QAction::toggled,
		q, &ProjectWindow::p_drawPolyline );
	ProjectWindow::connect( insertText, &QAction::triggered,
		q, &ProjectWindow::p_insertText );
	ProjectWindow::connect( insertImage, &QAction::triggered,
		q, &ProjectWindow::p_insertImage );
	ProjectWindow::connect( m_group, &QAction::triggered,
		q, &ProjectWindow::p_group );
	ProjectWindow::connect( m_ungroup, &QAction::triggered,
		q, &ProjectWindow::p_ungroup );
	ProjectWindow::connect( m_strokeColor, &QAction::triggered,
		q, &ProjectWindow::p_strokeColor );
//	ProjectWindow::connect( fillColor, &QAction::triggered,
//		q, &ProjectWindow::p_fillColor );
	ProjectWindow::connect( m_widget->tabs(), &QTabWidget::currentChanged,
		q, &ProjectWindow::p_tabChanged );
	ProjectWindow::connect( exportToPdf, &QAction::triggered,
		q, &ProjectWindow::p_exportToPDf );
	ProjectWindow::connect( exportToHtml, &QAction::triggered,
		q, &ProjectWindow::p_exportToHtml );
	ProjectWindow::connect( exportToSvg, &QAction::triggered,
		q, &ProjectWindow::p_exportToSvg );
	ProjectWindow::connect( about, &QAction::triggered,
		q, &ProjectWindow::p_about );
	ProjectWindow::connect( aboutQt, &QAction::triggered,
		q, &ProjectWindow::p_aboutQt );
	ProjectWindow::connect( drawButton, &QAction::triggered,
		q, &ProjectWindow::p_drawButton );
	ProjectWindow::connect( drawComboBox, &QAction::triggered,
		q, &ProjectWindow::p_drawComboBox );
	ProjectWindow::connect( drawRadioButton, &QAction::triggered,
		q, &ProjectWindow::p_drawRadioButton );
	ProjectWindow::connect( drawCheckBox, &QAction::triggered,
		q, &ProjectWindow::p_drawCheckBox );
	ProjectWindow::connect( drawHSlider, &QAction::triggered,
		q, &ProjectWindow::p_drawHSlider );
	ProjectWindow::connect( drawVSlider, &QAction::triggered,
		q, &ProjectWindow::p_drawVSlider );
	ProjectWindow::connect( drawSpinbox, &QAction::triggered,
		q, &ProjectWindow::p_drawSpinbox );
	ProjectWindow::connect( m_alignHorizLeft, &QAction::triggered,
		q, &ProjectWindow::p_alignHorizontalLeft );
	ProjectWindow::connect( m_alignHorizCenter, &QAction::triggered,
		q, &ProjectWindow::p_alignHorizontalCenter );
	ProjectWindow::connect( m_alignHorizRight, &QAction::triggered,
		q, &ProjectWindow::p_alignHorizontalRight );
	ProjectWindow::connect( m_alignVertTop, &QAction::triggered,
		q, &ProjectWindow::p_alignVerticalTop );
	ProjectWindow::connect( m_alignVertCenter, &QAction::triggered,
		q, &ProjectWindow::p_alignVerticalCenter );
	ProjectWindow::connect( m_alignVertBottom, &QAction::triggered,
		q, &ProjectWindow::p_alignVerticalBottom );
	ProjectWindow::connect( m_widget->undoGroup(), &QUndoGroup::cleanChanged,
		q, &ProjectWindow::p_canUndoChanged );
	ProjectWindow::connect( m_widget->descriptionTab()->editor(),
		&TextEditor::undoAvailable,
		q, &ProjectWindow::p_canUndoChanged );
	ProjectWindow::connect( m_widget, &ProjectWidget::formAdded,
		q, &ProjectWindow::p_formAdded );
	ProjectWindow::connect( m_widget, &ProjectWidget::formDeleted,
		q, &ProjectWindow::p_formDeleted );

	q->switchToSelectMode();
}

void
ProjectWindowPrivate::setFlag( PageView * view,
	QGraphicsItem::GraphicsItemFlag f, bool enabled )
{
	setFlag( view->page()->childItems(), f, enabled );
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
ProjectWindowPrivate::enableEditing( PageView * view, bool on )
{
	enableEditing( view->page()->childItems(), on );
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
	m_cfg.description().set_tabName( m_widget->projectTabName() );

	m_cfg.description().set_text(
		m_widget->descriptionTab()->editor()->text() );

	m_cfg.page().clear();

	for( int i = 0; i < m_widget->pages().size(); ++i )
		m_cfg.page().push_back( m_widget->pages().at( i )->page()->cfg() );
}

void
ProjectWindowPrivate::prepareDrawingWithRectPlacer( bool editable )
{
	m_widget->enableSelection( false );

	foreach( PageView * v, m_widget->pages() )
	{
		v->page()->setCursor( Qt::CrossCursor );

		v->page()->switchToLineDrawingMode();

		setFlag( v, QGraphicsItem::ItemIsSelectable, false );

		enableEditing( v, editable );
	}
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

TabsList *
ProjectWindow::tabsList() const
{
	return d->m_tabsList;
}

void
ProjectWindow::readProject( const QString & fileName )
{
	QFile file( fileName );

	if( file.open( QIODevice::ReadOnly ) )
	{
		try {
			Cfg::tag_Project< cfgfile::qstring_trait_t > tag;

			QTextStream stream( &file );
			stream.setCodec( QTextCodec::codecForName( "UTF-8" ) );

			cfgfile::read_cfgfile( tag, stream, fileName );

			file.close();

			p_newProject();

			d->m_fileName = fileName;

			d->m_widget->setProject( tag.get_cfg() );

			setWindowModified( false );

			setWindowTitle( tr( "Prototyper - %1[*]" )
				.arg( QFileInfo( fileName ).baseName() ) );

			switchToSelectMode();
			d->m_addedForms.clear();
			d->m_deletedForms.clear();
		}
		catch( const cfgfile::exception_t< cfgfile::qstring_trait_t > & x )
		{
			file.close();

			QMessageBox::warning( this, tr( "Unable to Read Project..." ),
				tr( "Unable to read project.\n%1" ).arg( x.desc() ) );
		}
	}
	else
		QMessageBox::warning( this, tr( "Unable to Read Project..." ),
			tr( "Unable to read project.\nUnable to open file" ) );
}

void
ProjectWindow::closeEvent( QCloseEvent * e )
{
	e->accept();

	p_quit();
}

void
ProjectWindow::switchToPolylineMode()
{
	d->m_drawPolyLine->trigger();
}

void
ProjectWindow::switchToSelectMode()
{
	if( !d->m_select->isChecked() )
		d->m_select->setChecked( true );
	else
		p_select( true );
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

	d->m_widget->tabs()->setCurrentIndex( 0 );

	TopGui::instance()->saveCfg( 0 );

	QApplication::quit();
}

void
ProjectWindow::p_showHideGrid( bool show )
{
	GridMode mode = ( show ? ShowGrid : NoGrid );

	d->m_cfg.set_showGrid( show );

	foreach( PageView * view, d->m_widget->pages() )
		view->page()->setGridMode( mode );

	setWindowModified( true );
}

void
ProjectWindow::p_snapGrid( bool on )
{
	foreach( PageView * view, d->m_widget->pages() )
		view->page()->enableSnap( on );

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
			d->m_cfg.set_defaultGridStep( dlg.gridStep() );

			foreach( PageView * view, d->m_widget->pages() )
				view->page()->setGridStep( dlg.gridStep() );

			setWindowModified( true );
		}
		else if( index > 0 )
		{
			d->m_widget->pages()[ index - 1 ]->page()->setGridStep(
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
		step = d->m_widget->pages().at( index - 1 )->page()->gridStep();

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

	switchToSelectMode();
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

		QFile file( d->m_fileName );

		if( file.open( QIODevice::WriteOnly ) )
		{
			try {
				Cfg::tag_Project< cfgfile::qstring_trait_t > tag( d->m_cfg );

				QTextStream stream( &file );
				stream.setCodec( QTextCodec::codecForName( "UTF-8" ) );

				cfgfile::write_cfgfile( tag, stream );

				file.close();

				d->m_widget->cleanUndoGroup();

				d->m_widget->descriptionTab()->editor()->document()->
					clearUndoRedoStacks();

				d->m_widget->setTabRenamed( false );
			}
			catch( const cfgfile::exception_t< cfgfile::qstring_trait_t > & x )
			{
				file.close();

				QMessageBox::warning( this, tr( "Unable to Save Project..." ),
					tr( "Unable to save project.\n%1" ).arg( x.desc() ) );

				return;
			}
		}
		else
		{
			QMessageBox::warning( this, tr( "Unable to Save Project..." ),
				tr( "Unable to save project.\nUnable to open file." ) );

			return;
		}

		setWindowModified( false );

		d->m_addedForms.clear();

		d->m_deletedForms.clear();
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

	d->prepareDrawingWithRectPlacer();
}

void
ProjectWindow::p_drawRect()
{
	FormAction::instance()->setMode( FormAction::DrawRect );

	d->prepareDrawingWithRectPlacer();
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

	d->prepareDrawingWithRectPlacer( true );
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
ProjectWindow::p_select( bool checked )
{
	d->m_strokeColor->setEnabled( checked  );
	d->m_group->setEnabled( checked );
	d->m_ungroup->setEnabled( checked );
	d->m_alignHorizLeft->setEnabled( checked );
	d->m_alignHorizCenter->setEnabled( checked );
	d->m_alignHorizRight->setEnabled( checked );
	d->m_alignVertTop->setEnabled( checked );
	d->m_alignVertCenter->setEnabled( checked );
	d->m_alignVertBottom->setEnabled( checked );

	if( checked )
	{
		FormAction::instance()->setMode( FormAction::Select );

		d->m_widget->enableSelection( true );

		foreach( PageView * v, d->m_widget->pages() )
		{
			v->page()->setCursor( Qt::ArrowCursor );

			d->setFlag( v, QGraphicsItem::ItemIsSelectable, true );

			d->enableEditing( v, false );

			v->page()->switchToSelectMode();
		}
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
						obj->setObjectPen( QPen( c ) );
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
		d->m_stdItemsToolBar->show();
		d->m_widget->descriptionTab()->toolBar()->hide();

		FormAction::instance()->setForm(
			d->m_widget->pages().at( index - 1 )->page() );
	}
	else
	{
		d->m_formToolBar->hide();
		d->m_stdItemsToolBar->hide();
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

	if( !fileName.isEmpty() )
	{
		if( !fileName.endsWith( QLatin1String( ".pdf" ), Qt::CaseInsensitive ) )
			fileName.append( QLatin1String( ".pdf" ) );

		QFile file( fileName );

		bool canModify = true;

		if( !file.open( QIODevice::WriteOnly ) )
			canModify = false;

		file.close();

		if( canModify )
		{
			d->updateCfg();

			PdfExporter exporter( d->m_cfg );

			exporter.exportToDoc( fileName );
		}
		else
			QMessageBox::critical( this, tr( "Unable to export..." ),
				tr( "Unable to save file %1.\nFile is not writable." )
					.arg( fileName ) );
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

	if( !fileName.isEmpty() )
	{
		if( !fileName.endsWith( QLatin1String( ".htm" ), Qt::CaseInsensitive ) &&
			!fileName.endsWith( QLatin1String( ".html" ), Qt::CaseInsensitive ) )
				fileName.append( QLatin1String( ".html" ) );

		QFile file( fileName );

		bool canModify = true;

		if( !file.open( QIODevice::WriteOnly ) )
			canModify = false;

		file.close();

		if( canModify )
		{
			d->updateCfg();

			HtmlExporter exporter( d->m_cfg );

			exporter.exportToDoc( fileName );
		}
		else
			QMessageBox::critical( this, tr( "Unable to export..." ),
				tr( "Unable to save file %1.\nFile is not writable." )
					.arg( fileName ) );
	}
}

void
ProjectWindow::p_exportToSvg()
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

	QString dirName = QFileDialog::getExistingDirectory( this,
		tr( "Select directory to export project..." ),
		QStandardPaths::standardLocations(
			QStandardPaths::DocumentsLocation ).first() );

	if( !dirName.isEmpty() )
	{
		try {
			d->updateCfg();

			SvgExporter exporter( d->m_cfg );

			exporter.exportToDoc( dirName );
		}
		catch( const SvgExporterException & e )
		{
			QMessageBox::critical( this, tr( "Unable to export..." ),
				e.what() );
		}
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

void
ProjectWindow::p_drawButton()
{
	FormAction::instance()->setMode( FormAction::DrawButton );

	d->prepareDrawingWithRectPlacer();
}

void
ProjectWindow::p_drawComboBox()
{
	FormAction::instance()->setMode( FormAction::DrawComboBox );

	d->prepareDrawingWithRectPlacer();
}

void
ProjectWindow::p_drawRadioButton()
{
	FormAction::instance()->setMode( FormAction::DrawRadioButton );

	d->prepareDrawingWithRectPlacer();
}

void
ProjectWindow::p_drawCheckBox()
{
	FormAction::instance()->setMode( FormAction::DrawCheckBox );

	d->prepareDrawingWithRectPlacer();
}

void
ProjectWindow::p_drawHSlider()
{
	FormAction::instance()->setMode( FormAction::DrawHSlider );

	d->prepareDrawingWithRectPlacer();
}

void
ProjectWindow::p_drawVSlider()
{
	FormAction::instance()->setMode( FormAction::DrawVSlider );

	d->prepareDrawingWithRectPlacer();
}

void
ProjectWindow::p_drawSpinbox()
{
	FormAction::instance()->setMode( FormAction::DrawSpinBox );

	d->prepareDrawingWithRectPlacer();
}

void
ProjectWindow::p_alignVerticalTop()
{
	FormAction::instance()->form()->alignVerticalTop();
}

void
ProjectWindow::p_alignVerticalCenter()
{
	FormAction::instance()->form()->alignVerticalCenter();
}

void
ProjectWindow::p_alignVerticalBottom()
{
	FormAction::instance()->form()->alignVerticalBottom();
}

void
ProjectWindow::p_alignHorizontalLeft()
{
	FormAction::instance()->form()->alignHorizontalLeft();
}

void
ProjectWindow::p_alignHorizontalCenter()
{
	FormAction::instance()->form()->alignHorizontalCenter();
}

void
ProjectWindow::p_alignHorizontalRight()
{
	FormAction::instance()->form()->alignHorizontalRight();
}

void
ProjectWindow::p_canUndoChanged( bool canUndo )
{
	Q_UNUSED( canUndo )

	QList< QUndoStack* > stacks = d->m_widget->undoGroup()->stacks();

	bool can = false;

	if( !d->m_addedForms.isEmpty() )
		can = true;
	else if( !d->m_deletedForms.isEmpty() )
		can = true;
	else if( d->m_widget->descriptionTab()->editor()->document()->isUndoAvailable() )
		can = true;
	else if( d->m_widget->isTabRenamed() )
		can = true;
	else
	{
		foreach( QUndoStack * s, stacks )
		{
			if( !s->isClean() )
			{
				can = true;

				break;
			}
		}
	}

	if( can )
		setWindowModified( true );
	else
		setWindowModified( false );
}

void
ProjectWindow::p_formAdded( PageView * form )
{
	d->m_addedForms.append( form );
}

void
ProjectWindow::p_formDeleted( PageView * form )
{
	d->m_deletedForms.append( form );

	if( d->m_addedForms.contains( form ) )
		d->m_addedForms.removeOne( form );
}

} /* namespace Core */

} /* namespace Prototyper */
