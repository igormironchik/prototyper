
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
	explicit SpinBoxPropertiesPrivate( SpinBoxProperties * parent )
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

SpinBoxProperties::SpinBoxProperties( QWidget * parent )
	:	QWidget( parent )
	,	d( new SpinBoxPropertiesPrivate( this ) )
{
	d->init();
}

SpinBoxProperties::~SpinBoxProperties()
{
}

Ui::SpinBoxProperties *
SpinBoxProperties::ui() const
{
	return &d->m_ui;
}

} /* namespace Core */

} /* namespace Prototyper */
