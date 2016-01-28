
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
#include "project_widget.hpp"


namespace Prototyper {

namespace Core {

//
// ProjectWidgetPrivate
//

class ProjectWidgetPrivate {
public:
	ProjectWidgetPrivate( ProjectWidget * parent )
		:	q( parent )
	{
	}

	//! Parent.
	ProjectWidget * q;
}; // class ProjectWidgetPrivate


//
// ProjectWidget
//

ProjectWidget::ProjectWidget( QWidget * parent, Qt::WindowFlags f )
	:	QMainWindow( parent, f )
	,	d( new ProjectWidgetPrivate( this ) )
{
}

ProjectWidget::~ProjectWidget()
{
}

} /* namespace Core */

} /* namespace Prototyper */
