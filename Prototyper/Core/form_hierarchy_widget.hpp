
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

#ifndef PROTOTYPER__CORE__FORM_HIERARCHY_WIDGET_HPP__INCLUDED
#define PROTOTYPER__CORE__FORM_HIERARCHY_WIDGET_HPP__INCLUDED

// Qt include.
#include <QDockWidget>
#include <QScopedPointer>


namespace Prototyper {

namespace Core {

//
// FormHierarchyWidget
//

class FormHierarchyWidgetPrivate;

//! Form hierarchy.
class FormHierarchyWidget
	:	public QDockWidget
{
	Q_OBJECT

public:
	FormHierarchyWidget( QWidget * parent = 0, Qt::WindowFlags f = 0 );
	~FormHierarchyWidget();

private:
	Q_DISABLE_COPY( FormHierarchyWidget )

	QScopedPointer< FormHierarchyWidgetPrivate > d;
}; // class FormHierarchyWidget

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__FORM_HIERARCHY_WIDGET_HPP__INCLUDED
