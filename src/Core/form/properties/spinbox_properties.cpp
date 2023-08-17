
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
#include "spinbox_properties.hpp"
#include "ui_spinbox_properties.h"
#include "../utils.hpp"

// Qt include.
#include <QSpinBox>
#include <QCheckBox>


namespace Prototyper {

namespace Core {

//
// SpinBoxProperties
//

SpinBoxProperties::SpinBoxProperties( QWidget * parent )
	:	QWidget( parent )
{
	m_ui.setupUi( this );
}

SpinBoxProperties::~SpinBoxProperties() = default;

Ui::SpinBoxProperties *
SpinBoxProperties::ui()
{
	return &m_ui;
}

void
SpinBoxProperties::disconnectProperties()
{
	disconnect( m_ui.m_value,
		QOverload< int >::of( &QSpinBox::valueChanged ), nullptr, nullptr );
}

} /* namespace Core */

} /* namespace Prototyper */
