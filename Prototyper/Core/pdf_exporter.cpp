
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
#include "pdf_exporter.hpp"
#include "utils.hpp"

// Qt include.
#include <QPdfWriter>
#include <QPageLayout>
#include <QTextDocument>
#include <QList>
#include <QTemporaryFile>
#include <QSharedPointer>
#include <QSvgGenerator>


namespace Prototyper {

namespace Core {

//
// PdfExporterPrivate
//

class PdfExporterPrivate
	:	public ExporterPrivate
{
public:
	PdfExporterPrivate( const Cfg::Project & cfg, PdfExporter * parent )
		:	ExporterPrivate( cfg, parent )
	{
	}

	//! Create tmp images.
	void createImages();

	//! Init.
	void init() Q_DECL_OVERRIDE;

	//! Images' files.
	QList< QSharedPointer< QTemporaryFile > > m_images;
}; // class PdfExporterPrivate

void
PdfExporterPrivate::init()
{
	ExporterPrivate::init();
}

void
PdfExporterPrivate::createImages()
{
	foreach( const Cfg::Form & form, m_cfg.form() )
	{
		m_images.append( QSharedPointer< QTemporaryFile >
			( new QTemporaryFile ) );

		m_images.last()->open();

		QSvgGenerator svg;
		svg.setFileName( m_images.last()->fileName() );
		svg.setResolution( 72 );

		drawForm( svg, form );
	}
}


//
// PdfExporter
//

PdfExporter::PdfExporter( const Cfg::Project & project )
	:	Exporter( QScopedPointer< ExporterPrivate >
			( new PdfExporterPrivate( project, this ) ) )
{
}

PdfExporter::~PdfExporter()
{
}

void
PdfExporter::exportToDoc( const QString & fileName )
{
	PdfExporterPrivate * d = d_ptr();

	QPdfWriter pdf( fileName );

	pdf.setResolution( 72 );

	const QRect page = pdf.pageLayout().paintRectPixels( pdf.resolution() );

	d->createImages();

	QTextDocument doc;
	doc.setPageSize( QSizeF( page.size() ) );

	Cfg::fillTextDocument( &doc, d->m_cfg.description().text() );

	doc.print( &pdf );
}

} /* namespace Core */

} /* namespace Prototyper */
