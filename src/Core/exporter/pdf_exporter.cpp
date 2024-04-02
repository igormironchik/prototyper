
/*
	SPDX-FileCopyrightText: 2016-2024 Igor Mironchik <igor.mironchik@gmail.com>
	SPDX-License-Identifier: GPL-3.0-or-later
*/

// Prototyper include.
#include "pdf_exporter.hpp"
#include "../form/utils.hpp"
#include "exporter_private.hpp"
#include "../constants.hpp"

// Qt include.
#include <QPdfWriter>
#include <QPageLayout>
#include <QTextDocument>
#include <QList>
#include <QTemporaryFile>
#include <QSharedPointer>
#include <QSvgGenerator>
#include <QSvgRenderer>
#include <QTextCursor>
#include <QTextBlock>
#include <QAbstractTextDocumentLayout>
#include <QPainter>

// C++ include.
#include <algorithm>


namespace Prototyper {

namespace Core {

static const int c_pageBreakType = QTextFormat::UserFormat + 1;

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
	//! Fill document.
	void fillDocument( QTextDocument & doc, qreal dpi );
	//! Print document.
	void printDocument( const QTextDocument & doc, QPdfWriter & pdf,
		const QRectF & body );

	//! Images' files.
	QList< QSharedPointer< QTemporaryFile > > m_images;
}; // class PdfExporterPrivate

void
PdfExporterPrivate::createImages()
{
	for( const Cfg::Page & form : m_cfg.page() )
	{
		m_images.append( QSharedPointer< QTemporaryFile >
			( new QTemporaryFile ) );

		m_images.last()->open();

		QSvgGenerator svg;
		svg.setFileName( m_images.last()->fileName() );
		svg.setResolution( c_resolution );

		drawForm( svg, form, c_resolution );

		m_images.last()->close();
	}
}

void
PdfExporterPrivate::fillDocument( QTextDocument & doc, qreal dpi )
{
	Cfg::fillTextDocument( &doc, m_cfg.description().text(), dpi );

	QTextCursor c( &doc );

	int i = 0;

	for( const Cfg::Page & form : m_cfg.page() )
	{
		c.movePosition( QTextCursor::End );

		c.insertText( QStringLiteral( "\n" ) );

		c.movePosition( QTextCursor::End );

		QTextCharFormat fmt;
		fmt.setObjectType( c_pageBreakType );

		c.insertText( QString( QChar::ObjectReplacementCharacter ) +
			QStringLiteral( "\n" ), fmt );

		c.movePosition( QTextCursor::End );

		QTextCharFormat header;
		header.setFontWeight( QFont::Bold );
		header.setFontPointSize( c_headerFontSize );

		c.setCharFormat( header );

		c.insertText( form.tabName() + QStringLiteral( "\n\n" ) );

		c.movePosition( QTextCursor::End );

		QTextImageFormat image;

		image.setName( m_images.at( i )->fileName() );

		++i;

		c.insertImage( image );
	}
}

void
PdfExporterPrivate::printDocument( const QTextDocument & doc, QPdfWriter & pdf,
	const QRectF & body )
{
	QTextBlock block = doc.begin();

	QPainter p;
	p.begin( &pdf );

	qreal y = 0.0;

	while( block.isValid() )
	{
		QTextBlock::Iterator it = block.begin();

		QTextImageFormat imageFormat;

		bool isImage = false;
		bool isBreak = false;

		for( ; !it.atEnd(); ++it )
		{
			const QString txt = it.fragment().text();
			bool isObject = txt.contains(
				QChar::ObjectReplacementCharacter );
			isImage = isObject &&
				it.fragment().charFormat().isImageFormat();
			isBreak = isObject &&
				( it.fragment().charFormat().objectType() == c_pageBreakType );

			if( isImage )
				imageFormat = it.fragment().charFormat().toImageFormat();
		}

		if( isBreak )
		{
			pdf.newPage();

			y = 0.0;
		}
		else if( isImage )
		{
			QSvgRenderer svg( imageFormat.name() );
			QSize s = svg.viewBox().size();

			if( s.width() > body.size().width() ||
				s.height() > body.size().height() - y )
					s.scale( QSize( qRound( body.size().width() ),
						qRound( body.size().height() - y ) ), Qt::KeepAspectRatio );

			if( ( y + s.height() ) > body.height() )
			{
				pdf.newPage();

				y = 0.0;
			}

			p.save();

			p.translate( ( body.size().width() - s.width() ) / 2, y );

			svg.render( &p, QRectF( 0, 0, s.width(), s.height() ) );

			y += s.height();

			p.restore();
		}
		else
		{
			const QRectF r = block.layout()->boundingRect();

			block.layout()->setPosition( QPointF( 0.0, 0.0 ) );

			if( ( y + r.height() ) > body.height() )
			{
				pdf.newPage();

				y = 0.0;
			}

			block.layout()->draw( &p, QPointF( 0.0, y ) );

			y += r.height();
		}

		block = block.next();
	}

	p.end();
}


//
// PdfExporter
//

PdfExporter::PdfExporter( const Cfg::Project & project )
	:	Exporter( std::make_unique< PdfExporterPrivate >( project, this ) )
{
}

PdfExporter::~PdfExporter() = default;

void
PdfExporter::exportToDoc( const QString & fileName )
{
	PdfExporterPrivate * d = d_ptr();

	d->m_images.clear();

	QPdfWriter pdf( fileName );

	pdf.setResolution( c_resolution );

	QPageLayout layout = pdf.pageLayout();
	layout.setUnits( QPageLayout::Point );
	const qreal margin = ( 2.0 / 2.54 ) * 72;
	layout.setMargins( QMarginsF( margin, margin,
		margin, margin ) );
	pdf.setPageLayout( layout );

	const QRectF body( 0, 0, pdf.width(), pdf.height() );

	d->createImages();

	QTextDocument doc;
	doc.documentLayout()->setPaintDevice( &pdf );
	doc.setPageSize( body.size() );

	d->fillDocument( doc, c_resolution );

	doc.setPageSize( body.size() );

	d->printDocument( doc, pdf, body );
}

} /* namespace Core */

} /* namespace Prototyper */
