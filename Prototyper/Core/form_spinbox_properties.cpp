
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
#include "form_spinbox_properties.hpp"
#include "ui_form_spinbox_properties.h"
#include "utils.hpp"

// Qt include.
#include <QSpinBox>
#include <QCheckBox>


namespace Prototyper {

namespace Core {

//
// SpinBoxPropertiesPrivate
//

class SpinBoxPropertiesPrivate {
public:
	SpinBoxPropertiesPrivate( SpinBoxProperties * parent )
		:	q( parent )
	{
	}

	//! Init.
	void init();

	//! Parent.
	SpinBoxProperties * q;
	//! Ui.
	Ui::SpinBoxProperties m_ui;
}; // class SpinBoxPropertiesPrivate

void
SpinBoxPropertiesPrivate::init()
{
	m_ui.setupUi( q );
}


//
// SpinBoxProperties
//

SpinBoxProperties::SpinBoxProperties( QWidget * parent, Qt::WindowFlags f )
	:	QDialog( parent, f )
	,	d( new SpinBoxPropertiesPrivate( this ) )
{
	d->init();
}

SpinBoxProperties::~SpinBoxProperties()
{
}

Cfg::SpinBox
SpinBoxProperties::cfg() const
{
	Cfg::SpinBox s;

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
	c.setFontSize( d->m_ui.m_size->value() );
	c.setText( QString::number( d->m_ui.m_value->value() ) );

	s.setText( c );

	return s;
}

void
SpinBoxProperties::setCfg( const Cfg::SpinBox & s )
{
	if( s.text().style().contains( Cfg::c_boldStyle ) )
		d->m_ui.m_bold->setChecked( true );
	else
		d->m_ui.m_bold->setChecked( false );

	if( s.text().style().contains( Cfg::c_italicStyle ) )
		d->m_ui.m_italic->setChecked( true );
	else
		d->m_ui.m_italic->setChecked( false );

	if( s.text().style().contains( Cfg::c_underlineStyle ) )
		d->m_ui.m_underline->setChecked( true );
	else
		d->m_ui.m_underline->setChecked( false );

	d->m_ui.m_size->setValue( s.text().fontSize() );

	d->m_ui.m_value->setValue( s.text().text().toInt() );
}

} /* namespace Core */

} /* namespace Prototyper */
