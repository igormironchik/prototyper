
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
		if( c.position() != c.selectionEnd() )
			c.setPosition( c.selectionEnd() );

		QTextCharFormat fmt = c.charFormat();

		qreal s = fmt.fontPointSize();
		s -= 1.0;

		if( s < 5.0 )
			s = 5.0;

		fmt.setFontPointSize( s );

		textCursor().setCharFormat( fmt );
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
		if( c.position() != c.selectionEnd() )
			c.setPosition( c.selectionEnd() );

		QTextCharFormat fmt = c.charFormat();

		qreal s = fmt.fontPointSize();
		s += 1.0;
		fmt.setFontPointSize( s );

		textCursor().setCharFormat( fmt );
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
		if( c.position() != c.selectionEnd() )
			c.setPosition( c.selectionEnd() );

		QTextCharFormat fmt = c.charFormat();

		fmt.setFontWeight( on ? QFont::Bold : QFont::Normal );

		textCursor().setCharFormat( fmt );
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
		if( c.position() != c.selectionEnd() )
			c.setPosition( c.selectionEnd() );

		QTextCharFormat fmt = c.charFormat();

		fmt.setFontItalic( on );

		textCursor().setCharFormat( fmt );
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
		if( c.position() != c.selectionEnd() )
			c.setPosition( c.selectionEnd() );

		QTextCharFormat fmt = c.charFormat();

		fmt.setFontUnderline( on );

		textCursor().setCharFormat( fmt );
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

void
TextEditor::clearFormat()
{
	QTextCursor c = textCursor();

	if( c.hasSelection() )
	{
		QTextCharFormat fmt = c.charFormat();

		fmt.setFontUnderline( false );
		fmt.setFontItalic( false );
		fmt.setFontWeight( QFont::Normal );
		fmt.setFontPointSize( 10 );

		textCursor().setCharFormat( fmt );
	}
	else
	{
		setFontUnderline( false );
		setFontItalic( false );
		setFontWeight( QFont::Normal );
		setFontPointSize( 10 );
	}
}

} /* namespace Core */

} /* namespace Prototyper */
