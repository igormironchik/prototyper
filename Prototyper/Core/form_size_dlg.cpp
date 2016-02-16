
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
#include "form_size_dlg.hpp"
#include "ui_form_size_dlg.h"

// Qt include.
#include <QSpinBox>


namespace Prototyper {

namespace Core {

//
// FormSizeDlgPrivate
//

class FormSizeDlgPrivate {
public:
	FormSizeDlgPrivate( int w, int h, FormSizeDlg * parent )
		:	q( parent )
	{
		init( w, h );
	}

	//! Init.
	void init( int w, int h );

	//! Parent.
	FormSizeDlg * q;
	//! Ui.
	Ui::FormSizeDlg m_ui;
}; // class FormSizeDlgPrivate

void
FormSizeDlgPrivate::init( int w, int h )
{
	m_ui.setupUi( q );

	m_ui.m_width->setValue( w );
	m_ui.m_height->setValue( h );
}


//
// FormSizeDlg
//

FormSizeDlg::FormSizeDlg( int w, int h,
	QWidget * parent, Qt::WindowFlags f )
	:	QDialog( parent, f )
	,	d( new FormSizeDlgPrivate( w, h, this ) )
{
}

FormSizeDlg::~FormSizeDlg()
{
}

QSize
FormSizeDlg::size() const
{
	return QSize( d->m_ui.m_width->value(),
		d->m_ui.m_height->value() );
}

} /* namespace Core */

} /* namespace Prototyper */
