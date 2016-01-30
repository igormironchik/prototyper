
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
#include "text_opts_bar.hpp"

// Qt include.
#include <QAction>
#include <QTextCursor>
#include <QTextCharFormat>


namespace Prototyper {

namespace Core {

//
// TextOptsBarPrivate
//

class TextOptsBarPrivate {
public:
	TextOptsBarPrivate( TextOptsBar::IconSize s, TextOptsBar * parent )
		:	q( parent )
		,	m_fontBold( 0 )
		,	m_fontItalic( 0 )
		,	m_fontUnderline( 0 )
		,	m_iconSize( s )
	{
	}

	//! Init.
	void init();

	//! Parent.
	TextOptsBar * q;
	//! Bold action.
	QAction * m_fontBold;
	//! Italic action.
	QAction * m_fontItalic;
	//! Underline action.
	QAction * m_fontUnderline;
	//! Icon size.
	TextOptsBar::IconSize m_iconSize;
}; // class TextOptsBarPrivate;

void
TextOptsBarPrivate::init()
{
	const QString fontLessTip = TextOptsBar::tr( "Less Size of the Font" );
	QAction * fontLess = q->addAction(
		( m_iconSize == TextOptsBar::Large ?
			QIcon( ":/Core/img/format-font-size-less.png" ) :
			QIcon( ":/Core/img/format-font-size-less-small.png" ) ),
		fontLessTip );
	fontLess->setShortcut( TextOptsBar::tr( "Ctrl+-" ) );
	fontLess->setToolTip( fontLessTip );
	fontLess->setStatusTip( fontLessTip );

	if( m_iconSize == TextOptsBar::Small )
		q->setIconSize( QSize( 16, 16 ) );

	const QString fontMoreTip = TextOptsBar::tr( "More Size of the Font" );
	QAction * fontMore = q->addAction(
		( m_iconSize == TextOptsBar::Large ?
			QIcon( ":/Core/img/format-font-size-more.png" ) :
			QIcon( ":/Core/img/format-font-size-more-small.png" ) ),
		fontMoreTip );
	fontMore->setShortcut( TextOptsBar::tr( "Ctrl+=" ) );
	fontMore->setToolTip( fontMoreTip );
	fontMore->setStatusTip( fontMoreTip );

	q->addSeparator();

	const QString fontBoldTip = TextOptsBar::tr( "Bold" );
	m_fontBold = q->addAction(
		( m_iconSize == TextOptsBar::Large ?
			QIcon( ":/Core/img/format-text-bold.png" ) :
			QIcon( ":/Core/img/format-text-bold-small.png" ) ),
		fontBoldTip );
	m_fontBold->setShortcut( TextOptsBar::tr( "Ctrl+B" ) );
	m_fontBold->setToolTip( fontBoldTip );
	m_fontBold->setStatusTip( fontBoldTip );
	m_fontBold->setCheckable( true );

	const QString fontItalicTip = TextOptsBar::tr( "Italic" );
	m_fontItalic = q->addAction(
		( m_iconSize == TextOptsBar::Large ?
			QIcon( ":/Core/img/format-text-italic.png" ) :
			QIcon( ":/Core/img/format-text-italic-small.png" ) ),
		fontItalicTip );
	m_fontItalic->setShortcut( TextOptsBar::tr( "Ctrl+I" ) );
	m_fontItalic->setToolTip( fontItalicTip );
	m_fontItalic->setStatusTip( fontItalicTip );
	m_fontItalic->setCheckable( true );

	const QString fontUnderlineTip = TextOptsBar::tr( "Underline" );
	m_fontUnderline = q->addAction(
		( m_iconSize == TextOptsBar::Large ?
			QIcon( ":/Core/img/format-text-underline.png" ) :
			QIcon( ":/Core/img/format-text-underline-small.png" ) ),
		fontUnderlineTip );
	m_fontUnderline->setShortcut( TextOptsBar::tr( "Ctrl+U" ) );
	m_fontUnderline->setToolTip( fontUnderlineTip );
	m_fontUnderline->setStatusTip( fontUnderlineTip );
	m_fontUnderline->setCheckable( true );

	q->addSeparator();

	const QString fontColorTip = TextOptsBar::tr( "Color of the Text" );
	QAction * fontColor = q->addAction(
		( m_iconSize == TextOptsBar::Large ?
			QIcon( ":/Core/img/format-text-color.png" ) :
			QIcon( ":/Core/img/format-text-color-small.png" ) ),
		fontColorTip );
	fontColor->setToolTip( fontColorTip );
	fontColor->setStatusTip( fontColorTip );

	q->addSeparator();

	const QString clearFormatTip = TextOptsBar::tr( "Clear Format" );
	QAction * clearFormat = q->addAction(
		( m_iconSize == TextOptsBar::Large ?
			QIcon( ":/Core/img/edit-clear.png" ) :
			QIcon( ":/Core/img/edit-clear-small.png" ) ),
		clearFormatTip );
	clearFormat->setToolTip( clearFormatTip );
	clearFormat->setStatusTip( clearFormatTip );

	TextOptsBar::connect( fontLess, &QAction::triggered,
		q, &TextOptsBar::lessFontSize );
	TextOptsBar::connect( fontMore, &QAction::triggered,
		q, &TextOptsBar::moreFontSize );
	TextOptsBar::connect( m_fontBold, &QAction::triggered,
		q, &TextOptsBar::bold );
	TextOptsBar::connect( m_fontItalic, &QAction::triggered,
		q, &TextOptsBar::italic );
	TextOptsBar::connect( m_fontUnderline, &QAction::triggered,
		q, &TextOptsBar::underline );
	TextOptsBar::connect( fontColor, &QAction::triggered,
		q, &TextOptsBar::textColor );
	TextOptsBar::connect( clearFormat, &QAction::triggered,
		q, &TextOptsBar::slotClearFormat );
}


//
// TextOptsBar
//

TextOptsBar::TextOptsBar( IconSize s, QWidget * parent )
	:	QToolBar( parent )
	,	d( new TextOptsBarPrivate( s, this ) )
{
	d->init();
}

TextOptsBar::~TextOptsBar()
{
}

void
TextOptsBar::updateState( const QTextCursor & cursor )
{
	QTextCharFormat fmt = cursor.charFormat();

	if( cursor.hasSelection() )
	{
		QTextCursor c = cursor;

		if( c.position() != c.selectionEnd() )
			c.setPosition( c.selectionEnd() );

		fmt = c.charFormat();
	}

	d->m_fontBold->setChecked( fmt.fontWeight() == QFont::Bold );
	d->m_fontItalic->setChecked( fmt.fontItalic() );
	d->m_fontUnderline->setChecked( fmt.fontUnderline() );
}

void
TextOptsBar::slotClearFormat()
{
	d->m_fontBold->setChecked( false );
	d->m_fontItalic->setChecked( false );
	d->m_fontUnderline->setChecked( false );

	emit clearFormat();
}

} /* namespace Core */

} /* namespace Prototyper */