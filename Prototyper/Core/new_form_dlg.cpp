
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
#include "new_form_dlg.hpp"
#include "ui_new_form_dlg.h"

// Qt include.
#include <QLineEdit>
#include <QPushButton>
#include <QPalette>
#include <QDialogButtonBox>
#include <QSpinBox>


namespace Prototyper {

namespace Core {

//
// NewFormDlgPrivate
//

class NewFormDlgPrivate {
public:
	NewFormDlgPrivate( const QStringList & names, const QString & title,
		NewFormDlg * parent )
		:	q( parent )
		,	m_names( names )
		,	m_title( title )
	{
	}

	//! Init.
	void init();

	//! Parent.
	NewFormDlg * q;
	//! Ui.
	Ui::NewFormDlg m_ui;
	//! Names.
	const QStringList & m_names;
	//! Normal text color.
	QColor m_color;
	//! Title.
	QString m_title;
}; // class NewFormDlgPrivate

void
NewFormDlgPrivate::init()
{
	m_ui.setupUi( q );

	q->setWindowTitle( m_title );

	m_color = m_ui.m_edit->palette().color( QPalette::Text );

	m_ui.m_btns->button( QDialogButtonBox::Ok )->setEnabled( false );

	NewFormDlg::connect( m_ui.m_edit, &QLineEdit::textChanged,
		q, &NewFormDlg::textChanged );
}


//
// NewFormDlg
//

NewFormDlg::NewFormDlg( const QStringList & names,
	const QString & title, QWidget * parent, Qt::WindowFlags f )
	:	QDialog( parent, f )
	,	d( new NewFormDlgPrivate( names, title, this ) )
{
	d->init();
}

NewFormDlg::~NewFormDlg()
{
}

QString
NewFormDlg::name() const
{
	return d->m_ui.m_edit->text().trimmed();
}

QSize
NewFormDlg::size() const
{
	return QSize( d->m_ui.m_width->value(),
		d->m_ui.m_height->value() );
}

void
NewFormDlg::textChanged( const QString & text )
{
	if( !text.isEmpty() && !d->m_names.contains( text.trimmed() ) )
	{
		QPalette p = d->m_ui.m_edit->palette();
		p.setColor( QPalette::Text, d->m_color );
		d->m_ui.m_edit->setPalette( p );

		d->m_ui.m_btns->button( QDialogButtonBox::Ok )->setEnabled( true );
	}
	else
	{
		if( text.isEmpty() )
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
