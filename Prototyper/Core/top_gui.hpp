
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

#ifndef PROTOTYPER__CORE__TOP_GUI_HPP__INCLUDED
#define PROTOTYPER__CORE__TOP_GUI_HPP__INCLUDED

// Qt include.
#include <QObject>
#include <QScopedPointer>


namespace Prototyper {

namespace Core {

class ProjectWindow;
class PropsWindow;
class ToolsWindow;


//
// TopGui
//

class TopGuiPrivate;

//! Singleton for top GUI components.
class TopGui
	:	public QObject
{
	Q_OBJECT

private:
	TopGui();
	~TopGui();

	static void cleanup();

public:
	//! \return Instance of top GUI.
	static TopGui * instance();

	//! \return Project window.
	ProjectWindow * projectWindow();
	//! \return Tools window.
	ToolsWindow * toolsWindow();
	//! \return Properties window.
	PropsWindow * propsWindow();

	//! Save cfg.
	void saveCfg( QWidget * parent = 0 );

private:
	Q_DISABLE_COPY( TopGui )

	QScopedPointer< TopGuiPrivate > d;
}; // class TopGui

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__TOP_GUI_HPP__INCLUDED
