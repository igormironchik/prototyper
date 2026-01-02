
/*
    SPDX-FileCopyrightText: 2026 Igor Mironchik <igor.mironchik@gmail.com>
    SPDX-License-Identifier: GPL-3.0-or-later
*/

// Prototyper include.
#include "grid_step_dlg.hpp"
#include "ui_grid_step_dlg.h"

//! Qt include.
#include <QCheckBox>
#include <QSpinBox>

namespace Prototyper
{

namespace Core
{

//
// GridStepDlgPrivate
//

class GridStepDlgPrivate
{
public:
    explicit GridStepDlgPrivate(GridStepDlg *parent)
        : q(parent)
    {
    }

    //! Init.
    void init(int step,
              bool forAll);

    //! Parent.
    GridStepDlg *q;
    //! Ui.
    Ui::GridStepDlg m_ui;
}; // class GridStepDlgPrivate

void GridStepDlgPrivate::init(int step,
                              bool forAll)
{
    m_ui.setupUi(q);

    m_ui.m_step->setValue(step);
    m_ui.m_box->setChecked(forAll);
}

//
// GridStepDlg
//

GridStepDlg::GridStepDlg(int step,
                         bool forAll,
                         QWidget *parent,
                         Qt::WindowFlags f)
    : QDialog(parent,
              f)
    , d(new GridStepDlgPrivate(this))
{
    d->init(step, forAll);
}

GridStepDlg::~GridStepDlg() = default;

int GridStepDlg::gridStep() const
{
    return d->m_ui.m_step->value();
}

bool GridStepDlg::applyForAllForms() const
{
    return d->m_ui.m_box->isChecked();
}

} /* namespace Core */

} /* namespace Prototyper */
