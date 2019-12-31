
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
#include "form_checkbox.hpp"
#include "form_radio_button.hpp"
#include "form_combobox.hpp"
#include "form_spinbox.hpp"
#include "form_hslider.hpp"
#include "form_vslider.hpp"
#include "page.hpp"
#include "form_polyline.hpp"

// Qt include.
#include <QSvgGenerator>
#include <QPainter>
#include <QTextDocument>
#include <QAbstractTextDocumentLayout>

// C++ include.
#include <algorithm>


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

static inline void drawLine( const Cfg::Line & line, QPainter & p, qreal dpi )
{
	p.save();

	p.setPen( QPen( QColor( line.pen().color() ),
		MmPx::instance().fromMm( line.pen().width(), dpi ) ) );

	p.drawLine( MmPx::instance().fromMm( line.p1().x() + line.pos().x(), dpi ),
		MmPx::instance().fromMm( line.p1().y() + line.pos().y(), dpi ),
		MmPx::instance().fromMm( line.p2().x() + line.pos().x(), dpi ),
		MmPx::instance().fromMm( line.p2().y() + line.pos().y(), dpi ) );

	p.restore();
}

static inline void drawPolyline( const Cfg::Polyline & cfg, QPainter & p, qreal dpi )
{
	p.save();

	FormPolyline::draw( &p, cfg, dpi );

	p.restore();
}

static inline void drawRect( const Cfg::Rect & rect, QPainter & p, qreal dpi )
{
	p.save();

	const QRectF r( MmPx::instance().fromMm( rect.pos().x(), dpi ),
		MmPx::instance().fromMm( rect.pos().y(), dpi ),
		MmPx::instance().fromMm( rect.size().width(), dpi ),
		MmPx::instance().fromMm( rect.size().height(), dpi ) );

	p.setPen( QPen( QColor( rect.pen().color() ),
		MmPx::instance().fromMm( rect.pen().width(), dpi ) ) );

	p.setBrush( Qt::NoBrush );

	p.drawRect( r );

	p.restore();
}

static inline QFont font( const Cfg::TextStyle & s, QPainter & p, qreal dpi )
{
	Q_UNUSED( dpi )

	QFont f = p.font();

	if( std::find( s.style().cbegin(), s.style().cend(), Cfg::c_boldStyle ) !=
		s.style().cend() )
			f.setBold( true );

	if( std::find( s.style().cbegin(), s.style().cend(), Cfg::c_italicStyle ) !=
		s.style().cend() )
			f.setItalic( true );

	if( std::find( s.style().cbegin(), s.style().cend(), Cfg::c_underlineStyle ) !=
		s.style().cend() )
			f.setUnderline( true );

	f.setPointSize( s.fontSize() );

	return f;
}

static inline void drawButton( const Cfg::Button & btn, QPainter & p, qreal dpi )
{
	p.save();

	p.setPen( QPen( QColor( btn.pen().color() ),
		MmPx::instance().fromMm( btn.pen().width(), dpi ) ) );

	p.setFont( font( btn.text(), p, dpi ) );

	const QRect r( MmPx::instance().fromMm( btn.pos().x(), dpi ),
		MmPx::instance().fromMm( btn.pos().y(), dpi ),
		MmPx::instance().fromMm( btn.size().width(), dpi ),
		MmPx::instance().fromMm( btn.size().height(), dpi ) );

	p.drawRect( r );

	p.drawText( r, Qt::AlignCenter, btn.text().text() );

	p.restore();
}

static inline void drawCheckBox( const Cfg::CheckBox & chk, QPainter & p, qreal dpi )
{
	p.save();

	const QRectF r = QRectF( MmPx::instance().fromMm( chk.pos().x(), dpi ),
		MmPx::instance().fromMm( chk.pos().y(), dpi ),
		MmPx::instance().fromMm( chk.size().width(), dpi ),
		MmPx::instance().fromMm( chk.size().height(), dpi ) );

	FormCheckBox::draw( &p, Cfg::fromPen( chk.pen(), dpi ),
		font( chk.text(), p, dpi ),
		r,
		MmPx::instance().fromMm( chk.size().width(), dpi ),
		chk.isChecked(),
		chk.text().text(),
		r,
		dpi );

	p.restore();
}

static inline void drawRadioButton( const Cfg::CheckBox & chk, QPainter & p, qreal dpi )
{
	p.save();

	const QRectF r = QRectF( MmPx::instance().fromMm( chk.pos().x(), dpi ),
		MmPx::instance().fromMm( chk.pos().y(), dpi ),
		MmPx::instance().fromMm( chk.size().width(), dpi ),
		MmPx::instance().fromMm( chk.size().height(), dpi ) );

	FormRadioButton::draw( &p, Cfg::fromPen( chk.pen(), dpi ),
		font( chk.text(), p, dpi ),
		r,
		MmPx::instance().fromMm( chk.width(), dpi ),
		chk.isChecked(),
		chk.text().text(),
		r,
		dpi );

	p.restore();
}

static inline void drawComboBox( const Cfg::ComboBox & cb, QPainter & p, qreal dpi )
{
	p.save();

	FormComboBox::draw( &p,
		QRectF( MmPx::instance().fromMm( cb.pos().x(), dpi ),
			MmPx::instance().fromMm( cb.pos().y(), dpi ),
			MmPx::instance().fromMm( cb.size().width(), dpi ),
			MmPx::instance().fromMm( cb.size().height(), dpi ) ),
		Cfg::fromPen( cb.pen(), dpi ), dpi );

	p.restore();
}

static inline void drawSpinBox( const Cfg::SpinBox & s, QPainter & p, qreal dpi )
{
	p.save();

	FormSpinBox::draw( &p,
		QRectF( MmPx::instance().fromMm( s.pos().x(), dpi ),
			MmPx::instance().fromMm( s.pos().y(), dpi ),
			MmPx::instance().fromMm( s.size().width(), dpi ),
			MmPx::instance().fromMm( s.size().height(), dpi ) ),
		Cfg::fromPen( s.pen(), dpi ),
		font( s.text(), p, dpi ),
		s.text().text(),
		dpi );

	p.restore();
}

static inline void drawHSlider( const Cfg::HSlider & hs, QPainter & p, qreal dpi )
{
	p.save();

	FormHSlider::draw( &p,
		QRectF( MmPx::instance().fromMm( hs.pos().x(), dpi ),
			MmPx::instance().fromMm( hs.pos().y(), dpi ),
			MmPx::instance().fromMm( hs.size().width(), dpi ),
			MmPx::instance().fromMm( hs.size().height(), dpi ) ),
		Cfg::fromPen( hs.pen(), dpi ), dpi );

	p.restore();
}

static inline void drawVSlider( const Cfg::VSlider & vs, QPainter & p, qreal dpi )
{
	p.save();

	FormVSlider::draw( &p,
		QRectF( MmPx::instance().fromMm( vs.pos().x(), dpi ),
			MmPx::instance().fromMm( vs.pos().y(), dpi ),
			MmPx::instance().fromMm( vs.size().width(), dpi ),
			MmPx::instance().fromMm( vs.size().height(), dpi ) ),
		Cfg::fromPen( vs.pen(), dpi ), dpi );

	p.restore();
}

static inline void drawGroup( const Cfg::Group & group, QPainter & p, qreal dpi,
	QSvgGenerator & svg )
{
	p.save();

	p.translate( MmPx::instance().fromMm( group.pos().x(), dpi ),
		MmPx::instance().fromMm( group.pos().y(), dpi ) );

	foreach( const Cfg::Group & group, group.group() )
		drawGroup( group, p, dpi, svg );

	foreach( const Cfg::Line & line, group.line() )
		drawLine( line, p, dpi );

	foreach( const Cfg::Polyline & poly, group.polyline() )
		drawPolyline( poly, p, dpi );

	foreach( const Cfg::Text & text, group.text() )
	{
		p.save();

		QTextDocument doc;
		doc.documentLayout()->setPaintDevice( &svg );
		doc.setTextWidth( text.textWidth() );

		Cfg::fillTextDocument( &doc, text.text() );

		p.translate( MmPx::instance().fromMm( text.pos().x(), dpi ),
			MmPx::instance().fromMm( text.pos().y(), dpi ) );

		doc.drawContents( &p );

		p.restore();
	}

	foreach( const Cfg::Image & image, group.image() )
	{
		const QSize s( MmPx::instance().fromMm( image.size().width(), dpi ),
			MmPx::instance().fromMm( image.size().height(), dpi ) );

		const QByteArray data = QByteArray::fromBase64( image.data().toLatin1() );

		QImage img = QImage::fromData( data, "PNG" );

		p.drawImage( MmPx::instance().fromMm( image.pos().x(), dpi ),
			MmPx::instance().fromMm( image.pos().y(), dpi ),
			img.scaled( s, ( image.keepAspectRatio() ? Qt::KeepAspectRatio :
					Qt::IgnoreAspectRatio ),
				Qt::SmoothTransformation ) );
	}

	foreach( const Cfg::Rect & rect, group.rect() )
		drawRect( rect, p, dpi );

	foreach( const Cfg::Button & btn, group.button() )
		drawButton( btn, p, dpi );

	foreach( const Cfg::CheckBox & chk, group.checkbox() )
		drawCheckBox( chk, p, dpi );

	foreach( const Cfg::CheckBox & chk, group.radiobutton() )
		drawRadioButton( chk, p, dpi );

	foreach( const Cfg::ComboBox & cb, group.combobox() )
		drawComboBox( cb, p, dpi );

	foreach( const Cfg::SpinBox & s, group.spinbox() )
		drawSpinBox( s, p, dpi );

	foreach( const Cfg::HSlider & hs, group.hslider() )
		drawHSlider( hs, p, dpi );

	foreach( const Cfg::VSlider & vs, group.vslider() )
		drawVSlider( vs, p, dpi );

	p.restore();
}

void
ExporterPrivate::drawForm( QSvgGenerator & svg, const Cfg::Page & form, qreal dpi )
{
	svg.setViewBox( QRect( -1, 0,
		MmPx::instance().fromMm( form.size().width(), dpi ) + 1,
		MmPx::instance().fromMm( form.size().height(), dpi ) ) );
	svg.setResolution( dpi );

	QPainter p;
	p.begin( &svg );

	p.setPen( Qt::gray );

	Page::draw( &p, MmPx::instance().fromMm( form.size().width(), dpi ),
		MmPx::instance().fromMm( form.size().height(), dpi ), 0, false );

	foreach( const Cfg::Group & group, form.group() )
		drawGroup( group, p, dpi, svg );

	foreach( const Cfg::Line & line, form.line() )
		drawLine( line, p, dpi );

	foreach( const Cfg::Polyline & poly, form.polyline() )
		drawPolyline( poly, p, dpi );

	foreach( const Cfg::Button & btn, form.button() )
		drawButton( btn, p, dpi );

	foreach( const Cfg::CheckBox & chk, form.checkbox() )
		drawCheckBox( chk, p, dpi );

	foreach( const Cfg::CheckBox & chk, form.radiobutton() )
		drawRadioButton( chk, p, dpi );

	foreach( const Cfg::ComboBox & cb, form.combobox() )
		drawComboBox( cb, p, dpi );

	foreach( const Cfg::SpinBox & s, form.spinbox() )
		drawSpinBox( s, p, dpi );

	foreach( const Cfg::HSlider & hs, form.hslider() )
		drawHSlider( hs, p, dpi );

	foreach( const Cfg::VSlider & vs, form.vslider() )
		drawVSlider( vs, p, dpi );

	foreach( const Cfg::Text & text, form.text() )
	{
		p.save();

		QTextDocument doc;
		doc.documentLayout()->setPaintDevice( &svg );
		doc.setTextWidth( MmPx::instance().fromMm( text.textWidth(), dpi ) );

		Cfg::fillTextDocument( &doc, text.text() );

		p.translate( MmPx::instance().fromMm( text.pos().x(), dpi ),
			MmPx::instance().fromMm( text.pos().y(), dpi ) );

		doc.drawContents( &p );

		p.restore();
	}

	foreach( const Cfg::Image & image, form.image() )
	{
		const QSize s( MmPx::instance().fromMm( image.size().width(), dpi ),
			MmPx::instance().fromMm( image.size().height(), dpi ) );

		const QByteArray data = QByteArray::fromBase64( image.data().toLatin1() );

		QImage img = QImage::fromData( data, "PNG" );

		p.drawImage( MmPx::instance().fromMm( image.pos().x(), dpi ),
			MmPx::instance().fromMm( image.pos().y(), dpi ),
			img.scaled( s, ( image.keepAspectRatio() ? Qt::KeepAspectRatio :
					Qt::IgnoreAspectRatio ),
				Qt::SmoothTransformation ) );
	}

	foreach( const Cfg::Rect & rect, form.rect() )
		drawRect( rect, p, dpi );

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
