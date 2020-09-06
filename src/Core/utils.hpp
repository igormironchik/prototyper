
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

#ifndef PROTOTYPER__CORE__UTILS_HPP__INCLUDED
#define PROTOTYPER__CORE__UTILS_HPP__INCLUDED

// Qt include.
#include <QtGlobal>
#include <QFont>
#include <QTextCursor>

// Prototyper include.
#include "project_cfg.hpp"
#include "constants.hpp"


QT_BEGIN_NAMESPACE
class QTextCharFormat;
class QTextCursor;
class QTextDocument;
class QTextBlockFormat;
class QGraphicsItem;
QT_END_NAMESPACE


namespace Prototyper {

namespace Core {

//
// minMaxZ
//

//! \return Min and max Z index.
QPair< qreal, qreal >
minMaxZ( const QList< QGraphicsItem* > & items );


//
// MmPx
//

class MmPx {
public:
	static const MmPx & instance();

	qreal toMmX( qreal px ) const;
	qreal fromMmX( qreal mm ) const;

	qreal toMmY( qreal px ) const;
	qreal fromMmY( qreal mm ) const;

	QSizeF a4() const;

	qreal fromMm( qreal mm, qreal dpi ) const;

	int fromPtY( qreal pt ) const;
	qreal toPtY( int px ) const;

	int fromPt( qreal pt, qreal dpi ) const;

	qreal yDpi() const;

private:
	MmPx();

	qreal m_xdots;
	qreal m_ydots;
}; // class MmPx

namespace Cfg {

static const QString c_boldStyle = QLatin1String( "bold" );
static const QString c_italicStyle = QLatin1String( "italic" );
static const QString c_underlineStyle = QLatin1String( "underline" );
static const QString c_normalStyle = QLatin1String( "normal" );
static const QString c_left = QLatin1String( "left" );
static const QString c_right = QLatin1String( "right" );
static const QString c_center = QLatin1String( "center" );
static const QString c_link = QLatin1String( "link" );

static const QString c_minimize = QLatin1String( "minimize" );
static const QString c_maximize = QLatin1String( "maximize" );
static const QString c_close = QLatin1String( "close" );


//
// textStyle
//

//! \return Text style for the configuration.
std::vector< QString > textStyle( const QTextCharFormat & f,
	const QTextBlockFormat & b );


//
// text
//

//! \return Text for the configuration.
std::vector< Cfg::TextStyle > text( QTextCursor c, const QString & data );


//
// pen
//

Cfg::Pen pen( const QPen & p );


//
// fromPen
//

QPen fromPen( const Cfg::Pen & p );
QPen fromPen( const Cfg::Pen & p, qreal dpi );


//
// brush
//

Cfg::Brush brush( const QBrush & b );


//
// fromBrush
//

QBrush fromBrush( const Cfg::Brush & b );


//
// initBlockFormat
//

void initBlockFormat( QTextBlockFormat & b,
	const Cfg::TextStyle & style );


//
// fillTextDocument
//

void fillTextDocument( QTextDocument * doc,
	const std::vector< Cfg::TextStyle > & text,
	qreal dpi = MmPx::instance().yDpi(),
	qreal scale = 1.0 );


//
// textStyleFromFont
//

TextStyle textStyleFromFont( const QFont & f );


//
// applyTextFormat
//

template< typename T >
void
applyTextFormat( const Cfg::TextStyle & s, T * editor )
{
	if( std::find( s.style().cbegin(), s.style().cend(),
			Cfg::c_normalStyle ) != s.style().cend() )
	{
		editor->setFontWeight( QFont::Normal );
		editor->setFontItalic( false );
		editor->setFontUnderline( false );
	}
	else
	{
		if( std::find( s.style().cbegin(), s.style().cend(),
			Cfg::c_boldStyle ) != s.style().cend() )
				editor->setFontWeight( QFont::Bold );
		else
			editor->setFontWeight( QFont::Normal );

		if( std::find( s.style().cbegin(), s.style().cend(),
			Cfg::c_italicStyle ) != s.style().cend() )
				editor->setFontItalic( true );
		else
			editor->setFontItalic( false );

		if( std::find( s.style().cbegin(), s.style().cend(),
			Cfg::c_underlineStyle ) != s.style().cend() )
				editor->setFontUnderline( true );
		else
			editor->setFontUnderline( false );
	}

	if( std::find( s.style().cbegin(), s.style().cend(),
		Cfg::c_left ) != s.style().cend() )
			editor->setAlignment( Qt::AlignLeft );
	else if( std::find( s.style().cbegin(), s.style().cend(),
		Cfg::c_right ) != s.style().cend() )
			editor->setAlignment( Qt::AlignRight );
	else if( std::find( s.style().cbegin(), s.style().cend(),
		Cfg::c_center ) != s.style().cend() )
			editor->setAlignment( Qt::AlignCenter );

	QFont f = editor->currentFont();
	f.setPixelSize( MmPx::instance().fromPtY( s.fontSize() ) );
	editor->setCurrentFont( f );

	QTextCursor cursor = editor->textCursor();
	QTextCharFormat fmt = cursor.charFormat();
	fmt.setAnchor( false );
	fmt.setAnchorHref( QString() );
	fmt.setForeground( QBrush( c_textColor ) );
	cursor.setCharFormat( fmt );
	editor->setTextCursor( cursor );
}

} /* namespace Cfg */

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__UTILS_HPP__INCLUDED
