
/*
    SPDX-FileCopyrightText: 2016-2024 Igor Mironchik <igor.mironchik@gmail.com>
    SPDX-License-Identifier: GPL-3.0-or-later
*/

// Prototyper include.
#include "spinbox_properties.hpp"
#include "../utils.hpp"
#include "ui_spinbox_properties.h"

// Qt include.
#include <QCheckBox>
#include <QSpinBox>

namespace Prototyper
{

namespace Core
{

//
// SpinBoxProperties
//

SpinBoxProperties::SpinBoxProperties(QWidget *parent)
    : QWidget(parent)
{
    m_ui.setupUi(this);
}

SpinBoxProperties::~SpinBoxProperties() = default;

Ui::SpinBoxProperties *SpinBoxProperties::ui()
{
    return &m_ui;
}

void SpinBoxProperties::disconnectProperties()
{
    disconnect(m_ui.m_value, QOverload<int>::of(&QSpinBox::valueChanged), nullptr, nullptr);
}

} /* namespace Core */

} /* namespace Prototyper */
