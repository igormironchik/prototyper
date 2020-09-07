
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

#ifndef PROTOTYPER__CORE__FORM_TEXT_STYLE_PROPERTIES_HPP__INCLUDED
#define PROTOTYPER__CORE__FORM_TEXT_STYLE_PROPERTIES_HPP__INCLUDED

// Qt include.
#include <QWidget>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>

// Prototyper include.
#include "project_cfg.hpp"
#include "ui_form_text_style_properties.h"
#include "form_undo_commands.hpp"
#include "utils.hpp"


namespace Prototyper {

namespace Core {

//
// FormTextStyleProperties
//

//! Properties of the Text on the form.
class TextStyleProperties
	:	public QWidget
{
	Q_OBJECT

public:
	TextStyleProperties( QWidget * parent = Q_NULLPTR );
	~TextStyleProperties();

	Ui::TextStyleProperties * ui();

	//! Connect properties signals/slots.
	template< typename T >
	void connectProperties( T * owner )
	{
		connect( m_ui.m_size,
			QOverload< int >::of( &QSpinBox::valueChanged ), owner->q,
			[this, owner]( int v ) {
				const auto oldText = owner->q->text();
				owner->m_font.setPixelSize( MmPx::instance().fromPtY( v ) );
				owner->q->page()->emitChanged();

				owner->q->page()->undoStack()->push( new UndoChangeTextWithOpts( owner->q->page(),
					owner->q->objectId(), oldText, owner->q->text() ) );

				owner->q->update();
			} );

		connect( m_ui.m_bold,
			&QCheckBox::stateChanged, owner->q,
			[this, owner]( int v ) {
				const auto oldText = owner->q->text();
				owner->m_font.setWeight( ( v == Qt::Checked ? QFont::Bold : QFont::Normal ) );
				owner->q->page()->emitChanged();

				owner->q->page()->undoStack()->push( new UndoChangeTextWithOpts( owner->q->page(),
					owner->q->objectId(), oldText, owner->q->text() ) );

				owner->q->update();
			} );

		connect( m_ui.m_italic,
			&QCheckBox::stateChanged, owner->q,
			[this, owner]( int v ) {
				const auto oldText = owner->q->text();
				owner->m_font.setItalic( ( v == Qt::Checked ) );
				owner->q->page()->emitChanged();

				owner->q->page()->undoStack()->push( new UndoChangeTextWithOpts( owner->q->page(),
					owner->q->objectId(), oldText, owner->q->text() ) );

				owner->q->update();
			} );

		connect( m_ui.m_underline,
			&QCheckBox::stateChanged, owner->q,
			[this, owner]( int v ) {
				const auto oldText = owner->q->text();
				owner->m_font.setUnderline( ( v == Qt::Checked ) );
				owner->q->page()->emitChanged();

				owner->q->page()->undoStack()->push( new UndoChangeTextWithOpts( owner->q->page(),
					owner->q->objectId(), oldText, owner->q->text() ) );

				owner->q->update();
			} );
	}

	//! Disconnect properties signals/slots.
	void disconnectProperties();

private:
	Q_DISABLE_COPY( TextStyleProperties )

	//! Ui.
	Ui::TextStyleProperties m_ui;
}; // class TextStyleProperties

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__FORM_TEXT_STYLE_PROPERTIES_HPP__INCLUDED
