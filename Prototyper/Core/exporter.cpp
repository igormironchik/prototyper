
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
#include "form.hpp"

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

static inline void drawRect( const Cfg::Rect & rect, QPainter & p )
{
	p.save();

	const QRectF r( rect.topLeft().x(), rect.topLeft().y(),
		rect.size().width(), rect.size().height() );

	p.setPen( QPen( QColor( rect.pen().color() ), rect.pen().width() ) );

	p.setBrush( Qt::NoBrush );

	p.drawRect( r );

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

	foreach( const Cfg::Rect & rect, group.rect() )
		drawRect( rect, p );
}

static inline void drawButton( const Cfg::Button & btn, QPainter & p )
{
	p.save();

	p.setPen( QPen( QColor( btn.pen().color() ), btn.pen().width() ) );

	QFont f = p.font();

	if( btn.text().style().contains( Cfg::c_boldStyle ) )
		f.setBold( true );

	if( btn.text().style().contains( Cfg::c_italicStyle ) )
		f.setItalic( true );

	if( btn.text().style().contains( Cfg::c_underlineStyle ) )
		f.setUnderline( true );

	f.setPointSize( btn.text().fontSize() );

	p.setFont( f );

	const QRect r( btn.pos().x(), btn.pos().y(),
		btn.size().width(), btn.size().height() );

	p.drawRect( r );

	p.drawText( r, Qt::AlignCenter, btn.text().text() );

	p.restore();
}

void
ExporterPrivate::drawForm( QSvgGenerator & svg, const Cfg::Form & form )
{
	const int wh = ( form.windowButtons().isEmpty() ? 0 : 30 );

	svg.setViewBox( QRect( 0, 0,
		form.size().width(), form.size().height() + wh ) );

	QPainter p;
	p.begin( &svg );

	p.setPen( Qt::gray );

	FormProperties::Buttons btns;

	if( form.windowButtons().contains( Cfg::c_maximize ) )
		btns |= FormProperties::MaximizeButton;

	if( form.windowButtons().contains( Cfg::c_minimize ) )
		btns |= FormProperties::MinimizeButton;

	if( form.windowButtons().contains( Cfg::c_close ) )
		btns |= FormProperties::CloseButton;

	Form::draw( &p, form.size().width(), form.size().height(), btns, 0, false );

	foreach( const Cfg::Group & group, form.group() )
		drawGroup( group, p );

	foreach( const Cfg::Line & line, form.line() )
		drawLine( line, p );

	foreach( const Cfg::Polyline & poly, form.polyline() )
	{
		foreach( const Cfg::Line & line, poly.line() )
			drawLine( line, p );
	}

	foreach( const Cfg::Button & btn, form.button() )
		drawButton( btn, p );

	foreach( const Cfg::CheckBox & chk, form.checkbox() )
	{
		p.save();

		QFont f = p.font();

		if( chk.text().style().contains( Cfg::c_boldStyle ) )
			f.setBold( true );

		if( chk.text().style().contains( Cfg::c_italicStyle ) )
			f.setItalic( true );

		if( chk.text().style().contains( Cfg::c_underlineStyle ) )
			f.setUnderline( true );

		f.setPointSize( chk.text().fontSize() );

		FormCheckBox::draw( &p, Cfg::fromPen( chk.pen() ), f,
			QRectF( chk.pos().x(), chk.pos().y(),
				chk.size().width(), chk.size().height() ),
			chk.width(),
			chk.isChecked(),
			chk.text().text() );

		p.restore();
	}

	foreach( const Cfg::CheckBox & chk, form.radiobutton() )
	{
		p.save();

		QFont f = p.font();

		if( chk.text().style().contains( Cfg::c_boldStyle ) )
			f.setBold( true );

		if( chk.text().style().contains( Cfg::c_italicStyle ) )
			f.setItalic( true );

		if( chk.text().style().contains( Cfg::c_underlineStyle ) )
			f.setUnderline( true );

		f.setPointSize( chk.text().fontSize() );

		FormRadioButton::draw( &p, Cfg::fromPen( chk.pen() ), f,
			QRectF( chk.pos().x(), chk.pos().y(),
				chk.size().width(), chk.size().height() ),
			chk.width(),
			chk.isChecked(),
			chk.text().text() );

		p.restore();
	}

	foreach( const Cfg::ComboBox & cb, form.combobox() )
	{
		p.save();

		FormComboBox::draw( &p,
			QRectF( cb.pos().x(), cb.pos().y(),
				cb.size().width(), cb.size().height() ),
			Cfg::fromPen( cb.pen() ) );

		p.restore();
	}

	foreach( const Cfg::SpinBox & s, form.spinbox() )
	{
		p.save();

		QFont f = p.font();

		if( s.text().style().contains( Cfg::c_boldStyle ) )
			f.setBold( true );

		if( s.text().style().contains( Cfg::c_italicStyle ) )
			f.setItalic( true );

		if( s.text().style().contains( Cfg::c_underlineStyle ) )
			f.setUnderline( true );

		f.setPointSize( s.text().fontSize() );

		FormSpinBox::draw( &p,
			QRectF( s.pos().x(), s.pos().y(),
				s.size().width(), s.size().height() ),
			Cfg::fromPen( s.pen() ), f,
			s.text().text() );

		p.restore();
	}

	foreach( const Cfg::HSlider & hs, form.hslider() )
	{
		p.save();

		FormHSlider::draw( &p,
			QRectF( hs.pos().x(), hs.pos().y(),
				hs.size().width(), hs.size().height() ),
			Cfg::fromPen( hs.pen() ) );

		p.restore();
	}

	foreach( const Cfg::VSlider & vs, form.vslider() )
	{
		p.save();

		FormVSlider::draw( &p,
			QRectF( vs.pos().x(), vs.pos().y(),
				vs.size().width(), vs.size().height() ),
			Cfg::fromPen( vs.pen() ) );

		p.restore();
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

	foreach( const Cfg::Rect & rect, form.rect() )
		drawRect( rect, p );

	p.end();
}

static inline void findLinksInGroup( QMap< QString, QString > & lnks,
	const Cfg::Group & group )
{
	if( !group.link().isEmpty() )
		lnks.insert( group.objectId(), group.link() );

	foreach( const Cfg::Group & g, group.group() )
		findLinksInGroup( lnks, g );

	foreach( const Cfg::Line & line, group.line() )
		if( !line.link().isEmpty() )
			lnks.insert( line.objectId(), line.link() );

	foreach( const Cfg::Polyline & poly, group.polyline() )
		if( !poly.link().isEmpty() )
			lnks.insert( poly.objectId(), poly.link() );

	foreach( const Cfg::Text & text, group.text() )
		if( !text.link().isEmpty() )
			lnks.insert( text.objectId(), text.link() );

	foreach( const Cfg::Image & image, group.image() )
		if( !image.link().isEmpty() )
			lnks.insert( image.objectId(), image.link() );

	foreach( const Cfg::Rect & rect, group.rect() )
		if( !rect.link().isEmpty() )
			lnks.insert( rect.objectId(), rect.link() );
}

QMap< QString, QString >
ExporterPrivate::links( const Cfg::Form & form )
{
	QMap< QString, QString > lnks;

	if( !form.link().isEmpty() )
		lnks.insert( form.tabName(), form.link() );

	foreach( const Cfg::Group & g, form.group() )
		findLinksInGroup( lnks, g );

	foreach( const Cfg::Line & line, form.line() )
		if( !line.link().isEmpty() )
			lnks.insert( line.objectId(), line.link() );

	foreach( const Cfg::Polyline & poly, form.polyline() )
		if( !poly.link().isEmpty() )
			lnks.insert( poly.objectId(), poly.link() );

	foreach( const Cfg::Text & text, form.text() )
		if( !text.link().isEmpty() )
			lnks.insert( text.objectId(), text.link() );

	foreach( const Cfg::Image & image, form.image() )
		if( !image.link().isEmpty() )
			lnks.insert( image.objectId(), image.link() );

	foreach( const Cfg::Rect & rect, form.rect() )
		if( !rect.link().isEmpty() )
			lnks.insert( rect.objectId(), rect.link() );

	return lnks;
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
