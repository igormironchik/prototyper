
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
#include "utils.hpp"

// Qt include.
#include <QTextCharFormat>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextBlockFormat>


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

QList< QString > textStyle( const QTextCharFormat & f,
	const QTextBlockFormat & b )
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

	switch( b.alignment() )
	{
		case Qt::AlignCenter :
		case Qt::AlignHCenter :
		{
			res.append( c_center );
		}
			break;

		case Qt::AlignRight :
		{
			res.append( c_right );
		}
			break;

		default :
		{
			res.append( c_left );
		}
			break;
	}

	return res;
} // textStyle


//
// text
//

QList< Cfg::TextStyle > text( QTextCursor c, const QString & data )
{
	QList< Cfg::TextStyle > blocks;

	int pos = 0;

	c.setPosition( pos );

	QTextCharFormat f = c.charFormat();
	QTextBlockFormat b = c.blockFormat();

	if( f.fontPointSize() < 1.0 )
		f.setFontPointSize( 10.0 );

	QString t;

	while( c.movePosition( QTextCursor::NextCharacter ) )
	{
		if( f != c.charFormat() || b != c.blockFormat() )
		{
			Cfg::TextStyle style;
			style.setStyle( textStyle( f, b ) );
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
		style.setStyle( textStyle( f, b ) );
		style.setFontSize( f.fontPointSize() );
		style.setText( t );

		blocks.append( style );
	}

	return blocks;
} // text


//
// pen
//

Cfg::Pen pen( const QPen & p )
{
	Cfg::Pen res;

	res.setWidth( p.widthF() );
	res.setColor( p.color().name( QColor::HexArgb ) );

	return res;
} // pen


//
// fromPen
//

QPen fromPen( const Cfg::Pen & p )
{
	return QPen( QColor( p.color() ), p.width(), Qt::SolidLine );
} // fromPen


//
// brush
//

Cfg::Brush brush( const QBrush & b )
{
	Cfg::Brush res;

	res.setColor( b.color().name( QColor::HexArgb ) );

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
	if( style.style().contains( c_left ) )
		b.setAlignment( Qt::AlignLeft );
	else if( style.style().contains( c_right ) )
		b.setAlignment( Qt::AlignRight );
	else if( style.style().contains( c_center ) )
		b.setAlignment( Qt::AlignCenter );
}


//
// fillTextDocument
//

void fillTextDocument( QTextDocument * doc,
	const QList< Cfg::TextStyle > & text, qreal scale )
{
	QTextCursor c( doc );

	c.movePosition( QTextCursor::End );

	QTextCharFormat fmt = c.charFormat();
	QTextBlockFormat b = c.blockFormat();

	foreach( const Cfg::TextStyle & s, text )
	{
		if( s.style().contains( Cfg::c_normalStyle ) )
		{
			fmt.setFontWeight( QFont::Normal );
			fmt.setFontItalic( false );
			fmt.setFontUnderline( false );
		}
		else
		{
			if( s.style().contains( Cfg::c_boldStyle ) )
				fmt.setFontWeight( QFont::Bold );
			else
				fmt.setFontWeight( QFont::Normal );

			if( s.style().contains( Cfg::c_italicStyle ) )
				fmt.setFontItalic( true );
			else
				fmt.setFontItalic( false );

			if( s.style().contains( Cfg::c_underlineStyle ) )
				fmt.setFontUnderline( true );
			else
				fmt.setFontUnderline( false );
		}

		initBlockFormat( b, s );

		fmt.setFontPointSize( s.fontSize() * scale );

		c.setCharFormat( fmt );

		c.setBlockFormat( b );

		c.insertText( s.text() );

		c.movePosition( QTextCursor::End );
	}
}

} /* namespace Cfg */

} /* namespace Core */

} /* namespace Prototyper */
