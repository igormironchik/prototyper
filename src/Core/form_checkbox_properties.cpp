
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
#include "form_checkbox_properties.hpp"
#include "utils.hpp"

// Qt include.
#include <QLineEdit>
#include <QSpinBox>
#include <QCheckBox>


namespace Prototyper {

namespace Core {

//
// FormCheckBoxProperties
//

CheckBoxProperties::CheckBoxProperties( QWidget * parent )
	:	QWidget( parent )
{
	m_ui.setupUi( this );
}

CheckBoxProperties::~CheckBoxProperties() = default;

Ui::CheckBoxProperties *
CheckBoxProperties::ui()
{
	return &m_ui;
}

void
CheckBoxProperties::disconnectProperties()
{
	disconnect( m_ui.m_checked,
		&QCheckBox::stateChanged, nullptr, nullptr );
}

} /* namespace Core */

} /* namespace Prototyper */
