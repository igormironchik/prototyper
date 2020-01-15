
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
#include "html_exporter.hpp"
#include "utils.hpp"
#include "exporter_private.hpp"

// Qt include.
#include <QTextStream>
#include <QFile>
#include <QSvgGenerator>
#include <QByteArray>
#include <QBuffer>


namespace Prototyper {

namespace Core {

//
// HtmlExporterPrivate
//

class HtmlExporterPrivate
	:	public ExporterPrivate
{
public:
	HtmlExporterPrivate( const Cfg::Project & cfg, HtmlExporter * parent )
		:	ExporterPrivate( cfg, parent )
	{
	}

	//! Print document.
	void printDocument( QTextStream & stream );
}; // class HtmlExporterPrivate

static inline QString printStyle( const Cfg::TextStyle & style )
{
	return QStringLiteral( "style=\"font-size: " ) +
		QString::number( qRound( style.fontSize() ) )
		+ QStringLiteral( "pt;\"" );
}

static inline void printText( QTextStream & stream,
	const std::vector< Cfg::TextStyle > & text )
{
	foreach( const Cfg::TextStyle & t, text )
	{
		stream << QStringLiteral( "<span " )
			<< printStyle( t )
			<< QStringLiteral( ">" )
			<< ( std::find( t.style().cbegin(), t.style().cend(),
					Cfg::c_boldStyle ) != t.style().cend() ?
						QStringLiteral( "<b>" ) : QString() )
			<< ( std::find( t.style().cbegin(), t.style().cend(),
					Cfg::c_italicStyle ) != t.style().cend() ?
						QStringLiteral( "<i>" ) : QString() )
			<< ( std::find( t.style().cbegin(), t.style().cend(),
					Cfg::c_underlineStyle ) != t.style().cend() ?
						QStringLiteral( "<ins>" ) : QString() );

		QStringList strings = t.text().split( QLatin1Char( '\n' ) );

		foreach( const QString & str, strings )
			stream << str << QStringLiteral( "<br>" ) << endl;

		stream << ( std::find( t.style().cbegin(), t.style().cend(),
					Cfg::c_underlineStyle ) != t.style().cend()  ?
						QStringLiteral( "</ins>" ) : QString() )
			<< ( std::find( t.style().cbegin(), t.style().cend(),
					Cfg::c_italicStyle ) != t.style().cend() ?
						QStringLiteral( "</i>" ) : QString() )
			<< ( std::find( t.style().cbegin(), t.style().cend(),
					Cfg::c_boldStyle ) != t.style().cend() ?
						QStringLiteral( "</b>" ) : QString() )
			<< QStringLiteral( "</span>" )
			<< endl;
	}
}

void
HtmlExporterPrivate::printDocument( QTextStream & stream )
{
	stream << QStringLiteral( "<!DOCTYPE html><head></head>" )
		<< endl << QStringLiteral( "<body>" ) << endl
		<< QStringLiteral( "<div style=\"width: 800px; margin: auto;\">" ) ;

	printText( stream, m_cfg.description().text() );

	stream << QStringLiteral( "<br><br>" ) << endl;

	foreach( const Cfg::Page & form, m_cfg.page() )
	{
		std::vector< Cfg::TextStyle > headList;
		Cfg::TextStyle head;
		head.style().push_back( Cfg::c_boldStyle );
		head.fontSize() = 20.0;
		head.text() = form.tabName();
		headList.push_back( head );

		stream << QStringLiteral( "<a name=\"" )
			<< form.tabName() << QStringLiteral( "\">" );

		printText( stream, headList );

		stream << QStringLiteral( "</a>" ) << QStringLiteral( "<br><br>" );

		QByteArray data;
		QBuffer buff( &data );
		buff.open( QIODevice::WriteOnly );
		QSvgGenerator svg;
		svg.setResolution( 150 );
		svg.setOutputDevice( &buff );

		drawForm( svg, form, 150.0 );

		const int i = data.indexOf( QStringLiteral( "\n" ) );

		if( i != -1 )
			data.remove( 0, i );

		stream
			<< QStringLiteral( "<div>" )
			<< data << QStringLiteral( "</div><br>" );
	}

	stream << QStringLiteral( "</div></body>" ) << endl;
}


//
// HtmlExporter
//

HtmlExporter::HtmlExporter( const Cfg::Project & project )
	:	Exporter( QScopedPointer< ExporterPrivate >
			( new HtmlExporterPrivate( project, this ) ) )
{
}

HtmlExporter::~HtmlExporter() = default;

void
HtmlExporter::exportToDoc( const QString & fileName )
{
	QFile file( fileName );

	file.open( QIODevice::WriteOnly | QIODevice::Truncate );

	QTextStream stream( &file );

	d_ptr()->printDocument( stream );

	file.close();
}

} /* namespace Core */

} /* namespace Prototyper */
