
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
#include "utils.hpp"
#include "constants.hpp"

// Qt include.
#include <QTextCharFormat>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextBlockFormat>
#include <QApplication>
#include <QScreen>


namespace Prototyper {

namespace Core {

bool operator != ( const QTextCharFormat & f1, const QTextCharFormat & f2 )
{
	return ( f1.fontPointSize() != f2.fontPointSize() ||
		f1.fontWeight() != f2.fontWeight() ||
		f1.fontItalic() != f2.fontItalic() ||
		f1.fontUnderline() != f2.fontUnderline() );
}

bool operator != ( QTextBlockFormat & f1, const QTextBlockFormat & f2 )
{
	return ( f1.alignment() != f2.alignment() );
}


namespace Cfg {

//
// textStyle
//

std::vector< QString > textStyle( const QTextCharFormat & f,
	const QTextBlockFormat & b )
{
	std::vector< QString > res;

	if( f.fontWeight() == QFont::Bold )
		res.push_back( c_boldStyle );

	if( f.fontItalic() )
		res.push_back( c_italicStyle );

	if( f.fontUnderline() )
		res.push_back( c_underlineStyle );

	if( res.empty() )
		res.push_back( c_normalStyle );

	switch( b.alignment() )
	{
		case Qt::AlignCenter :
		case Qt::AlignHCenter :
		{
			res.push_back( c_center );
		}
			break;

		case Qt::AlignRight :
		{
			res.push_back( c_right );
		}
			break;

		default :
		{
			res.push_back( c_left );
		}
			break;
	}

	return res;
} // textStyle


//
// text
//

std::vector< Cfg::TextStyle > text( QTextCursor c, const QString & data )
{
	std::vector< Cfg::TextStyle > blocks;

	int pos = 0;

	c.setPosition( pos );

	QTextCharFormat f = c.charFormat();
	QTextBlockFormat b = c.blockFormat();

	QString t;

	while( c.movePosition( QTextCursor::NextCharacter ) )
	{
		if( f != c.charFormat() || b != c.blockFormat() )
		{
			Cfg::TextStyle style;
			style.set_style( textStyle( f, b ) );
			QFont font = f.font();
			style.set_fontSize( font.pixelSize() < MmPx::instance().fromPtY( 1.0 ) ?
				MmPx::instance().fromPtY( c_defaultFontSize ) : MmPx::instance().toPtY( font.pixelSize() ) );
			style.set_text( t );

			blocks.push_back( style );

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
		style.set_style( textStyle( f, b ) );
		QFont font = f.font();
		style.set_fontSize( font.pixelSize() < MmPx::instance().fromPtY( 1.0 ) ?
			MmPx::instance().fromPtY( c_defaultFontSize ) : MmPx::instance().toPtY( font.pixelSize() ) );
		style.set_text( t );

		blocks.push_back( style );
	}

	return blocks;
} // text


//
// pen
//

Cfg::Pen pen( const QPen & p )
{
	Cfg::Pen res;

	res.set_width( MmPx::instance().toMmX( p.widthF() ) );
	res.set_color( p.color().name( QColor::HexArgb ) );

	return res;
} // pen


//
// fromPen
//

QPen fromPen( const Cfg::Pen & p )
{
	return QPen( QColor( p.color() ), MmPx::instance().fromMmX( p.width() ), Qt::SolidLine );
} // fromPen

QPen fromPen( const Cfg::Pen & p, qreal dpi )
{
	return QPen( QColor( p.color() ), MmPx::instance().fromMm( p.width(), dpi ), Qt::SolidLine );
} // fromPen


//
// brush
//

Cfg::Brush brush( const QBrush & b )
{
	Cfg::Brush res;

	res.set_color( b.color().name( QColor::HexArgb ) );

	return res;
} // brush


//
// fromBrush
//

QBrush fromBrush( const Cfg::Brush & b )
{
	return QBrush( QColor( b.color() ) );
}


//
// initBlockFormat
//

void initBlockFormat( QTextBlockFormat & b,
	const Cfg::TextStyle & style )
{
	if( std::find( style.style().cbegin(), style.style().cend(),
		c_left ) != style.style().cend() )
			b.setAlignment( Qt::AlignLeft );
	else if( std::find( style.style().cbegin(), style.style().cend(),
		c_right ) != style.style().cend() )
			b.setAlignment( Qt::AlignRight );
	else if( std::find( style.style().cbegin(), style.style().cend(),
		c_center ) != style.style().cend() )
			b.setAlignment( Qt::AlignCenter );
}


//
// fillTextDocument
//

void fillTextDocument( QTextDocument * doc,
	const std::vector< Cfg::TextStyle > & text, qreal dpi, qreal scale )
{
	QTextCursor c( doc );

	c.movePosition( QTextCursor::End );

	QTextCharFormat fmt = c.charFormat();
	QTextBlockFormat b = c.blockFormat();

	foreach( const Cfg::TextStyle & s, text )
	{
		if( std::find( s.style().cbegin(), s.style().cend(),
			Cfg::c_normalStyle ) != s.style().cend() )
		{
			fmt.setFontWeight( QFont::Normal );
			fmt.setFontItalic( false );
			fmt.setFontUnderline( false );
		}
		else
		{
			if( std::find( s.style().cbegin(), s.style().cend(),
				Cfg::c_boldStyle ) != s.style().cend() )
					fmt.setFontWeight( QFont::Bold );
			else
				fmt.setFontWeight( QFont::Normal );

			if( std::find( s.style().cbegin(), s.style().cend(),
				Cfg::c_italicStyle ) != s.style().cend() )
					fmt.setFontItalic( true );
			else
				fmt.setFontItalic( false );

			if( std::find( s.style().cbegin(), s.style().cend(),
				Cfg::c_underlineStyle ) != s.style().cend() )
					fmt.setFontUnderline( true );
			else
				fmt.setFontUnderline( false );
		}

		initBlockFormat( b, s );

		QFont f = fmt.font();
		f.setPixelSize( MmPx::instance().fromPt( s.fontSize() * scale, dpi ) );
		fmt.setFont( f );

		c.setCharFormat( fmt );

		c.setBlockFormat( b );

		c.insertText( s.text() );

		c.movePosition( QTextCursor::End );
	}

	doc->clearUndoRedoStacks();
}


//
// textStyleFromFont
//

TextStyle
textStyleFromFont( const QFont & f )
{
	Cfg::TextStyle textStyle;

	std::vector< QString > style;

	if( f.weight() == QFont::Bold )
		style.push_back( Cfg::c_boldStyle );

	if( f.italic() )
		style.push_back( Cfg::c_italicStyle );

	if( f.underline() )
		style.push_back( Cfg::c_underlineStyle );

	if( style.empty() )
		style.push_back( Cfg::c_normalStyle );

	textStyle.set_style( style );
	textStyle.set_fontSize( MmPx::instance().toPtY( f.pixelSize() ) );

	return textStyle;
}

} /* namespace Cfg */


//
// MmPx
//

const MmPx &
MmPx::instance()
{
	static MmPx	inst;

	return inst;
}

qreal
MmPx::toMmX( qreal px ) const
{
	return ( px / ( m_xdots / 25.4 ) );
}

qreal
MmPx::fromMmX( qreal mm ) const
{
	return ( ( m_xdots / 25.4 ) * mm );
}

qreal
MmPx::toMmY( qreal px ) const
{
	return ( px / ( m_ydots / 25.4 ) );
}

qreal
MmPx::fromMmY( qreal mm ) const
{
	return ( ( m_ydots / 25.4 ) * mm );
}

MmPx::MmPx()
	:	m_xdots( qApp->primaryScreen()->physicalDotsPerInchX() )
	,	m_ydots( qApp->primaryScreen()->physicalDotsPerInchY() )
{
}

QSizeF
MmPx::a4() const
{
	return { fromMmX( 210.0 ), fromMmY( 297.0 ) };
}

qreal
MmPx::fromMm( qreal mm, qreal dpi ) const
{
	return ( ( dpi / 25.4 ) * mm );
}

int
MmPx::fromPtY( qreal pt ) const
{
	return qRound( ( m_ydots / 72.0 ) * pt );
}

int
MmPx::fromPt( qreal pt, qreal dpi ) const
{
	return qRound( ( dpi / 72.0 ) * pt );
}

qreal
MmPx::toPtY( int px ) const
{
	return ( ( static_cast< qreal > ( px ) / m_ydots ) * 72.0 );
}

qreal
MmPx::yDpi() const
{
	return m_ydots;
}

} /* namespace Core */

} /* namespace Prototyper */
