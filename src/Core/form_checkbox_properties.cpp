
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

	std::vector< QString > style;

	if( d->m_ui.m_bold->isChecked() )
		style.push_back( Cfg::c_boldStyle );

	if( d->m_ui.m_italic->isChecked() )
		style.push_back( Cfg::c_italicStyle );

	if( d->m_ui.m_underline->isChecked() )
		style.push_back( Cfg::c_underlineStyle );

	if( style.empty() )
		style.push_back( Cfg::c_normalStyle );

	c.set_style( style );
	c.set_fontSize( d->m_ui.m_fontSize->value() );
	c.set_text( d->m_ui.m_text->text() );

	res.set_text( c );

	res.set_isChecked( d->m_ui.m_isChecked->isChecked() );

	return res;
}

void
CheckBoxProperties::setCfg( const Cfg::CheckBox & c )
{
	if( std::find( c.text().style().cbegin(), c.text().style().cend(),
		Cfg::c_boldStyle ) != c.text().style().cend() )
			d->m_ui.m_bold->setChecked( true );
	else
		d->m_ui.m_bold->setChecked( false );

	if( std::find( c.text().style().cbegin(), c.text().style().cend(),
		Cfg::c_italicStyle ) != c.text().style().cend() )
			d->m_ui.m_italic->setChecked( true );
	else
		d->m_ui.m_italic->setChecked( false );

	if( std::find( c.text().style().cbegin(), c.text().style().cend(),
		Cfg::c_underlineStyle ) != c.text().style().cend() )
			d->m_ui.m_underline->setChecked( true );
	else
		d->m_ui.m_underline->setChecked( false );

	d->m_ui.m_fontSize->setValue( c.text().fontSize() );

	d->m_ui.m_text->setText( c.text().text() );

	d->m_ui.m_isChecked->setChecked( c.isChecked() );
}

} /* namespace Core */

} /* namespace Prototyper */
