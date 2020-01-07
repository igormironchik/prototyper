
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
	return QString( "style=\"font-size: ") +
		QString::number( qRound( style.fontSize() ) )
		+ QLatin1String( "pt;\"" );
}

static inline void printText( QTextStream & stream,
	const std::vector< Cfg::TextStyle > & text )
{
	foreach( const Cfg::TextStyle & t, text )
	{
		stream << QLatin1String( "<span " )
			<< printStyle( t )
			<< QLatin1String( ">" )
			<< ( std::find( t.style().cbegin(), t.style().cend(),
					Cfg::c_boldStyle ) != t.style().cend() ?
						QLatin1String( "<b>" ) : QString() )
			<< ( std::find( t.style().cbegin(), t.style().cend(),
					Cfg::c_italicStyle ) != t.style().cend() ?
						QLatin1String( "<i>" ) : QString() )
			<< ( std::find( t.style().cbegin(), t.style().cend(),
						Cfg::c_underlineStyle ) != t.style().cend() ?
							QLatin1String( "<ins>" ) : QString() );

		QStringList strings = t.text().split( QLatin1Char( '\n' ) );

		foreach( const QString & str, strings )
			stream << str << QLatin1String( "<br>" ) << endl;

		stream << ( std::find( t.style().cbegin(), t.style().cend(),
						Cfg::c_underlineStyle ) != t.style().cend()  ?
							QLatin1String( "</ins>" ) : QString() )
			<< ( std::find( t.style().cbegin(), t.style().cend(),
					Cfg::c_italicStyle ) != t.style().cend() ?
						QLatin1String( "</i>" ) : QString() )
			<< ( std::find( t.style().cbegin(), t.style().cend(),
					Cfg::c_boldStyle ) != t.style().cend() ?
						QLatin1String( "</b>" ) : QString() )
			<< QLatin1String( "</span>" )
			<< endl;
	}
}

void
HtmlExporterPrivate::printDocument( QTextStream & stream )
{
	stream << QLatin1String( "<!DOCTYPE html><head></head>" )
		<< endl << QLatin1String( "<body>" ) << endl
		<< QLatin1String( "<div style=\"width: 800px; margin: auto;\">" ) ;

	printText( stream, m_cfg.description().text() );

	stream << QLatin1String( "<br><br>" ) << endl;

	foreach( const Cfg::Page & form, m_cfg.page() )
	{
		std::vector< Cfg::TextStyle > headList;
		Cfg::TextStyle head;
		head.style().push_back( Cfg::c_boldStyle );
		head.fontSize() = 20.0;
		head.text() = form.tabName();
		headList.push_back( head );

		stream << QLatin1String( "<a name=\"" )
			<< form.tabName() << QLatin1String( "\">" );

		printText( stream, headList );

		stream << QLatin1String( "</a>" ) << QLatin1String( "<br><br>" );

		QByteArray data;
		QBuffer buff( &data );
		buff.open( QIODevice::WriteOnly );
		QSvgGenerator svg;
		svg.setResolution( 150 );
		svg.setOutputDevice( &buff );

		drawForm( svg, form, 150.0 );

		const int i = data.indexOf( QLatin1String( "\n" ) );

		if( i != -1 )
			data.remove( 0, i );

		stream
			<< QLatin1String( "<div>" )
			<< data << QLatin1String( "</div><br>" );
	}

	stream << QLatin1String( "</div></body>" ) << endl;
}


//
// HtmlExporter
//

HtmlExporter::HtmlExporter( const Cfg::Project & project )
	:	Exporter( QScopedPointer< ExporterPrivate >
			( new HtmlExporterPrivate( project, this ) ) )
{
}

HtmlExporter::~HtmlExporter()
{
}

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
