
/*!
	\file

	\author Igor Mironchik (igor.mironchik at gmail dot com).

	Copyright (c) 2016 Igor Mironchik

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
#include "text_editor.hpp"

#include <QTextCursor>
#include <QTextDocumentFragment>
#include <QTextDocument>


namespace Prototyper {

namespace Core {

//
// TextEditorPrivate
//

class TextEditorPrivate {
public:
	TextEditorPrivate( TextEditor * parent )
		:	q( parent )
	{
	}

	//! Init.
	void init();

	//! Parent.
	TextEditor * q;
}; // class TextEditorPrivate;

void
TextEditorPrivate::init()
{
	q->setWordWrapMode( QTextOption::WordWrap );
	q->setUndoRedoEnabled( true );

	q->setFontPointSize( 10.0 );

	TextEditor::connect( q, &QTextEdit::cursorPositionChanged,
		q, &TextEditor::slotCursorChanged );
}


//
// TextEditor
//

TextEditor::TextEditor( QWidget * parent )
	:	QTextEdit( parent )
	,	d( new TextEditorPrivate( this ) )
{
	d->init();
}

TextEditor::~TextEditor()
{
}

void
TextEditor::lessFontSize()
{
	QTextCursor c = textCursor();

	if( c.hasSelection() )
	{
		bool changed = false;

		if( c.position() == c.selectionStart() )
		{
			const int end = c.selectionEnd();

			c.clearSelection();

			c.setPosition( end, QTextCursor::KeepAnchor );

			setTextCursor( c );

			changed = true;
		}

		QTextCharFormat fmt = textCursor().charFormat();

		qreal s = fmt.fontPointSize();
		s -= 1.0;

		if( s < 5.0 )
			s = 5.0;

		fmt.setFontPointSize( s );

		textCursor().setCharFormat( fmt );

		if( changed )
		{
			const int start = c.selectionStart();

			c.clearSelection();

			c.setPosition( start, QTextCursor::KeepAnchor );

			setTextCursor( c );
		}
	}
	else
	{
		qreal s = fontPointSize();
		s -= 1.0;

		if( s < 5.0 )
			s = 5.0;

		setFontPointSize( s );
	}
}

void
TextEditor::moreFontSize()
{
	QTextCursor c = textCursor();

	if( c.hasSelection() )
	{
		bool changed = false;

		if( c.position() == c.selectionStart() )
		{
			const int end = c.selectionEnd();

			c.clearSelection();

			c.setPosition( end, QTextCursor::KeepAnchor );

			setTextCursor( c );

			changed = true;
		}

		QTextCharFormat fmt = textCursor().charFormat();

		qreal s = fmt.fontPointSize();
		s += 1.0;
		fmt.setFontPointSize( s );

		textCursor().setCharFormat( fmt );

		if( changed )
		{
			c = textCursor();

			const int start = c.selectionStart();

			c.clearSelection();

			c.setPosition( start, QTextCursor::KeepAnchor );

			setTextCursor( c );
		}
	}
	else
	{
		qreal s = fontPointSize();
		s += 1.0;
		setFontPointSize( s );
	}
}

void
TextEditor::bold( bool on )
{
	QTextCursor c = textCursor();

	if( c.hasSelection() )
	{
		bool changed = false;

		if( c.position() == c.selectionStart() )
		{
			const int end = c.selectionEnd();

			c.clearSelection();

			c.setPosition( end, QTextCursor::KeepAnchor );

			setTextCursor( c );

			changed = true;
		}

		QTextCharFormat fmt = textCursor().charFormat();
		fmt.setFontWeight( on ? QFont::Bold : QFont::Normal );

		textCursor().setCharFormat( fmt );

		if( changed )
		{
			c = textCursor();

			const int start = c.selectionStart();

			c.clearSelection();

			c.setPosition( start, QTextCursor::KeepAnchor );

			setTextCursor( c );
		}
	}
	else
		setFontWeight( on ? QFont::Bold : QFont::Normal );
}

void
TextEditor::italic( bool on )
{
	QTextCursor c = textCursor();

	if( c.hasSelection() )
	{
		bool changed = false;

		if( c.position() == c.selectionStart() )
		{
			const int end = c.selectionEnd();

			c.clearSelection();

			c.setPosition( end, QTextCursor::KeepAnchor );

			setTextCursor( c );

			changed = true;
		}

		QTextCharFormat fmt = textCursor().charFormat();
		fmt.setFontItalic( on );

		textCursor().setCharFormat( fmt );

		if( changed )
		{
			c = textCursor();

			const int start = c.selectionStart();

			c.clearSelection();

			c.setPosition( start, QTextCursor::KeepAnchor );

			setTextCursor( c );
		}
	}
	else
		setFontItalic( on );
}

void
TextEditor::underline( bool on )
{
	QTextCursor c = textCursor();

	if( c.hasSelection() )
	{
		bool changed = false;

		if( c.position() == c.selectionStart() )
		{
			const int end = c.selectionEnd();

			c.clearSelection();

			c.setPosition( end, QTextCursor::KeepAnchor );

			setTextCursor( c );

			changed = true;
		}

		QTextCharFormat fmt = textCursor().charFormat();
		fmt.setFontUnderline( on );

		textCursor().setCharFormat( fmt );

		if( changed )
		{
			c = textCursor();

			const int start = c.selectionStart();

			c.clearSelection();

			c.setPosition( start, QTextCursor::KeepAnchor );

			setTextCursor( c );
		}
	}
	else
		setFontUnderline( on );
}

void
TextEditor::changeTextColor()
{

}

void
TextEditor::slotCursorChanged()
{
	emit cursorChanged( textCursor() );
}

} /* namespace Core */

} /* namespace Prototyper */
