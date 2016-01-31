
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

TextEditor::~TextEditor()
{
}

bool operator != ( const QTextCharFormat & f1, const QTextCharFormat & f2 )
{
	return ( f1.fontPointSize() != f2.fontPointSize() ||
		f1.fontWeight() != f2.fontWeight() ||
		f1.fontItalic() != f2.fontItalic() ||
		f1.fontUnderline() != f2.fontUnderline() );
}

static const QString c_boldStyle = QLatin1String( "bold" );
static const QString c_italicStyle = QLatin1String( "italic" );
static const QString c_underlineStyle = QLatin1String( "underline" );
static const QString c_normalStyle = QLatin1String( "normal" );

static inline QList< QString > textStyle( const QTextCharFormat & f )
{
	QList< QString > res;

	if( f.fontWeight() == QFont::Bold )
		res.append( c_boldStyle );

	if( f.fontItalic() )
		res.append( c_italicStyle );

	if( f.fontUnderline() )
		res.append( c_underlineStyle );

	if( res.isEmpty() )
		res.append( c_normalStyle );

	return res;
}

Cfg::ProjectDesc
TextEditor::text() const
{
	QList< Cfg::TextStyle > blocks;

	Cfg::ProjectDesc desc;
	desc.setText( blocks );

	QTextCursor c = textCursor();

	int pos = 0;

	c.setPosition( pos );

	QTextCharFormat f = c.charFormat();

	QString t;

	const QString data = toPlainText();

	while( c.movePosition( QTextCursor::NextCharacter ) )
	{
		if( f != c.charFormat() )
		{
			Cfg::TextStyle style;
			style.setStyle( textStyle( f ) );
			style.setFontSize( f.fontPointSize() );
			style.setText( t );

			blocks.append( style );

			f = c.charFormat();

			t = data.at( pos );
		}
		else
			t.append( data.at( pos ) );

		++pos;
	};

	if( !t.isEmpty() )
	{
		Cfg::TextStyle style;
		style.setStyle( textStyle( f ) );
		style.setFontSize( f.fontPointSize() );
		style.setText( t );

		blocks.append( style );
	}

	return desc;
}

void
TextEditor::setText( const Cfg::ProjectDesc & c )
{
	reset();

	foreach( const Cfg::TextStyle & s, c.text() )
	{
		if( s.style().contains( c_normalStyle ) )
		{
			setFontWeight( QFont::Normal );
			setFontItalic( false );
			setFontUnderline( false );
		}
		else
		{
			if( s.style().contains( c_boldStyle ) )
				setFontWeight( QFont::Bold );
			else
				setFontWeight( QFont::Normal );

			if( s.style().contains( c_italicStyle ) )
				setFontItalic( true );
			else
				setFontItalic( false );

			if( s.style().contains( c_underlineStyle ) )
				setFontUnderline( true );
			else
				setFontUnderline( false );
		}

		setFontPointSize( s.fontSize() );

		insertPlainText( s.text() );

		QTextCursor cursor = textCursor();
		cursor.movePosition( QTextCursor::End );
		setTextCursor( cursor );
	}
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

	emit changed();
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

	emit changed();
}

void
TextEditor::reset()
{
	clear();

	clearFormat();
}

} /* namespace Core */

} /* namespace Prototyper */
