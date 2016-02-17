
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
#include "link_dlg.hpp"
#include "ui_link_dlg.h"

// Qt include.
#include <QPushButton>
#include <QDialogButtonBox>
#include <QListView>
#include <QStringListModel>


namespace Prototyper {

namespace Core {

//
// LinkDlgPrivate
//

class LinkDlgPrivate {
public:
	LinkDlgPrivate( const QStringList & names, LinkDlg * parent )
		:	q( parent )
		,	m_names( names )
		,	m_model( 0 )
	{
	}

	//! Init.
	void init();

	//! Parent.
	LinkDlg * q;
	//! Ui.
	Ui::LinkDlg m_ui;
	//! Names.
	QStringList m_names;
	//! Model.
	QStringListModel * m_model;
}; // class LinkDlgPrivate

void
LinkDlgPrivate::init()
{
	m_ui.setupUi( q );

	m_model = new QStringListModel( m_names, q );

	m_ui.m_list->setModel( m_model );

	m_ui.m_btns->button( QDialogButtonBox::Ok )->setEnabled( false );

	LinkDlg::connect( m_ui.m_list, &QListView::clicked,
		q, &LinkDlg::clicked );
}


//
// LinkDlg
//

LinkDlg::LinkDlg( const QStringList & names,
	QWidget * parent, Qt::WindowFlags f )
	:	QDialog( parent, f )
	,	d( new LinkDlgPrivate( names, this ) )
{
	d->init();
}

LinkDlg::~LinkDlg()
{
}

QString
LinkDlg::form() const
{
	return d->m_ui.m_list->currentIndex().data().toString();
}

void
LinkDlg::clicked( const QModelIndex & index )
{
	if( index.isValid() )
		d->m_ui.m_btns->button( QDialogButtonBox::Ok )->setEnabled( true );
	else
		d->m_ui.m_btns->button( QDialogButtonBox::Ok )->setEnabled( false );
}

} /* namespace Core */

} /* namespace Prototyper */
