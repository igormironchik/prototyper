
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
#include "text_editor.hpp"
#include "utils.hpp"
#include "constants.hpp"

#include <QTextCursor>
#include <QTextDocumentFragment>
#include <QTextDocument>
#include <QTextBlockFormat>


namespace Prototyper {

namespace Core {

//
// TextEditorPrivate
//

class TextEditorPrivate {
public:
	explicit TextEditorPrivate( TextEditor * parent )
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
	q->setUndoRedoEnabled( true );

	{
		QFont f = q->font();
		f.setPixelSize( MmPx::instance().fromPtY( c_defaultFontSize ) );
		q->setFont( f );
	}

	QTextCursor c = q->textCursor();

	QTextCharFormat fmt = c.charFormat();
	QFont f = fmt.font();
	f.setPixelSize( MmPx::instance().fromPtY( c_defaultFontSize ) );
	fmt.setFont( f );

	c.setCharFormat( fmt );

	q->setTextCursor( c );

	q->setAutoFormatting( QTextEdit::AutoNone );
	q->setTextInteractionFlags( Qt::TextEditorInteraction | Qt::TextBrowserInteraction );

	TextEditor::connect( q, &QTextEdit::cursorPositionChanged,
		q, &TextEditor::slotCursorChanged );
	TextEditor::connect( q, &QTextEdit::textChanged,
		q, &TextEditor::changed );
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

TextEditor::~TextEditor() = default;

std::vector< Cfg::TextStyle >
TextEditor::text() const
{
	return Cfg::text( textCursor(), toPlainText() );
}

void
TextEditor::setText( const std::vector< Cfg::TextStyle > & blocks )
{
	disconnect( this, &QTextEdit::textChanged, this, Q_NULLPTR );

	auto * doc = new QTextDocument( this );
	Cfg::fillTextDocument( doc, blocks );

	setDocument( doc );

	document()->setTextWidth( width() );
	document()->clearUndoRedoStacks();

	connect( this, &QTextEdit::textChanged, this, &TextEditor::changed );
}

void
TextEditor::setFontSize( int s )
{
	QTextCursor c = textCursor();

	if( c.hasSelection() )
	{
		if( c.position() != c.selectionEnd() )
			c.setPosition( c.selectionEnd() );

		QTextCharFormat fmt = c.charFormat();

		QFont f = fmt.font();
		f.setPixelSize( MmPx::instance().fromPtY( s ) );
		fmt.setFont( f );

		textCursor().setCharFormat( fmt );
	}
	else
	{
		QFont f = currentFont();
		f.setPixelSize( MmPx::instance().fromPtY( s ) );
		setCurrentFont( f );
	}

	emit changed();
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

	emit changed();
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

	emit changed();
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

	emit changed();
}

void
TextEditor::changeTextColor()
{
	emit changed();
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
		fmt.clearBackground();
		fmt.clearForeground();
		QFont f = fmt.font();
		f.setPixelSize( MmPx::instance().fromPtY( c_defaultFontSize ) );
		fmt.setFont( f );
		fmt.setAnchor( false );
		fmt.setAnchorHref( QString() );
		fmt.setForeground( QBrush( QColor( Qt::black ) ) );

		textCursor().setCharFormat( fmt );
	}
	else
	{
		setFontUnderline( false );
		setFontItalic( false );
		setFontWeight( QFont::Normal );
		setTextColor( Qt::black );
		QFont f = currentFont();
		f.setPixelSize( MmPx::instance().fromPtY( c_defaultFontSize ) );
		setCurrentFont( f );
		QTextCursor cursor = textCursor();
		QTextCharFormat fmt = cursor.charFormat();
		fmt.setAnchor( false );
		fmt.setAnchorHref( QString() );
		fmt.setForeground( QBrush( QColor( Qt::black ) ) );
		cursor.setCharFormat( fmt );
		setTextCursor( cursor );
	}

	emit changed();
}

void
TextEditor::reset()
{
	clear();

	clearFormat();
}

void
TextEditor::alignLeft()
{
	QTextCursor c = textCursor();

	if( c.hasSelection() )
	{
		QTextBlockFormat b = c.blockFormat();
		b.setAlignment( Qt::AlignLeft );
		c.setBlockFormat( b );
		setTextCursor( c );
	}
	else
		setAlignment( Qt::AlignLeft );
}

void
TextEditor::alignCenter()
{
	QTextCursor c = textCursor();

	if( c.hasSelection() )
	{
		QTextBlockFormat b = c.blockFormat();
		b.setAlignment( Qt::AlignCenter );
		c.setBlockFormat( b );
		setTextCursor( c );
	}
	else
		setAlignment( Qt::AlignCenter );
}

void
TextEditor::alignRight()
{
	QTextCursor c = textCursor();

	if( c.hasSelection() )
	{
		QTextBlockFormat b = c.blockFormat();
		b.setAlignment( Qt::AlignRight );
		c.setBlockFormat( b );
		setTextCursor( c );
	}
	else
		setAlignment( Qt::AlignRight );
}

void
TextEditor::insertLink()
{
	QTextCursor c = textCursor();

	if( c.hasSelection() )
	{
		QTextCharFormat fmt = c.charFormat();
		fmt.setAnchor( true );
		fmt.setAnchorHref( c.selectedText() );
		fmt.setForeground( QBrush( QColor( Qt::blue ) ) );

		textCursor().setCharFormat( fmt );
	}
}

} /* namespace Core */

} /* namespace Prototyper */
