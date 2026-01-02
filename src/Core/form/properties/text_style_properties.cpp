
/*
    SPDX-FileCopyrightText: 2026 Igor Mironchik <igor.mironchik@gmail.com>
    SPDX-License-Identifier: GPL-3.0-or-later
*/

// Prototyper include.
#include "text_style_properties.hpp"
#include "../utils.hpp"

// Qt include.
#include <QCheckBox>
#include <QLineEdit>
#include <QSpinBox>

namespace Prototyper
{

namespace Core
{

//
// FormTextStyleProperties
//

TextStyleProperties::TextStyleProperties(QWidget *parent)
    : QWidget(parent)
{
    m_ui.setupUi(this);
}

TextStyleProperties::~TextStyleProperties() = default;

Ui::TextStyleProperties *TextStyleProperties::ui()
{
    return &m_ui;
}

void TextStyleProperties::disconnectProperties()
{
    FormCheckBox::disconnect(m_ui.m_size, QOverload<int>::of(&QSpinBox::valueChanged), nullptr, nullptr);

    FormCheckBox::disconnect(m_ui.m_bold, &QCheckBox::checkStateChanged, nullptr, nullptr);

    FormCheckBox::disconnect(m_ui.m_italic, &QCheckBox::checkStateChanged, nullptr, nullptr);

    FormCheckBox::disconnect(m_ui.m_underline, &QCheckBox::checkStateChanged, nullptr, nullptr);
}

} /* namespace Core */

} /* namespace Prototyper */
