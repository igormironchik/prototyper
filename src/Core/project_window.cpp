/*!
	\file

	\author Igor Mironchik (igor.mironchik at gmail dot com).

	Copyright (c) 2016-2021 Igor Mironchik

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
#include "form/page.hpp"
#include "form/page_view.hpp"
#include "form/page_scene.hpp"
#include "dlg/grid_step_dlg.hpp"
#include "project_cfg.hpp"
#include "session_cfg.hpp"
#include "project_description_tab.hpp"
#include "text_editor.hpp"
#include "form/actions.hpp"
#include "tabs_list.hpp"
#include "form/object.hpp"
#include "form/text.hpp"
#include "exporter/pdf_exporter.hpp"
#include "exporter/html_exporter.hpp"
#include "exporter/svg_exporter.hpp"
#include "form/group.hpp"
#include "form/undo_commands.hpp"
#include "constants.hpp"
#include "form/utils.hpp"
#include "version.hpp"

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
#include <QTextStream>
#include <QFile>
#include <QStringListModel>
#include <QScrollArea>


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
		,	m_zoomToolBar( nullptr )
		,	m_formToolBarGroup( Q_NULLPTR )
		,	m_tabsList( Q_NULLPTR )
		,	m_drawLine( Q_NULLPTR )
		,	m_select( Q_NULLPTR )
		,	m_drawPolyLine( Q_NULLPTR )
		,	m_undoAction( Q_NULLPTR )
		,	m_redoAction( Q_NULLPTR )
		,	m_strokeColor( nullptr )
		,	m_fillColor( nullptr )
		,	m_group( nullptr )
		,	m_ungroup( nullptr )
		,	m_alignHorizLeft( nullptr )
		,	m_alignHorizCenter( nullptr )
		,	m_alignHorizRight( nullptr )
		,	m_alignVertTop( nullptr )
		,	m_alignVertCenter( nullptr )
		,	m_alignVertBottom( nullptr )
		,	m_drawRect( nullptr )
		,	m_insertText( nullptr )
		,	m_insertImage( nullptr )
		,	m_drawButton( nullptr )
		,	m_drawComboBox( nullptr )
		,	m_drawRadioButton( nullptr )
		,	m_drawCheckBox( nullptr )
		,	m_drawHSlider( nullptr )
		,	m_drawVSlider( nullptr )
		,	m_drawSpinbox( nullptr )
		,	m_zoomIn( nullptr )
		,	m_zoomOut( nullptr )
		,	m_zoomOriginal( nullptr )
		,	m_duplicate( nullptr )
		,	m_toTop( nullptr )
		,	m_toBottom( nullptr )
		,	m_up( nullptr )
		,	m_down( nullptr )
		,	m_propertiesDock( nullptr )
		,	m_propertiesScrollArea( nullptr )
		,	m_isQuit( false )
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
	void prepareForDrawing( bool editable = false );
	//! Clear edit mode in texts.
	void clearEditModeInTexts();
	//! Clean added/deleted pages.
	void cleanPages();

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
	//! Open folder path.
	QString m_openFolder;
	//! Form toolbar.
	QToolBar * m_formToolBar;
	//! Standard items' toolbar.
	QToolBar * m_stdItemsToolBar;
	//! Zoom toolbar.
	QToolBar * m_zoomToolBar;
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
	//! Fill color action.
	QAction * m_fillColor;
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
	QAction * m_drawRect;
	QAction * m_insertText;
	QAction * m_insertImage;
	QAction * m_drawButton;
	QAction * m_drawComboBox;
	QAction * m_drawRadioButton;
	QAction * m_drawCheckBox;
	QAction * m_drawHSlider;
	QAction * m_drawVSlider;
	QAction * m_drawSpinbox;
	QAction * m_zoomIn;
	QAction * m_zoomOut;
	QAction * m_zoomOriginal;
	QAction * m_duplicate;
	QAction * m_toTop;
	QAction * m_toBottom;
	QAction * m_up;
	QAction * m_down;
	//! Properties dock.
	QDockWidget * m_propertiesDock;
	//! Scroll area for properties.
	QScrollArea * m_propertiesScrollArea;
	//! Added forms.
	QList< PageView* > m_addedForms;
	//! Deleted forms.
	QList< PageView* > m_deletedForms;
	//! Author.
	QString m_author;
	//! Is quit?
	bool m_isQuit;
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
		QIcon( QStringLiteral( ":/Core/img/document-new.png" ) ),
		ProjectWindow::tr( "New Project" ) );
	newProject->setShortcutContext( Qt::ApplicationShortcut );
	newProject->setShortcut( ProjectWindow::tr( "Ctrl+N" ) );

	QAction * openProject = file->addAction(
		QIcon( QStringLiteral( ":/Core/img/document-open.png" ) ),
		ProjectWindow::tr( "Open Project" ) );
	openProject->setShortcutContext( Qt::ApplicationShortcut );
	openProject->setShortcut( ProjectWindow::tr( "Ctrl+O" ) );

	file->addSeparator();

	m_saveProject = file->addAction(
		QIcon( QStringLiteral( ":/Core/img/document-save.png" ) ),
		ProjectWindow::tr( "Save Project" ) );
	m_saveProject->setShortcutContext( Qt::ApplicationShortcut );
	m_saveProject->setShortcut( ProjectWindow::tr( "Ctrl+S" ) );

	QAction * saveProjectAs = file->addAction(
		QIcon( QStringLiteral( ":/Core/img/document-save-as.png" ) ),
		ProjectWindow::tr( "Save Project As" ) );

	file->addSeparator();

	QMenu * exportMenu = file->addMenu(
		QIcon( QStringLiteral( ":/Core/img/document-export.png" ) ),
		ProjectWindow::tr( "Export To" ) );

	QAction * exportToPdf = exportMenu->addAction(
		QIcon( QStringLiteral( ":/Core/img/application-pdf.png" ) ),
		ProjectWindow::tr( "PDF" ) );

	QAction * exportToHtml = exportMenu->addAction(
		QIcon( QStringLiteral( ":/Core/img/text-html.png" ) ),
		ProjectWindow::tr( "HTML" ) );

	QAction * exportToSvg = exportMenu->addAction(
		QIcon( QStringLiteral( ":/Core/img/image-svg+xml.png" ) ),
		ProjectWindow::tr( "SVG Images" ) );

	file->addSeparator();

	QAction * quitAction = file->addAction(
		QIcon( QStringLiteral( ":/Core/img/application-exit.png" ) ),
		ProjectWindow::tr( "Quit" ) );
	quitAction->setShortcutContext( Qt::ApplicationShortcut );
	quitAction->setShortcut( ProjectWindow::tr( "Ctrl+Q" ) );

	QMenu * view = q->menuBar()->addMenu( ProjectWindow::tr( "&Dock" ) );

	m_tabsList = new TabsList( q );
	m_tabsList->setObjectName( QStringLiteral( "m_tabsList" ) );

	m_tabsList->model()->setStringList( QStringList() << m_widget->projectTabName() );

	m_propertiesDock = new QDockWidget( q );
	m_propertiesDock->setWindowTitle( ProjectWindow::tr( "Properties" ) );
	m_propertiesDock->setObjectName( QStringLiteral( "m_propertiesDock" ) );
	m_propertiesScrollArea = new QScrollArea( m_propertiesDock );
	m_propertiesScrollArea->setWidgetResizable( true );
	m_propertiesDock->setWidget( m_propertiesScrollArea );
	m_propertiesDock->setMinimumWidth( 250 );
	auto w = new QWidget( m_propertiesScrollArea );
	m_propertiesScrollArea->setWidget( w );

	q->addDockWidget( Qt::LeftDockWidgetArea, m_tabsList );
	q->addDockWidget( Qt::RightDockWidgetArea, m_propertiesDock );

	m_propertiesDock->hide();

	view->addAction( m_tabsList->toggleViewAction() );
	m_tabsList->toggleViewAction()->setShortcutContext(
		Qt::ApplicationShortcut );
	m_tabsList->toggleViewAction()->setShortcut(
		ProjectWindow::tr( "Ctrl+Alt+T" ) );

	view->addAction( m_propertiesDock->toggleViewAction() );
	m_propertiesDock->toggleViewAction()->setShortcutContext(
		Qt::ApplicationShortcut );
	m_propertiesDock->toggleViewAction()->setShortcut(
		ProjectWindow::tr( "Ctrl+Alt+P" ) );

	QAction * newForm = new QAction( QIcon( QStringLiteral( ":/Core/img/list-add.png" ) ),
		ProjectWindow::tr( "Add Page" ), q );
	newForm->setShortcutContext( Qt::ApplicationShortcut );
	newForm->setShortcut( ProjectWindow::tr( "Ctrl+T" ) );
	q->addAction( newForm );

	m_formToolBar = new QToolBar( ProjectWindow::tr( "Drawing Tools" ), q );
	m_formToolBar->setObjectName( QStringLiteral( "m_formToolBar" ) );

	m_stdItemsToolBar = new QToolBar( ProjectWindow::tr( "Standard Items" ), q );
	m_stdItemsToolBar->setObjectName( QStringLiteral( "m_stdItemsToolBar" ) );

	m_zoomToolBar = new QToolBar( ProjectWindow::tr( "Zoom Tools" ), q );
	m_zoomToolBar->setObjectName( QStringLiteral( "m_zoomToolBar" ) );

	m_formToolBarGroup = new QActionGroup( q );
	m_formToolBarGroup->setExclusive( true );

	m_select = m_formToolBar->addAction(
		QIcon( QStringLiteral( ":/Core/img/edit-select.png" ) ),
		ProjectWindow::tr( "Select" ) );
	m_select->setCheckable( true );
	m_formToolBarGroup->addAction( m_select );
	m_select->setShortcutContext( Qt::ApplicationShortcut );
	m_select->setShortcut( ProjectWindow::tr( "Alt+S" ) );

	m_drawLine = m_formToolBar->addAction(
		QIcon( QStringLiteral( ":/Core/img/draw-freehand.png" ) ),
		ProjectWindow::tr( "Draw Line" ) );
	m_drawLine->setCheckable( true );
	m_formToolBarGroup->addAction( m_drawLine );
	m_drawLine->setShortcutContext( Qt::ApplicationShortcut );
	m_drawLine->setShortcut( ProjectWindow::tr( "Alt+L" ) );

	m_drawPolyLine = m_formToolBar->addAction(
		QIcon( QStringLiteral( ":/Core/img/draw-polyline.png" ) ),
		ProjectWindow::tr( "Draw Polyline" ) );
	m_drawPolyLine->setCheckable( true );
	m_formToolBarGroup->addAction( m_drawPolyLine );
	m_drawPolyLine->setShortcutContext( Qt::ApplicationShortcut );
	m_drawPolyLine->setShortcut( ProjectWindow::tr( "Alt+P" ) );

	m_drawRect = m_formToolBar->addAction(
		QIcon( QStringLiteral( ":/Core/img/draw-rectangle.png" ) ),
		ProjectWindow::tr( "Draw Rect" ) );
	m_drawRect->setCheckable( true );
	m_formToolBarGroup->addAction( m_drawRect );
	m_drawRect->setShortcutContext( Qt::ApplicationShortcut );
	m_drawRect->setShortcut( ProjectWindow::tr( "Alt+R" ) );

	m_insertText = m_formToolBar->addAction(
		QIcon( QStringLiteral( ":/Core/img/insert-text.png" ) ),
		ProjectWindow::tr( "Insert Text" ) );
	m_insertText->setCheckable( true );
	m_formToolBarGroup->addAction( m_insertText );
	m_insertText->setShortcutContext( Qt::ApplicationShortcut );
	m_insertText->setShortcut( ProjectWindow::tr( "Alt+T" ) );

	m_insertImage = m_formToolBar->addAction(
		QIcon( QStringLiteral( ":/Core/img/insert-image.png" ) ),
		ProjectWindow::tr( "Insert Image" ) );
	m_insertImage->setShortcutContext( Qt::ApplicationShortcut );
	m_insertImage->setShortcut( ProjectWindow::tr( "Alt+I" ) );

	m_formToolBar->addSeparator();

	m_group = m_formToolBar->addAction(
		QIcon( QStringLiteral( ":/Core/img/merge.png" ) ),
		ProjectWindow::tr( "Group" ) );
	m_group->setShortcutContext( Qt::ApplicationShortcut );
	m_group->setShortcut( ProjectWindow::tr( "Ctrl+G" ) );

	m_ungroup = m_formToolBar->addAction(
		QIcon( QStringLiteral( ":/Core/img/split.png" ) ),
		ProjectWindow::tr( "Ungroup" ) );
	m_ungroup->setShortcutContext( Qt::ApplicationShortcut );
	m_ungroup->setShortcut( ProjectWindow::tr( "Ctrl+U" ) );

	m_formToolBar->addSeparator();

	m_strokeColor = m_formToolBar->addAction(
		QIcon( QStringLiteral( ":/Core/img/format-stroke-color.png" ) ),
		ProjectWindow::tr( "Line Color" ) );

	m_fillColor = m_formToolBar->addAction(
		QIcon( QStringLiteral( ":/Core/img/fill-color.png" ) ),
		ProjectWindow::tr( "Fill Color" ) );

	m_drawButton = m_stdItemsToolBar->addAction(
		QIcon( QStringLiteral( ":/Core/img/draw-pushbutton.png" ) ),
		ProjectWindow::tr( "Draw Button" ) );
	m_drawButton->setCheckable( true );
	m_formToolBarGroup->addAction( m_drawButton );
	m_drawButton->setShortcutContext( Qt::ApplicationShortcut );
	m_drawButton->setShortcut( ProjectWindow::tr( "Alt+B" ) );

	m_drawComboBox = m_stdItemsToolBar->addAction(
		QIcon( QStringLiteral( ":/Core/img/draw-combobox.png" ) ),
		ProjectWindow::tr( "Draw ComboBox" ) );
	m_drawComboBox->setCheckable( true );
	m_formToolBarGroup->addAction( m_drawComboBox );
	m_drawComboBox->setShortcutContext( Qt::ApplicationShortcut );
	m_drawComboBox->setShortcut( ProjectWindow::tr( "Alt+C" ) );

	m_drawRadioButton = m_stdItemsToolBar->addAction(
		QIcon( QStringLiteral( ":/Core/img/draw-radiobutton.png" ) ),
		ProjectWindow::tr( "Draw Radio Button" ) );
	m_drawRadioButton->setCheckable( true );
	m_formToolBarGroup->addAction( m_drawRadioButton );
	m_drawRadioButton->setShortcutContext( Qt::ApplicationShortcut );
	m_drawRadioButton->setShortcut( ProjectWindow::tr( "Alt+R" ) );

	m_drawCheckBox = m_stdItemsToolBar->addAction(
		QIcon( QStringLiteral( ":/Core/img/draw-checkbox.png" ) ),
		ProjectWindow::tr( "Draw Check Box" ) );
	m_drawCheckBox->setCheckable( true );
	m_formToolBarGroup->addAction( m_drawCheckBox );
	m_drawCheckBox->setShortcutContext( Qt::ApplicationShortcut );
	m_drawCheckBox->setShortcut( ProjectWindow::tr( "Alt+K" ) );

	m_drawHSlider = m_stdItemsToolBar->addAction(
		QIcon( QStringLiteral( ":/Core/img/draw-hslider.png" ) ),
		ProjectWindow::tr( "Draw Horizontal Slider" ) );
	m_drawHSlider->setCheckable( true );
	m_formToolBarGroup->addAction( m_drawHSlider );
	m_drawHSlider->setShortcutContext( Qt::ApplicationShortcut );
	m_drawHSlider->setShortcut( ProjectWindow::tr( "Alt+H" ) );

	m_drawVSlider = m_stdItemsToolBar->addAction(
		QIcon( QStringLiteral( ":/Core/img/draw-vslider.png" ) ),
		ProjectWindow::tr( "Draw Vertical Slider" ) );
	m_drawVSlider->setCheckable( true );
	m_formToolBarGroup->addAction( m_drawVSlider );
	m_drawVSlider->setShortcutContext( Qt::ApplicationShortcut );
	m_drawVSlider->setShortcut( ProjectWindow::tr( "Alt+V" ) );

	m_drawSpinbox = m_stdItemsToolBar->addAction(
		QIcon( QStringLiteral( ":/Core/img/draw-spinbox.png" ) ),
		ProjectWindow::tr( "Draw Spinbox" ) );
	m_drawSpinbox->setCheckable( true );
	m_formToolBarGroup->addAction( m_drawSpinbox );
	m_drawSpinbox->setShortcutContext( Qt::ApplicationShortcut );
	m_drawSpinbox->setShortcut( ProjectWindow::tr( "Alt+S" ) );

	m_formToolBar->addSeparator();

	m_alignHorizLeft = m_formToolBar->addAction(
		QIcon( QStringLiteral( ":/Core/img/align-horizontal-left.png" ) ),
		ProjectWindow::tr( "Align Horizontal Left" ) );
	m_alignHorizLeft->setShortcutContext( Qt::ApplicationShortcut );
	m_alignHorizLeft->setShortcut( ProjectWindow::tr( "Ctrl+Alt+L" ) );

	m_alignHorizCenter = m_formToolBar->addAction(
		QIcon( QStringLiteral( ":/Core/img/align-horizontal-center.png" ) ),
		ProjectWindow::tr( "Align Horizontal Center" ) );
	m_alignHorizCenter->setShortcutContext( Qt::ApplicationShortcut );
	m_alignHorizCenter->setShortcut( ProjectWindow::tr( "Ctrl+Alt+C" ) );

	m_alignHorizRight = m_formToolBar->addAction(
		QIcon( QStringLiteral( ":/Core/img/align-horizontal-right.png" ) ),
		ProjectWindow::tr( "Align Horizontal Right" ) );
	m_alignHorizRight->setShortcutContext( Qt::ApplicationShortcut );
	m_alignHorizRight->setShortcut( ProjectWindow::tr( "Ctrl+Alt+R" ) );

	m_alignVertTop = m_formToolBar->addAction(
		QIcon( QStringLiteral( ":/Core/img/align-vertical-top.png" ) ),
		ProjectWindow::tr( "Align Vertical Top" ) );
	m_alignVertTop->setShortcutContext( Qt::ApplicationShortcut );
	m_alignVertTop->setShortcut( ProjectWindow::tr( "Ctrl+Alt+T" ) );

	m_alignVertCenter = m_formToolBar->addAction(
		QIcon( QStringLiteral( ":/Core/img/align-vertical-center.png" ) ),
		ProjectWindow::tr( "Align Vertical Center" ) );
	m_alignVertCenter->setShortcutContext( Qt::ApplicationShortcut );
	m_alignVertCenter->setShortcut( ProjectWindow::tr( "Ctrl+Alt+V" ) );

	m_alignVertBottom = m_formToolBar->addAction(
		QIcon( QStringLiteral( ":/Core/img/align-vertical-bottom.png" ) ),
		ProjectWindow::tr( "Align Vertical Bottom" ) );
	m_alignVertBottom->setShortcutContext( Qt::ApplicationShortcut );
	m_alignVertBottom->setShortcut( ProjectWindow::tr( "Ctrl+Alt+B" ) );

	m_formToolBar->addSeparator();

	m_toTop = m_formToolBar->addAction(
		QIcon( QStringLiteral( ":/Core/img/object-order-front.png" ) ),
		ProjectWindow::tr( "Bring to Top" ) );
	m_toTop->setShortcutContext( Qt::ApplicationShortcut );
	m_toTop->setShortcut( ProjectWindow::tr( "Ctrl+Alt+Up" ) );

	m_up = m_formToolBar->addAction(
		QIcon( QStringLiteral( ":/Core/img/object-order-raise.png" ) ),
		ProjectWindow::tr( "Raise" ) );
	m_up->setShortcutContext( Qt::ApplicationShortcut );
	m_up->setShortcut( ProjectWindow::tr( "Ctrl+Up" ) );

	m_down = m_formToolBar->addAction(
		QIcon( QStringLiteral( ":/Core/img/object-order-lower.png" ) ),
		ProjectWindow::tr( "Lower" ) );
	m_down->setShortcutContext( Qt::ApplicationShortcut );
	m_down->setShortcut( ProjectWindow::tr( "Ctrl+Down" ) );

	m_toBottom = m_formToolBar->addAction(
		QIcon( QStringLiteral( ":/Core/img/object-order-back.png" ) ),
		ProjectWindow::tr( "Bring to Bottom" ) );
	m_toBottom->setShortcutContext( Qt::ApplicationShortcut );
	m_toBottom->setShortcut( ProjectWindow::tr( "Ctrl+Alt+Down" ) );

	m_toTop->setEnabled( false );
	m_up->setEnabled( false );
	m_down->setEnabled( false );
	m_toBottom->setEnabled( false );

	m_zoomIn = m_zoomToolBar->addAction(
		QIcon( QStringLiteral( ":/Core/img/zoom-in.png" ) ),
		ProjectWindow::tr( "Zoom In" ) );
	m_zoomIn->setShortcutContext( Qt::ApplicationShortcut );
	m_zoomIn->setShortcut( ProjectWindow::tr( "Ctrl+=" ) );

	m_zoomOriginal = m_zoomToolBar->addAction(
		QIcon( QStringLiteral( ":/Core/img/zoom-original.png" ) ),
		ProjectWindow::tr( "Zoom Original" ) );
	m_zoomOriginal->setShortcutContext( Qt::ApplicationShortcut );
	m_zoomOriginal->setShortcut( ProjectWindow::tr( "Ctrl+1" ) );

	m_zoomOut = m_zoomToolBar->addAction(
		QIcon( QStringLiteral( ":/Core/img/zoom-out.png" ) ),
		ProjectWindow::tr( "Zoom Out" ) );
	m_zoomOut->setShortcutContext( Qt::ApplicationShortcut );
	m_zoomOut->setShortcut( ProjectWindow::tr( "Ctrl+-" ) );

	q->addToolBar( Qt::TopToolBarArea, m_formToolBar );

	q->addToolBar( Qt::TopToolBarArea, m_stdItemsToolBar );

	q->addToolBar( Qt::TopToolBarArea, m_zoomToolBar );

	m_formToolBar->hide();

	m_stdItemsToolBar->hide();

	m_zoomToolBar->hide();

	QMenu * form = q->menuBar()->addMenu( ProjectWindow::tr( "D&rawing" ) );

	form->addAction( newForm );

	form->addSeparator();

	m_grid = form->addAction(
		QIcon( QStringLiteral( ":/Core/img/view-grid.png" ) ),
		ProjectWindow::tr( "Show Grid" ) );
	m_grid->setShortcutContext( Qt::ApplicationShortcut );
	m_grid->setShortcut( ProjectWindow::tr( "Ctrl+Alt+G" ) );
	m_grid->setCheckable( true );
	m_grid->setChecked( true );

	m_gridStep = form->addAction(
		QIcon( QStringLiteral( ":/Core/img/measure.png" ) ),
		ProjectWindow::tr( "Grid Step" ) );

	QAction * snapGrid = form->addAction(
		QIcon( QStringLiteral( ":/Core/img/snap-intersection.png" ) ),
		ProjectWindow::tr( "Snap Grid" ) );
	snapGrid->setShortcutContext( Qt::ApplicationShortcut );
	snapGrid->setShortcut( ProjectWindow::tr( "Alt+G" ) );
	snapGrid->setCheckable( true );
	snapGrid->setChecked( true );

	form->addSeparator();

	m_duplicate = form->addAction( QIcon( QStringLiteral( ":/Core/img/edit-copy.png" ) ),
		ProjectWindow::tr( "Duplicate" ) );
	m_duplicate->setShortcutContext( Qt::ApplicationShortcut );
	m_duplicate->setShortcut( ProjectWindow::tr( "Ctrl+D" ) );
	m_duplicate->setEnabled( false );

	form->addSeparator();

	form->addAction( m_zoomIn );
	form->addAction( m_zoomOriginal );
	form->addAction( m_zoomOut );

	form->addSeparator();

	form->addAction( m_select );
	form->addAction( m_drawLine );
	form->addAction( m_drawPolyLine );
	form->addAction( m_drawRect );
	form->addAction( m_insertText );
	form->addAction( m_insertImage );
	form->addAction( m_drawButton );
	form->addAction( m_drawComboBox );
	form->addAction( m_drawRadioButton );
	form->addAction( m_drawCheckBox );
	form->addAction( m_drawHSlider );
	form->addAction( m_drawVSlider );
	form->addAction( m_drawSpinbox );

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

	form->addAction( m_toTop );
	form->addAction( m_up );
	form->addAction( m_down );
	form->addAction( m_toBottom );

	form->addSeparator();

	QAction * undoAction = m_widget->undoGroup()->createUndoAction( q );
	undoAction->setShortcutContext( Qt::ApplicationShortcut );
	undoAction->setShortcut( ProjectWindow::tr( "Ctrl+Z" ) );
	undoAction->setIcon( QIcon( QStringLiteral( ":/Core/img/edit-undo.png" ) ) );
	form->addAction( undoAction );

	QAction * redoAction = m_widget->undoGroup()->createRedoAction( q );
	redoAction->setShortcutContext( Qt::ApplicationShortcut );
	redoAction->setShortcut( ProjectWindow::tr( "Ctrl+Y" ) );
	redoAction->setIcon( QIcon( QStringLiteral( ":/Core/img/edit-redo.png" ) ) );
	form->addAction( redoAction );

	QMenu * help = q->menuBar()->addMenu( ProjectWindow::tr( "&Help" ) );
	QAction * about = help->addAction( QIcon( QStringLiteral( ":/Core/img/prototyper.png" ) ),
		ProjectWindow::tr( "About" ) );
	QAction * aboutQt = help->addAction( QIcon( QStringLiteral( ":/Core/img/qt.png" ) ),
		ProjectWindow::tr( "About Qt" ) );

	ProjectWindow::connect( quitAction, &QAction::triggered,
		q, &ProjectWindow::quit );
	ProjectWindow::connect( m_grid, &QAction::toggled,
		q, &ProjectWindow::showHideGrid );
	ProjectWindow::connect( snapGrid, &QAction::toggled,
		q, &ProjectWindow::snapGrid );
	ProjectWindow::connect( m_gridStep, &QAction::triggered,
		q, QOverload<>::of( &ProjectWindow::setGridStep ) );
	ProjectWindow::connect( newForm, &QAction::triggered,
		m_widget, &ProjectWidget::addPage );
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
	ProjectWindow::connect( m_select, &QAction::toggled,
		q, &ProjectWindow::select );
	ProjectWindow::connect( m_drawLine, &QAction::triggered,
		q, &ProjectWindow::drawLine );
	ProjectWindow::connect( m_drawRect, &QAction::triggered,
		q, &ProjectWindow::drawRect );
	ProjectWindow::connect( m_drawPolyLine, &QAction::toggled,
		q, &ProjectWindow::drawPolyline );
	ProjectWindow::connect( m_insertText, &QAction::triggered,
		q, &ProjectWindow::insertText );
	ProjectWindow::connect( m_insertImage, &QAction::triggered,
		q, &ProjectWindow::insertImage );
	ProjectWindow::connect( m_group, &QAction::triggered,
		q, &ProjectWindow::group );
	ProjectWindow::connect( m_ungroup, &QAction::triggered,
		q, &ProjectWindow::ungroup );
	ProjectWindow::connect( m_strokeColor, &QAction::triggered,
		q, &ProjectWindow::strokeColor );
	ProjectWindow::connect( m_fillColor, &QAction::triggered,
		q, &ProjectWindow::fillColor );
	ProjectWindow::connect( m_widget->tabs(), &QTabWidget::currentChanged,
		q, &ProjectWindow::tabChanged );
	ProjectWindow::connect( exportToPdf, &QAction::triggered,
		q, &ProjectWindow::exportToPDf );
	ProjectWindow::connect( exportToHtml, &QAction::triggered,
		q, &ProjectWindow::exportToHtml );
	ProjectWindow::connect( exportToSvg, &QAction::triggered,
		q, &ProjectWindow::exportToSvg );
	ProjectWindow::connect( about, &QAction::triggered,
		q, &ProjectWindow::about );
	ProjectWindow::connect( aboutQt, &QAction::triggered,
		q, &ProjectWindow::aboutQt );
	ProjectWindow::connect( m_drawButton, &QAction::triggered,
		q, &ProjectWindow::drawButton );
	ProjectWindow::connect( m_drawComboBox, &QAction::triggered,
		q, &ProjectWindow::drawComboBox );
	ProjectWindow::connect( m_drawRadioButton, &QAction::triggered,
		q, &ProjectWindow::drawRadioButton );
	ProjectWindow::connect( m_drawCheckBox, &QAction::triggered,
		q, &ProjectWindow::drawCheckBox );
	ProjectWindow::connect( m_drawHSlider, &QAction::triggered,
		q, &ProjectWindow::drawHSlider );
	ProjectWindow::connect( m_drawVSlider, &QAction::triggered,
		q, &ProjectWindow::drawVSlider );
	ProjectWindow::connect( m_drawSpinbox, &QAction::triggered,
		q, &ProjectWindow::drawSpinbox );
	ProjectWindow::connect( m_alignHorizLeft, &QAction::triggered,
		q, &ProjectWindow::alignHorizontalLeft );
	ProjectWindow::connect( m_alignHorizCenter, &QAction::triggered,
		q, &ProjectWindow::alignHorizontalCenter );
	ProjectWindow::connect( m_alignHorizRight, &QAction::triggered,
		q, &ProjectWindow::alignHorizontalRight );
	ProjectWindow::connect( m_alignVertTop, &QAction::triggered,
		q, &ProjectWindow::alignVerticalTop );
	ProjectWindow::connect( m_alignVertCenter, &QAction::triggered,
		q, &ProjectWindow::alignVerticalCenter );
	ProjectWindow::connect( m_alignVertBottom, &QAction::triggered,
		q, &ProjectWindow::alignVerticalBottom );
	ProjectWindow::connect( m_zoomIn, &QAction::triggered,
		q, &ProjectWindow::zoomIn );
	ProjectWindow::connect( m_zoomOriginal, &QAction::triggered,
		q, &ProjectWindow::zoomOriginal );
	ProjectWindow::connect( m_zoomOut, &QAction::triggered,
		q, &ProjectWindow::zoomOut );
	ProjectWindow::connect( m_duplicate, &QAction::triggered,
		q, &ProjectWindow::duplicate );
	ProjectWindow::connect( m_widget->undoGroup(), &QUndoGroup::cleanChanged,
		q, &ProjectWindow::canUndoChanged );
	ProjectWindow::connect( m_widget->descriptionTab()->editor(),
		&TextEditor::undoAvailable,
		q, &ProjectWindow::canUndoChanged );
	ProjectWindow::connect( m_widget, &ProjectWidget::pageAdded,
		q, &ProjectWindow::pageAdded );
	ProjectWindow::connect( m_widget, &ProjectWidget::pageDeleted,
		q, &ProjectWindow::pageDeleted );
	ProjectWindow::connect( m_toTop, &QAction::triggered,
		q, &ProjectWindow::toTop );
	ProjectWindow::connect( m_up, &QAction::triggered,
		q, &ProjectWindow::raise );
	ProjectWindow::connect( m_down, &QAction::triggered,
		q, &ProjectWindow::lower );
	ProjectWindow::connect( m_toBottom, &QAction::triggered,
		q, &ProjectWindow::toBottom );

	q->switchToSelectMode();

	q->tabChanged( 0 );
}

void
ProjectWindowPrivate::clearEditModeInTexts()
{
	for( const auto & p : qAsConst( m_widget->pages() ) )
		p->page()->clearEditModeInTexts();
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

		auto * tmp = dynamic_cast< FormObject* > ( item );

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
		auto * text = dynamic_cast< FormText* > ( item );

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

	for( const auto & page : qAsConst( m_widget->pages() ) )
		m_cfg.page().push_back( page->page()->cfg() );
}

void
ProjectWindowPrivate::prepareForDrawing( bool editable )
{
	m_widget->enableSelection( false );

	foreach( PageView * v, m_widget->pages() )
	{
		v->page()->setCursor( Qt::CrossCursor );

		v->page()->switchToDrawingMode();

		setFlag( v, QGraphicsItem::ItemIsSelectable, false );

		enableEditing( v, editable );
	}
}

void
ProjectWindowPrivate::cleanPages()
{
	m_addedForms.clear();
	m_deletedForms.clear();
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

ProjectWindow::~ProjectWindow() = default;

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

const QString &
ProjectWindow::author() const
{
	return d->m_author;
}

void
ProjectWindow::setAuthor( const QString & name )
{
	d->m_author = name;
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

			cfgfile::read_cfgfile( tag, stream, fileName );

			file.close();

			newProject();

			d->m_fileName = fileName;

			d->m_openFolder = QFileInfo( fileName ).absolutePath();

			d->m_widget->setProject( tag.get_cfg() );

			setWindowModified( false );

			setWindowTitle( tr( "Prototyper - %1[*]" )
				.arg( QFileInfo( fileName ).baseName() ) );

			switchToSelectMode();
			d->cleanPages();
			tabChanged( 0 );
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

	if( !d->m_isQuit )
		quit();
}

void
ProjectWindow::switchToPolylineMode()
{
	d->m_drawPolyLine->setChecked( true );
}

void
ProjectWindow::switchToSelectMode()
{
	if( !d->m_select->isChecked() )
		d->m_select->setChecked( true );
	else
		select( true );
}

void
ProjectWindow::quit()
{
	d->m_isQuit = true;

	if( isWindowModified() )
	{
		QMessageBox::StandardButton btn =
			QMessageBox::question( this, tr( "Do You Want to Save Project..." ),
				tr( "Do you want to save project?" ),
				QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes );

		if( btn == QMessageBox::Yes )
			saveProjectImpl();
	}

	d->m_widget->tabs()->setCurrentIndex( 0 );

	TopGui::instance()->saveCfg( nullptr );

	for( auto * p : d->m_widget->pages() )
		p->scene()->clearSelection();

	QApplication::quit();
}

void
ProjectWindow::showHideGrid( bool show )
{
	GridMode mode = ( show ? ShowGrid : NoGrid );

	d->m_cfg.set_showGrid( show );

	foreach( PageView * view, d->m_widget->pages() )
		view->page()->setGridMode( mode );
}

void
ProjectWindow::snapGrid( bool on )
{
	foreach( PageView * view, d->m_widget->pages() )
		view->page()->enableSnap( on );

	PageAction::instance()->enableSnap( on );
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
			d->m_cfg.set_defaultGridStep( MmPx::instance().toMmX( dlg.gridStep() ) );

			foreach( PageView * view, d->m_widget->pages() )
				view->page()->setGridStep( dlg.gridStep() );
		}
		else if( index > 0 )
			d->m_widget->pages()[ index - 1 ]->page()->setGridStep(
				dlg.gridStep() );
	}
}

void
ProjectWindow::setGridStep()
{
	const int index = d->m_widget->tabs()->currentIndex();

	int step = MmPx::instance().fromMmX( d->m_cfg.defaultGridStep() );
	bool forAll = true;

	if( index > 0 )
		step = d->m_widget->pages().at( index - 1 )->page()->gridStep();

	GridStepDlg dlg( step, forAll, this );

	setGridStep( step, forAll );
}

void
ProjectWindow::openProject()
{
	const QString fileName =
		QFileDialog::getOpenFileName( this, tr( "Select Project to Open..." ),
			( d->m_openFolder.isEmpty() ? QStandardPaths::standardLocations(
				QStandardPaths::DocumentsLocation ).constFirst() : d->m_openFolder ),
			tr( "Prototyper Project (*.prototyper)" ) );

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

	d->m_fileName.clear();

	setWindowTitle( tr( "Prototyper - Unsaved[*]" ) );

	setWindowModified( false );

	switchToSelectMode();
	d->cleanPages();
	tabChanged( 0 );
}

void
ProjectWindow::saveProjectImpl( const QString & fileName )
{
	static const QString ext = QStringLiteral( ".prototyper" );

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

		d->cleanPages();

		d->m_widget->clearCommentChanged();
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
		( d->m_openFolder.isEmpty() ? QStandardPaths::standardLocations(
			QStandardPaths::DocumentsLocation ).constFirst() : d->m_openFolder ),
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
	setWindowModified( true );
}

void
ProjectWindow::drawLine()
{
	d->clearEditModeInTexts();

	PageAction::instance()->setMode( PageAction::DrawLine );

	d->prepareForDrawing();
}

void
ProjectWindow::drawRect()
{
	d->clearEditModeInTexts();

	PageAction::instance()->setMode( PageAction::DrawRect );

	d->prepareForDrawing();
}

void
ProjectWindow::drawPolyline( bool on )
{
	d->clearEditModeInTexts();

	if( on )
	{
		PageAction::instance()->setMode( PageAction::DrawLine );

		PageAction::instance()->setFlag( PageAction::Polyline );

		d->prepareForDrawing();
	}
	else
		PageAction::instance()->clearFlag( PageAction::Polyline );
}

void
ProjectWindow::insertText()
{
	d->clearEditModeInTexts();

	PageAction::instance()->setMode( PageAction::InsertText );

	d->prepareForDrawing( true );
}

void
ProjectWindow::insertImage()
{
	d->clearEditModeInTexts();

	const QString fileName =
		QFileDialog::getOpenFileName( this, tr( "Select Image" ),
			QStandardPaths::standardLocations(
				QStandardPaths::PicturesLocation ).constFirst(),
			tr( "Image Files (*.png *.jpg *.jpeg *.bmp)" ), nullptr,
			QFileDialog::DontUseNativeDialog );

	QApplication::processEvents();

	if( !fileName.isEmpty() )
	{
		QImage image( fileName );

		if( !image.isNull() )
		{
			QApplication::processEvents();

			auto * drag = new QDrag( this );
			auto * mimeData = new QMimeData;

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
ProjectWindow::group()
{
	PageAction::instance()->page()->group();
}

void
ProjectWindow::ungroup()
{
	PageAction::instance()->page()->ungroup();
}

void
ProjectWindow::select( bool checked )
{
	d->m_strokeColor->setEnabled( checked  );
	d->m_fillColor->setEnabled( checked );
	d->m_group->setEnabled( checked );
	d->m_ungroup->setEnabled( checked );
	d->m_alignHorizLeft->setEnabled( checked );
	d->m_alignHorizCenter->setEnabled( checked );
	d->m_alignHorizRight->setEnabled( checked );
	d->m_alignVertTop->setEnabled( checked );
	d->m_alignVertCenter->setEnabled( checked );
	d->m_alignVertBottom->setEnabled( checked );
	d->m_toTop->setEnabled( checked );
	d->m_up->setEnabled( checked );
	d->m_down->setEnabled( checked );
	d->m_toBottom->setEnabled( checked );

	d->clearEditModeInTexts();

	if( checked )
	{
		PageAction::instance()->setMode( PageAction::Select );

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
ProjectWindow::fillColor()
{
	const auto idx = d->m_widget->tabs()->currentIndex();

	auto c = PageAction::instance()->fillColor();

	if( idx > 0 )
	{
		const auto * v = d->m_widget->pages().at( idx - 1 );
		const auto s = v->pageScene()->selectedItems();

		if( s.size() == 1 )
		{
			auto * o = dynamic_cast< FormObject* > ( s.first() );

			if( o )
				c = o->objectBrush().color();
		}
	}

	c = QColorDialog::getColor(
		c, this, tr( "Select Fill Color..." ),
		QColorDialog::ShowAlphaChannel );

	if( c.isValid() )
	{
		PageAction::instance()->setFillColor( c );

		if( PageAction::instance()->page() )
		{
			QList< QGraphicsItem* > selected =
				PageAction::instance()->page()->scene()->selectedItems();

			if( !selected.isEmpty() )
			{
				foreach( QGraphicsItem * item, selected )
				{
					auto * obj = dynamic_cast< FormObject* > ( item );

					if( obj )
						obj->setObjectBrush( QBrush( c ) );
				}
			}
			else
				PageAction::instance()->setFillColor( c );
		}
	}
}

void
ProjectWindow::strokeColor()
{
	const auto idx = d->m_widget->tabs()->currentIndex();

	auto c = PageAction::instance()->strokeColor();

	if( idx > 0 )
	{
		const auto * v = d->m_widget->pages().at( idx - 1 );
		const auto s = v->pageScene()->selectedItems();

		if( s.size() == 1 )
		{
			auto * o = dynamic_cast< FormObject* > ( s.first() );

			if( o )
				c = o->objectPen().color();
		}
	}

	c = QColorDialog::getColor(
		c, this, tr( "Select Stroke Color..." ),
		QColorDialog::ShowAlphaChannel );

	if( c.isValid() )
	{
		if( PageAction::instance()->page() )
		{
			QList< QGraphicsItem* > selected =
				PageAction::instance()->page()->scene()->selectedItems();

			if( !selected.isEmpty() )
			{
				foreach( QGraphicsItem * item, selected )
				{
					auto * obj = dynamic_cast< FormObject* > ( item );

					if( obj )
						obj->setObjectPen( QPen( c ) );
				}
			}
			else
				PageAction::instance()->setStrokeColor( c );
		}
	}
}

void
ProjectWindow::tabChanged( int index )
{
	d->clearEditModeInTexts();

	if( index > 0 )
	{
		d->m_formToolBar->show();
		d->m_stdItemsToolBar->show();
		d->m_zoomToolBar->show();
		d->m_widget->descriptionTab()->toolBar()->hide();

		PageAction::instance()->setPage(
			d->m_widget->pages().at( index - 1 )->page() );

		d->m_group->setEnabled( true );
		d->m_select->setEnabled( true );
		d->m_ungroup->setEnabled( true );
		d->m_drawLine->setEnabled( true );
		d->m_alignVertTop->setEnabled( true );
		d->m_drawPolyLine->setEnabled( true );
		d->m_alignHorizLeft->setEnabled( true );
		d->m_alignHorizRight->setEnabled( true );
		d->m_alignVertBottom->setEnabled( true );
		d->m_alignHorizCenter->setEnabled( true );
		d->m_alignVertCenter->setEnabled( true );
		d->m_drawRect->setEnabled( true );
		d->m_insertText->setEnabled( true );
		d->m_insertImage->setEnabled( true );
		d->m_drawButton->setEnabled( true );
		d->m_drawComboBox->setEnabled( true );
		d->m_drawRadioButton->setEnabled( true );
		d->m_drawCheckBox->setEnabled( true );
		d->m_drawHSlider->setEnabled( true );
		d->m_drawVSlider->setEnabled( true );
		d->m_drawSpinbox->setEnabled( true );

		zoomChanged();
		selectionChanged();
		d->m_propertiesDock->show();
		d->m_propertiesDock->toggleViewAction()->setEnabled( true );
	}
	else
	{
		d->m_formToolBar->hide();
		d->m_stdItemsToolBar->hide();
		d->m_zoomToolBar->hide();
		d->m_widget->descriptionTab()->toolBar()->show();

		PageAction::instance()->setPage( nullptr );

		d->m_group->setEnabled( false );
		d->m_select->setEnabled( false );
		d->m_ungroup->setEnabled( false );
		d->m_drawLine->setEnabled( false );
		d->m_alignVertTop->setEnabled( false );
		d->m_drawPolyLine->setEnabled( false );
		d->m_alignHorizLeft->setEnabled( false );
		d->m_alignHorizRight->setEnabled( false );
		d->m_alignVertBottom->setEnabled( false );
		d->m_alignHorizCenter->setEnabled( false );
		d->m_alignVertCenter->setEnabled( false );
		d->m_drawRect->setEnabled( false );
		d->m_insertText->setEnabled( false );
		d->m_insertImage->setEnabled( false );
		d->m_drawButton->setEnabled( false );
		d->m_drawComboBox->setEnabled( false );
		d->m_drawRadioButton->setEnabled( false );
		d->m_drawCheckBox->setEnabled( false );
		d->m_drawHSlider->setEnabled( false );
		d->m_drawVSlider->setEnabled( false );
		d->m_drawSpinbox->setEnabled( false );
		d->m_zoomIn->setEnabled( false );
		d->m_zoomOut->setEnabled( false );
		d->m_zoomOriginal->setEnabled( false );
		d->m_duplicate->setEnabled( false );
		d->m_toTop->setEnabled( false );
		d->m_up->setEnabled( false );
		d->m_down->setEnabled( false );
		d->m_toBottom->setEnabled( false );

		d->m_propertiesDock->hide();
		d->m_propertiesDock->toggleViewAction()->setEnabled( false );
	}
}

void
ProjectWindow::zoomChanged()
{
	const int index = d->m_widget->tabs()->currentIndex() - 1;

	if( d->m_widget->pages().at( index )->scaleValue() < c_maxZoom )
		d->m_zoomIn->setEnabled( true );
	else
		d->m_zoomIn->setEnabled( false );

	if( d->m_widget->pages().at( index )->scaleValue() > c_minZoom )
		d->m_zoomOut->setEnabled( true );
	else
		d->m_zoomOut->setEnabled( false );

	if( qAbs( d->m_widget->pages().at( index )->scaleValue() - 1.0 ) > c_maxZero )
		d->m_zoomOriginal->setEnabled( true );
	else
		d->m_zoomOriginal->setEnabled( false );
}

void
ProjectWindow::selectionChanged()
{
	const auto index = d->m_widget->tabs()->currentIndex();

	if( index > 0 )
	{
		const auto s = d->m_widget->pages().at( index - 1 )->pageScene()->selectedItems();

		d->m_duplicate->setEnabled( !s.isEmpty() );

		d->m_toTop->setEnabled( !s.isEmpty() );
		d->m_up->setEnabled( !s.isEmpty() );
		d->m_down->setEnabled( !s.isEmpty() );
		d->m_toBottom->setEnabled( !s.isEmpty() );

		if( s.size() > 1 )
		{
			d->m_alignHorizLeft->setEnabled( true );
			d->m_alignHorizRight->setEnabled( true );
			d->m_alignVertBottom->setEnabled( true );
			d->m_alignHorizCenter->setEnabled( true );
			d->m_alignVertCenter->setEnabled( true );
			d->m_alignVertTop->setEnabled( true );
			d->m_group->setEnabled( true );
		}
		else
		{
			d->m_alignHorizLeft->setEnabled( false );
			d->m_alignHorizRight->setEnabled( false );
			d->m_alignVertBottom->setEnabled( false );
			d->m_alignHorizCenter->setEnabled( false );
			d->m_alignVertCenter->setEnabled( false );
			d->m_alignVertTop->setEnabled( false );
			d->m_group->setEnabled( false );
		}

		if( s.size() == 1 && dynamic_cast< FormGroup* > ( s.first() ) )
			d->m_ungroup->setEnabled( true );
		else
			d->m_ungroup->setEnabled( false );

		if( s.size() == 1 )
		{
			auto * o = dynamic_cast< FormObject* > ( s.first() );

			if( o )
			{
				auto * widget = o->properties( this );

				if( widget )
				{
					d->m_propertiesScrollArea->setWidget( widget );
					widget->show();
				}
				else
				{
					auto w = new QWidget( d->m_propertiesScrollArea );
					d->m_propertiesScrollArea->setWidget( w );
					w->show();
				}
			}
			else
			{
				auto w = new QWidget( d->m_propertiesScrollArea );
				d->m_propertiesScrollArea->setWidget( w );
				w->show();
			}
		}
		else
		{
			auto w = new QWidget( d->m_propertiesScrollArea );
			d->m_propertiesScrollArea->setWidget( w );
			w->show();
		}
	}
}

void
ProjectWindow::exportToPDf()
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

	QString fileName = QFileDialog::getSaveFileName( this,
		tr( "Select file to export project..." ),
		QStandardPaths::standardLocations(
			QStandardPaths::DocumentsLocation ).constFirst(),
		tr( "PDF (*.pdf)" ) );

	if( !fileName.isEmpty() )
	{
		if( !fileName.endsWith( QStringLiteral( ".pdf" ), Qt::CaseInsensitive ) )
			fileName.append( QStringLiteral( ".pdf" ) );

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
ProjectWindow::exportToHtml()
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

	QString fileName = QFileDialog::getSaveFileName( this,
		tr( "Select file to export project..." ),
		QStandardPaths::standardLocations(
			QStandardPaths::DocumentsLocation ).constFirst(),
		tr( "HTML (*.htm *.html)" ) );

	if( !fileName.isEmpty() )
	{
		if( !fileName.endsWith( QStringLiteral( ".htm" ), Qt::CaseInsensitive ) &&
			!fileName.endsWith( QStringLiteral( ".html" ), Qt::CaseInsensitive ) )
				fileName.append( QStringLiteral( ".html" ) );

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
ProjectWindow::exportToSvg()
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

	QString dirName = QFileDialog::getExistingDirectory( this,
		tr( "Select directory to export project..." ),
		QStandardPaths::standardLocations(
			QStandardPaths::DocumentsLocation ).constFirst() );

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
ProjectWindow::about()
{
	QMessageBox::about( this, tr( "About Prototyper" ),
		tr( "Prototyper - Simple UI prototyping software.\n\n"
			"Version %1\n\n"
			"Author - Igor Mironchik (igor.mironchik at gmail dot com).\n\n"
			"Copyright (c) 2016-2020 Igor Mironchik.\n\n"
			"Licensed under GNU GPL 3.0." ).arg( c_version ) );
}

void
ProjectWindow::aboutQt()
{
	QMessageBox::aboutQt( this );
}

void
ProjectWindow::drawButton()
{
	d->clearEditModeInTexts();

	PageAction::instance()->setMode( PageAction::DrawButton );

	d->prepareForDrawing();
}

void
ProjectWindow::drawComboBox()
{
	d->clearEditModeInTexts();

	PageAction::instance()->setMode( PageAction::DrawComboBox );

	d->prepareForDrawing();
}

void
ProjectWindow::drawRadioButton()
{
	d->clearEditModeInTexts();

	PageAction::instance()->setMode( PageAction::DrawRadioButton );

	d->prepareForDrawing();
}

void
ProjectWindow::drawCheckBox()
{
	d->clearEditModeInTexts();

	PageAction::instance()->setMode( PageAction::DrawCheckBox );

	d->prepareForDrawing();
}

void
ProjectWindow::drawHSlider()
{
	d->clearEditModeInTexts();

	PageAction::instance()->setMode( PageAction::DrawHSlider );

	d->prepareForDrawing();
}

void
ProjectWindow::drawVSlider()
{
	d->clearEditModeInTexts();

	PageAction::instance()->setMode( PageAction::DrawVSlider );

	d->prepareForDrawing();
}

void
ProjectWindow::drawSpinbox()
{
	d->clearEditModeInTexts();

	PageAction::instance()->setMode( PageAction::DrawSpinBox );

	d->prepareForDrawing();
}

void
ProjectWindow::alignVerticalTop()
{
	PageAction::instance()->page()->alignVerticalTop();
}

void
ProjectWindow::alignVerticalCenter()
{
	PageAction::instance()->page()->alignVerticalCenter();
}

void
ProjectWindow::alignVerticalBottom()
{
	PageAction::instance()->page()->alignVerticalBottom();
}

void
ProjectWindow::alignHorizontalLeft()
{
	PageAction::instance()->page()->alignHorizontalLeft();
}

void
ProjectWindow::alignHorizontalCenter()
{
	PageAction::instance()->page()->alignHorizontalCenter();
}

void
ProjectWindow::alignHorizontalRight()
{
	PageAction::instance()->page()->alignHorizontalRight();
}

void
ProjectWindow::canUndoChanged( bool canUndo )
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
	else if( d->m_widget->isCommentChanged() )
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

	setWindowModified( can );
}

void
ProjectWindow::pageAdded( Prototyper::Core::PageView * form )
{
	d->m_addedForms.append( form );
}

void
ProjectWindow::pageDeleted( Prototyper::Core::PageView * form )
{
	d->m_deletedForms.append( form );

	if( d->m_addedForms.contains( form ) )
		d->m_addedForms.removeOne( form );
}

void
ProjectWindow::zoomIn()
{
	const int index = d->m_widget->tabs()->currentIndex() - 1;

	d->m_widget->pages().at( index )->setScaleValue(
		d->m_widget->pages().at( index )->scaleValue() + PageView::zoomFactor() );

	if( qAbs( d->m_widget->pages().at( index )->scaleValue() - c_maxZoom ) < c_maxZero )
		d->m_zoomIn->setEnabled( false );

	d->m_zoomOriginal->setEnabled( true );
}

void
ProjectWindow::zoomOut()
{
	const int index = d->m_widget->tabs()->currentIndex() - 1;

	d->m_widget->pages().at( index )->setScaleValue(
		d->m_widget->pages().at( index )->scaleValue() - PageView::zoomFactor() );

	if( qAbs( d->m_widget->pages().at( index )->scaleValue() - c_minZoom ) < c_maxZero )
		d->m_zoomOut->setEnabled( false );

	d->m_zoomOriginal->setEnabled( true );
}

void
ProjectWindow::zoomOriginal()
{
	const int index = d->m_widget->tabs()->currentIndex() - 1;

	d->m_widget->pages().at( index )->setScaleValue( 1.0 );

	d->m_zoomIn->setEnabled( true );
	d->m_zoomOut->setEnabled( true );
	d->m_zoomOriginal->setEnabled( false );
}

void
ProjectWindow::duplicate()
{
	const auto idx = d->m_widget->tabs()->currentIndex();

	if( idx > 0 )
	{
		const auto s = d->m_widget->pages().at( idx - 1 )->pageScene()->selectedItems();

		d->m_widget->pages().at( idx - 1 )->pageScene()->clearSelection();

		if( !s.isEmpty() )
		{
			QStringList orig;
			QStringList dupl;

			for( const auto & i : s )
			{
				auto * o = dynamic_cast< FormObject* > ( i );

				if( o )
				{
					auto * n = o->clone();

					orig.append( o->objectId() );
					dupl.append( n->objectId() );

					n->setPosition( n->position() +
							QPointF( MmPx::instance().fromMmX( d->m_cfg.defaultGridStep() ),
						MmPx::instance().fromMmX( d->m_cfg.defaultGridStep() ) ), false );

					auto * g = dynamic_cast< QGraphicsItem* > ( n );

					if( g )
					{
						g->setFlag( QGraphicsItem::ItemIsSelectable, true );
						g->setSelected( true );
					}
				}
			}

			d->m_widget->pages().at( idx - 1 )->page()->undoStack()->push(
				new UndoDuplicate( d->m_widget->pages().at( idx - 1 )->page(),
					orig, dupl, MmPx::instance().fromMmX( d->m_cfg.defaultGridStep() ) ) );

			projectChanged();
		}
	}
}

namespace /* anonymous */ {

//! Change Z index.
void changeZ( const QList< QGraphicsItem* > & s, qreal delta, Page * page )
{
	UndoChangeZ::ZAndIds origZ;
	UndoChangeZ::ZAndIds newZ;

	for( const auto & i : s )
	{
		auto * o = dynamic_cast< FormObject* > ( i );

		if( o )
		{
			origZ.append( qMakePair( o->objectId(), i->zValue() ) );

			const auto z = i->zValue() + delta;

			newZ.append( qMakePair( o->objectId(), z ) );

			i->setZValue( z );
		}
	}

	if( !origZ.isEmpty() )
		page->undoStack()->push( new UndoChangeZ( page, origZ, newZ ) );
} // upZ

}

void
ProjectWindow::toTop()
{
	const auto idx = d->m_widget->tabs()->currentIndex();

	if( idx > 0 )
	{
		const auto * v = d->m_widget->pages().at( idx - 1 );
		const auto min = v->page()->topZ() + 1.0;
		const auto s = v->pageScene()->selectedItems();
		const auto minMax = minMaxZ( s );
		const auto delta = min - minMax.first;

		changeZ( s, delta, v->page() );

		setWindowModified( true );
	}
}

void
ProjectWindow::raise()
{
	const auto idx = d->m_widget->tabs()->currentIndex();

	if( idx > 0 )
	{
		const auto * v = d->m_widget->pages().at( idx - 1 );

		changeZ( v->pageScene()->selectedItems(), 1.0, v->page() );

		setWindowModified( true );
	}
}

void
ProjectWindow::lower()
{
	const auto idx = d->m_widget->tabs()->currentIndex();

	if( idx > 0 )
	{
		const auto * v = d->m_widget->pages().at( idx - 1 );

		changeZ( v->pageScene()->selectedItems(), -1.0, v->page() );

		setWindowModified( true );
	}
}

void
ProjectWindow::toBottom()
{
	const auto idx = d->m_widget->tabs()->currentIndex();

	if( idx > 0 )
	{
		const auto * v = d->m_widget->pages().at( idx - 1 );
		const auto max = v->page()->bottomZ() - 1.0;
		const auto s = v->pageScene()->selectedItems();
		const auto minMax = minMaxZ( s );
		const auto delta = max - minMax.second;

		changeZ( s, delta, v->page() );

		setWindowModified( true );
	}
}

} /* namespace Core */

} /* namespace Prototyper */
