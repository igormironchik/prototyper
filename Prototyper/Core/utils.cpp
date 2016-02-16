
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


namespace Prototyper {

namespace Core {

bool operator != ( const QTextCharFormat & f1, const QTextCharFormat & f2 )
{
	return ( f1.fontPointSize() != f2.fontPointSize() ||
		f1.fontWeight() != f2.fontWeight() ||
		f1.fontItalic() != f2.fontItalic() ||
		f1.fontUnderline() != f2.fontUnderline() );
}


namespace Cfg {

//
// textStyle
//

QList< QString > textStyle( const QTextCharFormat & f )
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

	QString t;

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

	return blocks;
} // text

} /* namespace Cfg */

} /* namespace Core */

} /* namespace Prototyper */