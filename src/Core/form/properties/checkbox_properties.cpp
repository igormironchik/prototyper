
/*
    SPDX-FileCopyrightText: 2025 Igor Mironchik <igor.mironchik@gmail.com>
    SPDX-License-Identifier: GPL-3.0-or-later
*/

// Prototyper include.
#include "checkbox_properties.hpp"
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
// FormCheckBoxProperties
//

CheckBoxProperties::CheckBoxProperties(QWidget *parent)
    : QWidget(parent)
{
    m_ui.setupUi(this);
}

CheckBoxProperties::~CheckBoxProperties() = default;

Ui::CheckBoxProperties *CheckBoxProperties::ui()
{
    return &m_ui;
}

void CheckBoxProperties::disconnectProperties()
{
    disconnect(m_ui.m_checked, &QCheckBox::checkStateChanged, nullptr, nullptr);
}

} /* namespace Core */

} /* namespace Prototyper */
