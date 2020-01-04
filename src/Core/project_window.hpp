
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

#ifndef PROTOTYPER__CORE__PROJECT_WINDOW_HPP__INCLUDED
#define PROTOTYPER__CORE__PROJECT_WINDOW_HPP__INCLUDED

// Qt include.
#include <QMainWindow>
#include <QScopedPointer>


namespace Prototyper {

namespace Core {

class ProjectWidget;
class TabsList;
class PageView;


//
// ProjectWindow
//

class ProjectWindowPrivate;

//! Main window with project.
class ProjectWindow
	:	public QMainWindow
{
	Q_OBJECT

public:
	ProjectWindow( QWidget * parent = 0, Qt::WindowFlags f = 0 );
	~ProjectWindow();

	//! \return Project widget.
	ProjectWidget * projectWidget() const;

	//! \return Project's file name.
	const QString & projectFileName() const;

	//! \return Show/hide grid action.
	QAction * showHideGridAction() const;
	//! \return Grid step action.
	QAction * gridStepAction() const;

	//! \return Tabs list widget.
	TabsList * tabsList() const;

public slots:
	//! Read project.
	void readProject( const QString & fileName );
	//! Set grid step.
	void setGridStep( int step, bool forAll );
	//! Switch to polyline mode.
	void switchToPolylineMode();
	//! Switch to select mode.
	void switchToSelectMode();

protected:
	void closeEvent( QCloseEvent * e ) Q_DECL_OVERRIDE;

private slots:
	//! Quit.
	void p_quit();
	//! Show/hide grid.
	void p_showHideGrid( bool show );
	//! Enable/disable snap to grid.
	void p_snapGrid( bool on );
	//! Set grid step.
	void p_setGridStep();
	//! Open project.
	void p_openProject();
	//! New project.
	void p_newProject();
	//! Save project.
	void p_saveProjectImpl( const QString & fileName = QString() );
	//! Save project.
	void p_saveProject();
	//! Save project as.
	void p_saveProjectAs();
	//! Project changed.
	void p_projectChanged();
	//! Draw line.
	void p_drawLine();
	//! Draw rect.
	void p_drawRect();
	//! Draw polyline.
	void p_drawPolyline( bool on );
	//! Insert text.
	void p_insertText();
	//! Insert image.
	void p_insertImage();
	//! Group.
	void p_group();
	//! Ungroup.
	void p_ungroup();
	//! Select.
	void p_select( bool checked );
	//! Fill color.
	void p_fillColor();
	//! Stroke color.
	void p_strokeColor();
	//! Tab changed.
	void p_tabChanged( int index );
	//! Export to PDF.
	void p_exportToPDf();
	//! Export to HTML.
	void p_exportToHtml();
	//! Export to SVG images.
	void p_exportToSvg();
	//! Show about dialog.
	void p_about();
	//! Show about Qt dialog.
	void p_aboutQt();
	//! Draw button.
	void p_drawButton();
	//! Draw combobox.
	void p_drawComboBox();
	//! Draw radio button.
	void p_drawRadioButton();
	//! Draw check box.
	void p_drawCheckBox();
	//! Draw horizontal slider.
	void p_drawHSlider();
	//! Draw vertical slider.
	void p_drawVSlider();
	//! Draw spinbox.
	void p_drawSpinbox();
	//! Align vertical top.
	void p_alignVerticalTop();
	//! Align vertical center.
	void p_alignVerticalCenter();
	//! Align vertical bottom.
	void p_alignVerticalBottom();
	//! Align horizontal left.
	void p_alignHorizontalLeft();
	//! Align horizontal center.
	void p_alignHorizontalCenter();
	//! Align horizontal right.
	void p_alignHorizontalRight();
	//! Can undo changed.
	void p_canUndoChanged( bool canUndo );
	//! Form added.
	void p_formAdded( PageView * form );
	//! Form deleted.
	void p_formDeleted( PageView * form );
	//! Zoom in.
	void p_zoomIn();
	//! Zoom out.
	void p_zoomOut();
	//! Zoom original.
	void p_zoomOriginal();
	//! Zoom changed.
	void p_zoomChanged();
	//! Selection changed.
	void p_selectionChanged();

private:
	friend class ProjectWindowPrivate;
	friend class TopGuiPrivate;
	friend class ProjectWidget;
	friend class ProjectWidgetPrivate;

	Q_DISABLE_COPY( ProjectWindow )

	QScopedPointer< ProjectWindowPrivate > d;
}; // class ProjectWindow

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__PROJECT_WINDOW_HPP__INCLUDED
