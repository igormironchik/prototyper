
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
#include "exporter.hpp"
#include "utils.hpp"

// Qt include.
#include <QSvgGenerator>
#include <QPainter>
#include <QTextDocument>


namespace Prototyper {

namespace Core {

//
// ExporterPrivate
//

ExporterPrivate::ExporterPrivate( const Cfg::Project & cfg, Exporter * parent )
	:	q( parent )
	,	m_cfg( cfg )
{
}

ExporterPrivate::~ExporterPrivate()
{
}

void
ExporterPrivate::init()
{

}

static inline void drawLine( const Cfg::Line & line, QPainter & p )
{
	p.save();

	p.setPen( QPen( QColor( line.pen().color() ), line.pen().width() ) );

	p.drawLine( line.p1().x(), line.p1().y(),
		line.p2().x(), line.p2().y() );

	p.restore();
}

static inline void drawGroup( const Cfg::Group & group, QPainter & p )
{
	foreach( const Cfg::Group & group, group.group() )
		drawGroup( group, p );

	foreach( const Cfg::Line & line, group.line() )
		drawLine( line, p );

	foreach( const Cfg::Polyline & poly, group.polyline() )
	{
		foreach( const Cfg::Line & line, poly.line() )
			drawLine( line, p );
	}

	foreach( const Cfg::Text & text, group.text() )
	{
		p.save();

		QTextDocument doc;
		doc.setTextWidth( text.textWidth() );

		Cfg::fillTextDocument( &doc, text.text() );

		p.translate( text.pos().x(), text.pos().y() );

		doc.drawContents( &p );

		p.restore();
	}

	foreach( const Cfg::Image & image, group.image() )
	{
		const QSize s( image.size().width(), image.size().height() );

		const QByteArray data = QByteArray::fromBase64( image.data().toLatin1() );

		QImage img = QImage::fromData( data, "PNG" );

		p.drawImage( image.pos().x(), image.pos().y(),
			img.scaled( s, ( image.keepAspectRatio() ? Qt::KeepAspectRatio :
					Qt::IgnoreAspectRatio ),
				Qt::SmoothTransformation ) );
	}
}

void
ExporterPrivate::drawForm( QSvgGenerator & svg, const Cfg::Form & form )
{
	svg.setSize( QSize( form.size().width(), form.size().height() ) );
	svg.setViewBox( QRect( 0, 0, form.size().width(), form.size().height() ) );

	QPainter p;
	p.begin( &svg );

	p.setPen( Qt::gray );

	p.drawRect( QRect( 0, 0, form.size().width(),
		form.size().height() ) );

	foreach( const Cfg::Group & group, form.group() )
		drawGroup( group, p );

	foreach( const Cfg::Line & line, form.line() )
		drawLine( line, p );

	foreach( const Cfg::Polyline & poly, form.polyline() )
	{
		foreach( const Cfg::Line & line, poly.line() )
			drawLine( line, p );
	}

	foreach( const Cfg::Text & text, form.text() )
	{
		p.save();

		QTextDocument doc;
		doc.setTextWidth( text.textWidth() );

		Cfg::fillTextDocument( &doc, text.text() );

		p.translate( text.pos().x(), text.pos().y() );

		doc.drawContents( &p );

		p.restore();
	}

	foreach( const Cfg::Image & image, form.image() )
	{
		const QSize s( image.size().width(), image.size().height() );

		const QByteArray data = QByteArray::fromBase64( image.data().toLatin1() );

		QImage img = QImage::fromData( data, "PNG" );

		p.drawImage( image.pos().x(), image.pos().y(),
			img.scaled( s, ( image.keepAspectRatio() ? Qt::KeepAspectRatio :
					Qt::IgnoreAspectRatio ),
				Qt::SmoothTransformation ) );
	}

	p.end();
}


//
// Exporter
//

Exporter::Exporter( const Cfg::Project & project )
	:	d( new ExporterPrivate( project, this ) )
{
	d->init();
}

Exporter::~Exporter()
{
}

Exporter::Exporter( QScopedPointer< ExporterPrivate > && dd )
	:	d( 0 )
{
	QScopedPointer< ExporterPrivate > tmp( 0 );

	tmp.swap( dd );

	tmp->init();

	d.swap( tmp );
}

} /* namespace Core */

} /* namespace Prototyper */
