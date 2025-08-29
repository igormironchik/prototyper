
/*
    SPDX-FileCopyrightText: 2025 Igor Mironchik <igor.mironchik@gmail.com>
    SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef PROTOTYPER__CORE__GRID_STEP_DLG_HPP__INCLUDED
#define PROTOTYPER__CORE__GRID_STEP_DLG_HPP__INCLUDED

// Qt include.
#include <QDialog>

// C++ include.
#include <memory>

namespace Prototyper
{

namespace Core
{

//
// GridStepDlg
//

class GridStepDlgPrivate;

//! Grid step dialog.
class GridStepDlg final : public QDialog
{
    Q_OBJECT

public:
    GridStepDlg(int step,
                bool forAll,
                QWidget *parent = 0,
                Qt::WindowFlags f = Qt::WindowFlags());
    ~GridStepDlg() override;

    //! \return Grid step.
    int gridStep() const;

    //! \return Apply for all forms?
    bool applyForAllForms() const;

private:
    Q_DISABLE_COPY(GridStepDlg)

    std::unique_ptr<GridStepDlgPrivate> d;
}; // class GridStepDlg

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__GRID_STEP_DLG_HPP__INCLUDED
