
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
#include "form_properties.hpp"
#include "ui_form_properties.h"


namespace Prototyper {

namespace Core {

//
// FormPropertiesPrivate
//

class FormPropertiesPrivate {
public:
	explicit FormPropertiesPrivate( FormProperties * parent )
		:	q( parent )
	{
	}

	//! Init.
	void init();

	//! Parent.
	FormProperties * q;
	//! Ui.
	Ui::FormProperties m_ui;
}; // class FormPropertiesPrivate

void
FormPropertiesPrivate::init()
{
	m_ui.setupUi( q );
}


//
// FormProperties
//

FormProperties::FormProperties( QWidget * parent, Qt::WindowFlags f )
	:	QDialog( parent, f )
	,	d( new FormPropertiesPrivate( this ) )
{
	d->init();
}

FormProperties::~FormProperties()
{
}

FormProperties::Buttons
FormProperties::buttons() const
{
	Buttons btns;

	if( d->m_ui.m_minimize->isChecked() )
		btns |= MinimizeButton;

	if( d->m_ui.m_maximize->isChecked() )
		btns |= MaximizeButton;

	if( d->m_ui.m_close->isChecked() )
		btns |= CloseButton;

	return btns;
}

void
FormProperties::setButtons( Buttons b )
{
	if( b.testFlag( MinimizeButton ) )
		d->m_ui.m_minimize->setChecked( true );
	else
		d->m_ui.m_minimize->setChecked( false );

	if( b.testFlag( MaximizeButton ) )
		d->m_ui.m_maximize->setChecked( true );
	else
		d->m_ui.m_maximize->setChecked( false );

	if( b.testFlag( CloseButton ) )
		d->m_ui.m_close->setChecked( true );
	else
		d->m_ui.m_close->setChecked( false );
}

} /* namespace Core */

} /* namespace Prototyper */
