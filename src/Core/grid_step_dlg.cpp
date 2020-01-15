
/*!
	\file

	\author Igor Mironchik (igor.mironchik at gmail dot com).

	Copyright (c) 2016-2020 Igor Mironchik

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
#include "grid_step_dlg.hpp"
#include "ui_grid_step_dlg.h"

//! Qt include.
#include <QSpinBox>
#include <QCheckBox>


namespace Prototyper {

namespace Core {

//
// GridStepDlgPrivate
//

class GridStepDlgPrivate {
public:
	explicit GridStepDlgPrivate( GridStepDlg * parent )
		:	q( parent )
	{
	}

	//! Init.
	void init( int step, bool forAll );

	//! Parent.
	GridStepDlg * q;
	//! Ui.
	Ui::GridStepDlg m_ui;
}; // class GridStepDlgPrivate

void
GridStepDlgPrivate::init( int step, bool forAll )
{
	m_ui.setupUi( q );

	m_ui.m_step->setValue( step );
	m_ui.m_box->setChecked( forAll );
}


//
// GridStepDlg
//

GridStepDlg::GridStepDlg( int step, bool forAll,
	QWidget * parent, Qt::WindowFlags f )
	:	QDialog( parent, f )
	,	d( new GridStepDlgPrivate( this ) )
{
	d->init( step, forAll );
}

GridStepDlg::~GridStepDlg() = default;

int
GridStepDlg::gridStep() const
{
	return d->m_ui.m_step->value();
}

bool
GridStepDlg::applyForAllForms() const
{
	return d->m_ui.m_box->isChecked();
}

} /* namespace Core */

} /* namespace Prototyper */
