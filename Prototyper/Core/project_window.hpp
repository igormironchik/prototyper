
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

public slots:
	//! Read project.
	void readProject( const QString & fileName );
	//! Set grid step.
	void setGridStep( int step, bool forAll );

protected:
	void closeEvent( QCloseEvent * e ) Q_DECL_OVERRIDE;

private slots:
	//! Quit.
	void p_quit();
	//! Show/hide grid.
	void p_showHideGrid( bool show );
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
	//! Draw polyline.
	void p_drawPolyline();
	//! Insert text.
	void p_insertText();
	//! Insert image.
	void p_insertImage();
	//! Group.
	void p_group();
	//! Ungroup.
	void p_ungroup();
	//! Select.
	void p_select();
	//! Move.
	void p_move();
	//! Fill color.
	void p_fillColor();
	//! Stroke color.
	void p_strokeColor();
	//! Tab changed.
	void p_tabChanged( int index );

private:
	friend class ProjectWindowPrivate;

	Q_DISABLE_COPY( ProjectWindow )

	QScopedPointer< ProjectWindowPrivate > d;
}; // class ProjectWindow

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__PROJECT_WINDOW_HPP__INCLUDED
