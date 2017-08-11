
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
#include "form_checkbox_properties.hpp"
#include "ui_form_checkbox_properties.h"
#include "utils.hpp"

// Qt include.
#include <QLineEdit>
#include <QSpinBox>
#include <QCheckBox>
#include <QDialogButtonBox>


namespace Prototyper {

namespace Core {

//
// CheckBoxPropertiesPrivate
//

class CheckBoxPropertiesPrivate {
public:
	explicit CheckBoxPropertiesPrivate( CheckBoxProperties * parent )
		:	q( parent )
	{
	}

	//! Init.
	void init();

	//! Parent.
	CheckBoxProperties * q;
	//! Ui.
	Ui::CheckBoxProperties m_ui;
}; // class CheckBoxPropertiesPrivate

void
CheckBoxPropertiesPrivate::init()
{
	m_ui.setupUi( q );
}


//
// FormCheckBoxProperties
//

CheckBoxProperties::CheckBoxProperties( QWidget * parent, Qt::WindowFlags f )
	:	QDialog( parent, f )
	,	d( new CheckBoxPropertiesPrivate( this ) )
{
	d->init();
}

CheckBoxProperties::~CheckBoxProperties()
{
}

Cfg::CheckBox
CheckBoxProperties::cfg() const
{
	Cfg::CheckBox res;

	Cfg::TextStyle c;

	QList< QString > style;

	if( d->m_ui.m_bold->isChecked() )
		style.append( Cfg::c_boldStyle );

	if( d->m_ui.m_italic->isChecked() )
		style.append( Cfg::c_italicStyle );

	if( d->m_ui.m_underline->isChecked() )
		style.append( Cfg::c_underlineStyle );

	if( style.isEmpty() )
		style.append( Cfg::c_normalStyle );

	c.setStyle( style );
	c.setFontSize( d->m_ui.m_fontSize->value() );
	c.setText( d->m_ui.m_text->text() );

	res.setText( c );

	res.setIsChecked( d->m_ui.m_isChecked->isChecked() );

	return res;
}

void
CheckBoxProperties::setCfg( const Cfg::CheckBox & c )
{
	if( c.text().style().contains( Cfg::c_boldStyle ) )
		d->m_ui.m_bold->setChecked( true );
	else
		d->m_ui.m_bold->setChecked( false );

	if( c.text().style().contains( Cfg::c_italicStyle ) )
		d->m_ui.m_italic->setChecked( true );
	else
		d->m_ui.m_italic->setChecked( false );

	if( c.text().style().contains( Cfg::c_underlineStyle ) )
		d->m_ui.m_underline->setChecked( true );
	else
		d->m_ui.m_underline->setChecked( false );

	d->m_ui.m_fontSize->setValue( c.text().fontSize() );

	d->m_ui.m_text->setText( c.text().text() );

	d->m_ui.m_isChecked->setChecked( c.isChecked() );
}

} /* namespace Core */

} /* namespace Prototyper */
