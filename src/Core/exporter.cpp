
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
#include "exporter_private.hpp"

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

void
ExporterPrivate::init()
{

}

namespace impl {

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

template< typename CFG >
void
draw( QPainter & p, QSvgGenerator & svg, const CFG & form, qreal dpi )
{
	std::list< qreal > z;

	foreach( const Cfg::Group & group, form.group() )
		z.push_back( qRound( group.z() ) );

	foreach( const Cfg::Line & line, form.line() )
		z.push_back( qRound( line.z() ) );

	foreach( const Cfg::Polyline & poly, form.polyline() )
		z.push_back( qRound( poly.z() ) );

	foreach( const Cfg::Text & text, form.text() )
		z.push_back( qRound( text.z() ) );

	foreach( const Cfg::Image & image, form.image() )
		z.push_back( qRound( image.z() ) );

	foreach( const Cfg::Rect & rect, form.rect() )
		z.push_back( qRound( rect.z() ) );

	foreach( const Cfg::Button & btn, form.button() )
		z.push_back( qRound( btn.z() ) );

	foreach( const Cfg::CheckBox & chk, form.checkbox() )
		z.push_back( qRound( chk.z() ) );

	foreach( const Cfg::CheckBox & chk, form.radiobutton() )
		z.push_back( qRound( chk.z() ) );

	foreach( const Cfg::ComboBox & cb, form.combobox() )
		z.push_back( qRound( cb.z() ) );

	foreach( const Cfg::SpinBox & s, form.spinbox() )
		z.push_back( qRound( s.z() ) );

	foreach( const Cfg::HSlider & hs, form.hslider() )
		z.push_back( qRound( hs.z() ) );

	foreach( const Cfg::VSlider & vs, form.vslider() )
		z.push_back( qRound( vs.z() ) );

	z.sort();
	auto last = std::unique( z.begin(), z.end() );
	z.erase( last, z.end() );

	while( !z.empty() )
	{
		const auto zv = z.front();

		foreach( const Cfg::Group & group, form.group() )
			if( qAbs( zv - group.z() ) < 0.5 )
				drawGroup( group, p, dpi, svg );

		foreach( const Cfg::Line & line, form.line() )
			if( qAbs( zv - line.z() ) < 0.5 )
				drawLine( line, p, dpi );

		foreach( const Cfg::Polyline & poly, form.polyline() )
			if( qAbs( zv - poly.z() ) < 0.5 )
				drawPolyline( poly, p, dpi );

		foreach( const Cfg::Text & text, form.text() )
		{
			if( qAbs( zv - text.z() ) < 0.5 )
			{
				p.save();

				QTextDocument doc;
				doc.documentLayout()->setPaintDevice( &svg );
				doc.setTextWidth( MmPx::instance().fromMm( text.textWidth(), dpi ) );

				Cfg::fillTextDocument( &doc, text.text(), dpi );

				p.translate( MmPx::instance().fromMm( text.pos().x(), dpi ),
					MmPx::instance().fromMm( text.pos().y(), dpi ) );

				doc.drawContents( &p );

				p.restore();
			}
		}

		foreach( const Cfg::Image & image, form.image() )
		{
			if( qAbs( zv - image.z() ) < 0.5 )
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
		}

		foreach( const Cfg::Rect & rect, form.rect() )
			if( qAbs( zv - rect.z() ) < 0.5 )
				drawRect( rect, p, dpi );

		foreach( const Cfg::Button & btn, form.button() )
			if( qAbs( zv - btn.z() ) < 0.5 )
				drawButton( btn, p, dpi );

		foreach( const Cfg::CheckBox & chk, form.checkbox() )
			if( qAbs( zv - chk.z() ) < 0.5 )
				drawCheckBox( chk, p, dpi );

		foreach( const Cfg::CheckBox & chk, form.radiobutton() )
			if( qAbs( zv - chk.z() ) < 0.5 )
				drawRadioButton( chk, p, dpi );

		foreach( const Cfg::ComboBox & cb, form.combobox() )
			if( qAbs( zv - cb.z() ) < 0.5 )
				drawComboBox( cb, p, dpi );

		foreach( const Cfg::SpinBox & s, form.spinbox() )
			if( qAbs( zv - s.z() ) < 0.5 )
				drawSpinBox( s, p, dpi );

		foreach( const Cfg::HSlider & hs, form.hslider() )
			if( qAbs( zv - hs.z() ) < 0.5 )
				drawHSlider( hs, p, dpi );

		foreach( const Cfg::VSlider & vs, form.vslider() )
			if( qAbs( zv - vs.z() ) < 0.5 )
				drawVSlider( vs, p, dpi );

		z.pop_front();
	}
}

void drawGroup( const Cfg::Group & group, QPainter & p, qreal dpi,
	QSvgGenerator & svg )
{
	p.save();

	p.translate( MmPx::instance().fromMm( group.pos().x(), dpi ),
		MmPx::instance().fromMm( group.pos().y(), dpi ) );

	draw( p, svg, group, dpi );

	p.restore();
}

} /* namespace impl */

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

	impl::draw( p, svg, form, dpi );

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
