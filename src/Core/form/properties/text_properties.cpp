
/*
	SPDX-FileCopyrightText: 2016-2024 Igor Mironchik <igor.mironchik@gmail.com>
	SPDX-License-Identifier: GPL-3.0-or-later
*/

// Prototyper include.
#include "text_properties.hpp"
#include "ui_text_properties.h"
#include "../utils.hpp"

// Qt include.
#include <QLineEdit>
#include <QSpinBox>
#include <QCheckBox>


namespace Prototyper {

namespace Core {

//
// FormTextProperties
//

TextProperties::TextProperties( QWidget * parent )
	:	QWidget( parent )
{
	m_ui.setupUi( this );
}

TextProperties::~TextProperties() = default;

Ui::TextProperties *
TextProperties::ui()
{
	return &m_ui;
}

void
TextProperties::disconnectProperties()
{
	FormCheckBox::disconnect( m_ui.m_text,
		&QLineEdit::textChanged, nullptr, nullptr );
}

} /* namespace Core */

} /* namespace Prototyper */
