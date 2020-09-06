
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

// Prototyper include.
#include "link_dlg.hpp"
#include "ui_link.h"

// Qt include.
#include <QLineEdit>


namespace Prototyper {

namespace Core {

//
// LinkDlgPrivate
//

class LinkDlgPrivate {
public:
	LinkDlgPrivate( LinkDlg * parent )
		:	q( parent )
	{
	}

	//! Parent.
	LinkDlg * q;
	//! Ui.
	Ui::LinkDlg m_ui;
}; // class LinkDlgPrivate


//
// LinkDlg
//

LinkDlg::LinkDlg( QWidget * parent )
	:	QDialog( parent )
	,	d( new LinkDlgPrivate( this ) )
{
	d->m_ui.setupUi( this );
}

LinkDlg::~LinkDlg() = default;

QString
LinkDlg::url() const
{
	return d->m_ui.m_url->text();
}

QString
LinkDlg::text() const
{
	return d->m_ui.m_text->text();
}

} /* namespace Core */

} /* namespace Prototyper */
