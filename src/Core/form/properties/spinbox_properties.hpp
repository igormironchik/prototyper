
/*
	SPDX-FileCopyrightText: 2016-2024 Igor Mironchik <igor.mironchik@gmail.com>
	SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef PROTOTYPER__CORE__FORM_SPINBOX_PROPERTIES_HPP__INCLUDED
#define PROTOTYPER__CORE__FORM_SPINBOX_PROPERTIES_HPP__INCLUDED

// Qt include.
#include <QWidget>

// Prototyper include.
#include "project_cfg.hpp"
#include "ui_spinbox_properties.h"
#include "../undo_commands.hpp"


namespace Prototyper {

namespace Core {

//
// SpinBoxProperties
//

//! Properties of the spinbox on the form.
class SpinBoxProperties final
	:	public QWidget
{
	Q_OBJECT

public:
	SpinBoxProperties( QWidget * parent = nullptr );
	~SpinBoxProperties() override;

	Ui::SpinBoxProperties * ui();

	//! Connect properties signals/slots.
	template< typename T >
	void connectProperties( T * owner )
	{
		connect( m_ui.m_value,
			QOverload< int >::of( &QSpinBox::valueChanged ), owner->q,
			[this, owner]( int v ) {
				const auto oldText = owner->q->text();
				owner->m_text = QString::number( v );
				owner->q->page()->emitChanged();

				owner->q->page()->undoStack()->push( new UndoChangeTextWithOpts( owner->q->page(),
					owner->q->objectId(), oldText, owner->q->text() ) );

				owner->q->update();
			} );
	}

	//! Disconnect properties signals/slots.
	void disconnectProperties();

private:
	Q_DISABLE_COPY( SpinBoxProperties )

	//! Ui.
	Ui::SpinBoxProperties m_ui;
}; // class SpinBoxProperties

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__FORM_SPINBOX_PROPERTIES_HPP__INCLUDED
