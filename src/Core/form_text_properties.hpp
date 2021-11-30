
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

#ifndef PROTOTYPER__CORE__FORM_TEXT_PROPERTIES_HPP__INCLUDED
#define PROTOTYPER__CORE__FORM_TEXT_PROPERTIES_HPP__INCLUDED

// Qt include.
#include <QWidget>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>

// Prototyper include.
#include "project_cfg.hpp"
#include "ui_form_text_properties.h"
#include "form_undo_commands.hpp"
#include "utils.hpp"


namespace Prototyper {

namespace Core {

//
// FormTextProperties
//

//! Properties of the Text on the form.
class TextProperties final
	:	public QWidget
{
	Q_OBJECT

public:
	TextProperties( QWidget * parent = Q_NULLPTR );
	~TextProperties() override;

	Ui::TextProperties * ui();

	//! Connect properties signals/slots.
	template< typename T >
	void connectProperties( T * owner )
	{
		connect( m_ui.m_text,
			&QLineEdit::textChanged, owner->q,
			[this, owner]( const QString & t ) {
				const auto oldText = owner->q->text();
				owner->m_text = t;
				owner->q->page()->emitChanged();

				owner->q->page()->undoStack()->push( new UndoChangeTextWithOpts( owner->q->page(),
					owner->q->objectId(), oldText, owner->q->text() ) );

				owner->q->update();
			} );
	}

	//! Disconnect properties signals/slots.
	void disconnectProperties();

private:
	Q_DISABLE_COPY( TextProperties )

	//! Ui.
	Ui::TextProperties m_ui;
}; // class TextProperties

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__FORM_TEXT_PROPERTIES_HPP__INCLUDED
