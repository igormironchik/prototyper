
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
#include "project_description_tab.hpp"

// Qt include.
#include <QTabWidget>
#include <QVBoxLayout>


namespace Prototyper {

namespace Core {

//
// ProjectWidgetPrivate
//

class ProjectWidgetPrivate {
public:
	ProjectWidgetPrivate( ProjectWidget * parent )
		:	q( parent )
		,	m_tabs( 0 )
		,	m_desc( 0 )
	{
	}

	//! Init.
	void init();

	//! Parent.
	ProjectWidget * q;
	//! Tabs.
	QTabWidget * m_tabs;
	//! Desc tab.
	ProjectDescTab * m_desc;
}; // class ProjectWidgetPrivate

void
ProjectWidgetPrivate::init()
{
	QVBoxLayout * layout = new QVBoxLayout( q );

	m_tabs = new QTabWidget( q );
	m_tabs->setTabPosition( QTabWidget::South );

	layout->addWidget( m_tabs );

	m_desc = new ProjectDescTab( m_tabs );
	m_tabs->addTab( m_desc, ProjectWidget::tr( "Project" ) );
}


//
// ProjectWidget
//

ProjectWidget::ProjectWidget( QWidget * parent, Qt::WindowFlags f )
	:	QWidget( parent, f )
	,	d( new ProjectWidgetPrivate( this ) )
{
	d->init();
}

ProjectWidget::~ProjectWidget()
{
}

} /* namespace Core */

} /* namespace Prototyper */
