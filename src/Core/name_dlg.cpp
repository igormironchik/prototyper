
/*!
	\file

	\author Igor Mironchik (igor.mironchik at gmail dot com).

	Copyright (c) 2016-2021 Igor Mironchik

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
#include "name_dlg.hpp"
#include "ui_name_dlg.h"

// Qt include.
#include <QLineEdit>
#include <QPushButton>
#include <QPalette>
#include <QDialogButtonBox>


namespace Prototyper {

namespace Core {

//
// NameDlgPrivate
//

class NameDlgPrivate {
public:
	NameDlgPrivate( const QStringList & names, const QString & title,
		const QString & oldName, NameDlg * parent )
		:	q( parent )
		,	m_names( names )
		,	m_title( title )
		,	m_oldName( oldName )
	{
	}

	//! Init.
	void init();

	//! Parent.
	NameDlg * q;
	//! Ui.
	Ui::NameDlg m_ui;
	//! Names.
	const QStringList & m_names;
	//! Normal text color.
	QColor m_color;
	//! Title.
	QString m_title;
	//! Old name.
	QString m_oldName;
}; // class NameDlgPrivate

void
NameDlgPrivate::init()
{
	m_ui.setupUi( q );

	q->setWindowTitle( m_title );

	m_color = m_ui.m_edit->palette().color( QPalette::Text );

	m_ui.m_edit->setText( m_oldName );
	m_ui.m_edit->selectAll();

	m_ui.m_btns->button( QDialogButtonBox::Ok )->setEnabled( false );

	NameDlg::connect( m_ui.m_edit, &QLineEdit::textChanged,
		q, &NameDlg::textChanged );
}


//
// NameDlg
//

NameDlg::NameDlg( const QStringList & names,
	const QString & title, const QString & oldName, QWidget * parent, Qt::WindowFlags f )
	:	QDialog( parent, f )
	,	d( new NameDlgPrivate( names, title, oldName, this ) )
{
	d->init();
}

NameDlg::~NameDlg() = default;

QString
NameDlg::name() const
{
	return d->m_ui.m_edit->text().simplified();
}

void
NameDlg::textChanged( const QString & text )
{
	if( !text.simplified().isEmpty() && !d->m_names.contains( text.simplified() ) )
	{
		QPalette p = d->m_ui.m_edit->palette();
		p.setColor( QPalette::Text, d->m_color );
		d->m_ui.m_edit->setPalette( p );

		d->m_ui.m_btns->button( QDialogButtonBox::Ok )->setEnabled( true );
	}
	else
	{
		if( text.simplified().isEmpty() )
		{
			QPalette p = d->m_ui.m_edit->palette();
			p.setColor( QPalette::Text, d->m_color );
			d->m_ui.m_edit->setPalette( p );
		}
		else
		{
			QPalette p = d->m_ui.m_edit->palette();
			p.setColor( QPalette::Text, Qt::red );
			d->m_ui.m_edit->setPalette( p );
		}

		d->m_ui.m_btns->button( QDialogButtonBox::Ok )->setEnabled( false );
	}
}

} /* namespace Core */

} /* namespace Prototyper */
