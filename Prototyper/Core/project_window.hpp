
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

public slots:
	//! Hide properties window;
	void hidePropsWindow();
	//! Hide tools window.
	void hideToolsWindow();

protected:
	void closeEvent( QCloseEvent * e ) Q_DECL_OVERRIDE;

private slots:
	//! Show/hide properties window.
	void showHidePropertiesWindow( bool show );
	//! Show/hide tools window.
	void showHideToolsWindow( bool show );
	//! Quit.
	void quit();
	//! Show/hide grid.
	void showHideGrid( bool show );
	//! Set grid step.
	void setGridStep();

private:
	friend class ProjectWindowPrivate;

	Q_DISABLE_COPY( ProjectWindow )

	QScopedPointer< ProjectWindowPrivate > d;
}; // class ProjectWindow

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__PROJECT_WINDOW_HPP__INCLUDED
