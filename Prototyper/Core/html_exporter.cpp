
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
	const QList< Cfg::TextStyle > & text )
{
	foreach( const Cfg::TextStyle & t, text )
	{
		stream << QLatin1String( "<span " )
			<< printStyle( t )
			<< QLatin1String( ">" )
			<< ( t.style().contains( Cfg::c_boldStyle ) ?
					QLatin1String( "<b>" ) : QString() )
			<< ( t.style().contains( Cfg::c_italicStyle ) ?
					QLatin1String( "<i>" ) : QString() )
			<< ( t.style().contains( Cfg::c_underlineStyle ) ?
					QLatin1String( "<ins>" ) : QString() );

		QStringList strings = t.text().split( QLatin1Char( '\n' ) );

		foreach( const QString & str, strings )
			stream << str << QLatin1String( "<br>" ) << endl;

		stream << ( t.style().contains( Cfg::c_underlineStyle ) ?
					QLatin1String( "</ins>" ) : QString() )
			<< ( t.style().contains( Cfg::c_italicStyle ) ?
					QLatin1String( "</i>" ) : QString() )
			<< ( t.style().contains( Cfg::c_boldStyle ) ?
					QLatin1String( "</b>" ) : QString() )
			<< QLatin1String( "</span>" )
			<< endl;
	}
}

static inline void printLink( QTextStream & stream, const QString & id,
	QMap< QString, QString > & lnks )
{
	QList< Cfg::TextStyle > linkList;
	Cfg::TextStyle lnk;
	lnk.style().append( Cfg::c_italicStyle );
	lnk.style().append( Cfg::c_underlineStyle );
	lnk.fontSize() = 8.0;
	lnk.text() = QString( "Linked to: " ) + lnks[ id ];
	linkList.append( lnk );

	stream << QLatin1String( "<a href=\"#" )
		<< lnks[ id ]
		<< QLatin1String( "\">" );

	printText( stream, linkList );

	lnks.remove( id );

	stream << QLatin1String( "</a>" ) << endl;
}

void
HtmlExporterPrivate::printDocument( QTextStream & stream )
{
	stream << QLatin1String( "<!DOCTYPE html><head></head>" )
		<< endl << QLatin1String( "<body>" ) << endl
		<< QLatin1String( "<div style=\"width: 800px; margin: auto;\">" ) ;

	printText( stream, m_cfg.description().text() );

	stream << QLatin1String( "<br><br>" ) << endl;

	foreach( const Cfg::Form & form, m_cfg.form() )
	{
		QMap< QString, QString > lnks = links( form );

		QList< Cfg::TextStyle > headList;
		Cfg::TextStyle head;
		head.style().append( Cfg::c_boldStyle );
		head.fontSize() = 20.0;
		head.text() = form.tabName();
		headList.append( head );

		stream << QLatin1String( "<a name=\"" )
			<< form.tabName() << QLatin1String( "\">" );

		printText( stream, headList );

		stream << QLatin1String( "</a>" ) << QLatin1String( "<br><br>" );

		QByteArray data;
		QBuffer buff( &data );
		buff.open( QIODevice::WriteOnly );
		QSvgGenerator svg;
		svg.setOutputDevice( &buff );

		drawForm( svg, form );

		const int i = data.indexOf( QLatin1String( "\n" ) );

		if( i != -1 )
			data.remove( 0, i );

		stream
			<< QLatin1String( "<div style=\"max-width=100%; "
							  "margin: auto; width: " )
			<< QString::number( form.size().width() )
			<< QLatin1String( "px;\">" )
			<< data << QLatin1String( "</div><br>" );

		auto formIt = std::find_if( form.desc().constBegin(),
			form.desc().constEnd(),
			[&form] ( const Cfg::Description & desc ) -> bool
				{
					return ( form.tabName() == desc.id() );
				}
		);

		if( formIt != form.desc().constEnd() && !formIt->text().isEmpty() )
		{
			stream << QLatin1String( "<br><br>" ) << endl;

			printText( stream, formIt->text() );

			stream << QLatin1String( "<br><br>" ) << endl;

			if( lnks.contains( formIt->id() ) )
			{
				printLink( stream, formIt->id(), lnks );

				stream << QLatin1String( "<br><br>" ) << endl;
			}
		}

		for( auto it = form.desc().constBegin(), last = form.desc().constEnd();
			it != last; ++it )
		{
			if( it != formIt && !it->text().isEmpty() )
			{
				QList< Cfg::TextStyle > headList;
				Cfg::TextStyle head;
				head.style().append( Cfg::c_boldStyle );
				head.style().append( Cfg::c_italicStyle );
				head.fontSize() = 15.0;
				head.text() = it->id();
				headList.append( head );

				printText( stream, headList );

				stream << QLatin1String( "<br><br>" ) << endl;

				printText( stream, it->text() );

				stream << QLatin1String( "<br><br>" ) << endl;

				if( lnks.contains( it->id() ) )
				{
					printLink( stream, it->id(), lnks );

					stream << QLatin1String( "<br><br>" ) << endl;
				}
			}
		}

		if( !lnks.isEmpty() )
		{
			QList< Cfg::TextStyle > linksList;
			Cfg::TextStyle head;
			head.style().append( Cfg::c_boldStyle );
			head.style().append( Cfg::c_italicStyle );
			head.fontSize() = 15.0;
			head.text() = QLatin1String( "Links:" );
			linksList.append( head );

			printText( stream, linksList );

			stream << QLatin1String( "<br><br>" ) << endl;

			linksList.clear();
			Cfg::TextStyle lnk;
			lnk.style().append( Cfg::c_underlineStyle );
			lnk.setFontSize( 8.0 );
			linksList.append( lnk );

			QMapIterator< QString, QString > it( lnks );

			while( it.hasNext() )
			{
				it.next();

				stream << QLatin1String( "<a href=\"#" )
					<< lnks[ it.key() ]
					<< QLatin1String( "\">" );

				linksList.last().setText( it.key() + QLatin1String( " -> " ) +
					it.value() );

				printText( stream, linksList );

				stream << QLatin1String( "</a><br>" ) << endl;
			}
		}
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
