
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
#include "name_dlg.hpp"

// Qt include.
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSpacerItem>
#include <QPalette>


namespace Prototyper {

namespace Core {

//
// NameDlgPrivate
//

class NameDlgPrivate {
public:
	NameDlgPrivate( const QStringList & names, NameDlg * parent )
		:	q( parent )
		,	m_names( names )
		,	m_edit( 0 )
		,	m_ok( 0 )
	{
	}

	//! Init.
	void init();

	//! Parent.
	NameDlg * q;
	//! Names.
	const QStringList & m_names;
	//! Line edit.
	QLineEdit * m_edit;
	//! Ok button.
	QPushButton * m_ok;
	//! Normal text color.
	QColor m_color;
}; // class NameDlgPrivate

void
NameDlgPrivate::init()
{
	q->setWindowTitle( NameDlg::tr( "Enter Name..." ) );

	QVBoxLayout * layout = new QVBoxLayout( q );

	m_edit = new QLineEdit( q );
	m_edit->setPlaceholderText( NameDlg::tr( "Enter Name" ) );
	m_color = m_edit->palette().color( QPalette::Text );
	layout->addWidget( m_edit );

	QHBoxLayout * hbox = new QHBoxLayout;

	QSpacerItem * s = new QSpacerItem( 0, 0,
		QSizePolicy::Expanding, QSizePolicy::Fixed );
	hbox->addItem( s );

	m_ok = new QPushButton( NameDlg::tr( "OK" ) );
	m_ok->setEnabled( false );
	hbox->addWidget( m_ok );

	QPushButton * cancel = new QPushButton( NameDlg::tr( "Cancel" ) );
	hbox->addWidget( cancel );

	layout->addLayout( hbox );

	NameDlg::connect( m_edit, &QLineEdit::textChanged,
		q, &NameDlg::textChanged );
	NameDlg::connect( m_ok, &QPushButton::clicked,
		q, &NameDlg::okPressed );
	NameDlg::connect( cancel, &QPushButton::clicked,
		q, &NameDlg::cancelPressed );
}


//
// NameDlg
//

NameDlg::NameDlg( const QStringList & names,
	QWidget * parent, Qt::WindowFlags f )
	:	QDialog( parent, f )
	,	d( new NameDlgPrivate( names, this ) )
{
	d->init();
}

NameDlg::~NameDlg()
{
}

QString
NameDlg::name() const
{
	return d->m_edit->text();
}

void
NameDlg::okPressed()
{
	accept();
}

void
NameDlg::cancelPressed()
{
	reject();
}

void
NameDlg::textChanged( const QString & text )
{
	if( !text.isEmpty() && !d->m_names.contains( text ) )
	{
		QPalette p = d->m_edit->palette();
		p.setColor( QPalette::Text, d->m_color );
		d->m_edit->setPalette( p );

		d->m_ok->setEnabled( true );
	}
	else
	{
		QPalette p = d->m_edit->palette();
		p.setColor( QPalette::Text, Qt::red );
		d->m_edit->setPalette( p );

		d->m_ok->setEnabled( false );
	}
}

} /* namespace Core */

} /* namespace Prototyper */
