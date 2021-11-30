
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

#ifndef PROTOTYPER__CORE__FORM_CHECKBOX_PROPERTIES_HPP__INCLUDED
#define PROTOTYPER__CORE__FORM_CHECKBOX_PROPERTIES_HPP__INCLUDED

// Qt include.
#include <QWidget>

// Prototyper include.
#include "project_cfg.hpp"
#include "ui_form_checkbox_properties.h"
#include "form_undo_commands.hpp"


namespace Prototyper {

namespace Core {

//
// FormCheckBoxProperties
//

//! Properties of the checkbox on the form.
class CheckBoxProperties final
	:	public QWidget
{
	Q_OBJECT

public:
	CheckBoxProperties( QWidget * parent = Q_NULLPTR );
	~CheckBoxProperties() override;

	Ui::CheckBoxProperties * ui();

	//! Connect properties signals/slots.
	template< typename T >
	void connectProperties( T * owner )
	{
		connect( m_ui.m_checked,
			&QCheckBox::stateChanged, owner->q,
			[this, owner]( int v ) {
				owner->m_checked = ( v == Qt::Checked );

				owner->q->page()->undoStack()->push( new UndoChangeCheckState( owner->q->page(),
					owner->q->objectId() ) );

				owner->q->update();
			} );
	}

	//! Disconnect properties signals/slots.
	void disconnectProperties();

private:
	Q_DISABLE_COPY( CheckBoxProperties )

	//! Ui.
	Ui::CheckBoxProperties m_ui;
}; // class ButtonProperties

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__FORM_CHECKBOX_PROPERTIES_HPP__INCLUDED
