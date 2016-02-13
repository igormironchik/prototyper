
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
#include "form_hierarchy_widget.hpp"
#include "form_hierarchy_model.hpp"

// Qt include.
#include <QTreeView>


namespace Prototyper {

namespace Core {

//
// FormHierarchyWidgetPrivate
//

class FormHierarchyWidgetPrivate {
public:
	FormHierarchyWidgetPrivate( FormHierarchyWidget * parent )
		:	q( parent )
		,	m_view( 0 )
		,	m_model( 0 )
	{
	}

	//! Init.
	void init();

	//! Parent.
	FormHierarchyWidget * q;
	//! Tree.
	QTreeView * m_view;
	//! Model.
	FormHierarchyModel * m_model;
}; // class FormHierarchyWidgetPrivate

void
FormHierarchyWidgetPrivate::init()
{
	m_view = new QTreeView( q );

	m_view->setHeaderHidden( true );

	m_model = new FormHierarchyModel( q );

	m_view->setModel( m_model );

	q->setWindowTitle( FormHierarchyWidget::tr( "Form's Hierarchy" ) );

	q->setWidget( m_view );
}


//
// FormHierarchyWidget
//

FormHierarchyWidget::FormHierarchyWidget( QWidget * parent, Qt::WindowFlags f )
	:	QDockWidget( parent, f )
	,	d( new FormHierarchyWidgetPrivate( this ) )
{
	d->init();
}

FormHierarchyWidget::~FormHierarchyWidget()
{
}

FormHierarchyModel *
FormHierarchyWidget::model() const
{
	return d->m_model;
}

} /* namespace Core */

} /* namespace Prototyper */
