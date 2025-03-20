
/*
	SPDX-FileCopyrightText: 2016-2024 Igor Mironchik <igor.mironchik@gmail.com>
	SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef PROTOTYPER__CORE__FORM_CHECKBOX_PROPERTIES_HPP__INCLUDED
#define PROTOTYPER__CORE__FORM_CHECKBOX_PROPERTIES_HPP__INCLUDED

// Qt include.
#include <QWidget>

// Prototyper include.
#include "project_cfg.hpp"
#include "ui_checkbox_properties.h"
#include "../undo_commands.hpp"


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
			&QCheckBox::checkStateChanged, owner->q,
			[this, owner]( Qt::CheckState v ) {
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
