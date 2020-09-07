
/*!
	\file

	\author Igor Mironchik (igor.mironchik at gmail dot com).

	Copyright (c) 2016-2020 Igor Mironchik

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

#ifndef PROTOTYPER__CORE__PROJECT_DESCRIPTION_TAB_HPP__INCLUDED
#define PROTOTYPER__CORE__PROJECT_DESCRIPTION_TAB_HPP__INCLUDED

// Qt include.
#include <QWidget>
#include <QScopedPointer>

QT_BEGIN_NAMESPACE
class QToolBar;
QT_END_NAMESPACE


namespace Prototyper {

namespace Core {

class TextEditor;

//
// ProjectDescTab
//

class ProjectDescTabPrivate;

//! Tab with text description of the project.
class ProjectDescTab final
	:	public QWidget
{
	Q_OBJECT

public:
	ProjectDescTab( QWidget * parent = 0, Qt::WindowFlags f = Qt::WindowFlags() );
	~ProjectDescTab() override;

	//! \return Tool bar.
	QToolBar * toolBar() const;
	//! \return Text editor.
	TextEditor * editor() const;

private:
	Q_DISABLE_COPY( ProjectDescTab )

	QScopedPointer< ProjectDescTabPrivate > d;
}; // class ProjectDescTab

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__PROJECT_DESCRIPTION_TAB_HPP__INCLUDED
