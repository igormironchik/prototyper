
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
#include "form_checkbox.hpp"
#include "utils.hpp"
#include "form_image_handles.hpp"
#include "form_checkbox_properties.hpp"
#include "form.hpp"

// Qt include.
#include <QPainter>
#include <QApplication>
#include <QMenu>
#include <QAction>
#include <QGraphicsSceneContextMenuEvent>


namespace Prototyper {

namespace Core {

//
// FormCheckBoxPrivate
//

FormCheckBoxPrivate::FormCheckBoxPrivate( FormCheckBox * parent,
	const QRectF & rect, qreal defaultSize )
	:	q( parent )
	,	m_rect( QRectF( rect.topLeft().x(), rect.topLeft().y(),
			rect.width(), defaultSize ) )
	,	m_checked( true )
	,	m_handles( 0 )
	,	m_text( FormCheckBox::tr( "Text" ) )
	,	m_width( rect.width() )
{
}

FormCheckBoxPrivate::~FormCheckBoxPrivate()
{
}

void
FormCheckBoxPrivate::init()
{
	m_handles.reset( new FormImageHandles( q, q->parentItem(), q->form() ) );

	m_handles->setMinSize( QSizeF( 20.0, 20.0 ) );

	m_handles->hide();

	setRect( m_rect );
}

void
FormCheckBoxPrivate::setRect( const QRectF & rect )
{
	const QRectF r = rect;

	m_rect = QRectF( 0.0, 0.0, r.height(), r.height() );

	m_width = r.width();

	q->setPos( r.topLeft() );

	QRectF hr = q->boundingRect();
	hr.moveTopLeft( r.topLeft() );

	m_handles->setRect( hr );
}


//
// FormCheckBox
//

FormCheckBox::FormCheckBox( const QRectF & rect, Form * form,
	QGraphicsItem * parent )
	:	QGraphicsObject( parent )
	,	FormObject( FormObject::CheckBoxType, form )
	,	d( new FormCheckBoxPrivate( this, rect ) )
{
	d->init();
}

FormCheckBox::FormCheckBox( const QRectF & rect, Form * form, qreal defaultSize,
	FormObject::ObjectType type, QGraphicsItem * parent )
	:	QGraphicsObject( parent )
	,	FormObject( type, form )
	,	d( new FormCheckBoxPrivate( this, rect, defaultSize ) )
{
	d->init();
}

FormCheckBox::~FormCheckBox()
{
}

void
FormCheckBox::paint( QPainter * painter, const QStyleOptionGraphicsItem * option,
	QWidget * widget )
{
	Q_UNUSED( option )
	Q_UNUSED( widget )

	draw( painter,
		objectPen(),
		d->m_font,
		QRectF( 0.0, 0.0, d->m_rect.width(), d->m_rect.height() ),
		d->m_width,
		d->m_checked,
		d->m_text );

	if( isSelected() && !group() )
		d->m_handles->show();
	else
		d->m_handles->hide();
}

void
FormCheckBox::draw( QPainter * painter, const QPen & pen, const QFont & font,
	const QRectF & rect, qreal width, bool isChecked, const QString & text )
{
	painter->setPen( pen );

	painter->drawRect( rect );

	if( isChecked )
	{
		painter->drawLine( QLineF( rect.topLeft().x() + 4.0,
			rect.topLeft().y() + rect.height() / 2.0,
			rect.topLeft().x() + rect.width() / 2.0,
			rect.bottomLeft().y() - 4.0 ) );

		painter->drawLine( QLineF(
			rect.topLeft().x() + rect.width() / 2.0,
			rect.bottomLeft().y() - 4.0,
			rect.topRight().x() - 4.0,
			rect.topRight().y() + 4.0 ) );
	}

	painter->setFont( font );

	QRectF r = rect;

	r.setRight( rect.x() + width );
	r.moveLeft( rect.x() + rect.height() + 4.0 );

	painter->drawText( r, Qt::AlignLeft | Qt::AlignVCenter, text );
}

void
FormCheckBox::setObjectPen( const QPen & p )
{
	FormObject::setObjectPen( p );

	update();
}

Cfg::CheckBox
FormCheckBox::cfg() const
{
	Cfg::CheckBox c;

	c.setWidth( d->m_width );

	c.setIsChecked( d->m_checked );
	c.setObjectId( objectId() );
	c.setLink( link() );
	c.setPen( Cfg::pen( objectPen() ) );

	Cfg::Point p;
	p.setX( pos().x() );
	p.setY( pos().y() );

	c.setPos( p );

	Cfg::Size s;
	s.setWidth( d->m_rect.width() );
	s.setHeight( d->m_rect.height() );

	c.setSize( s );

	c.setText( text() );

	return c;
}

void
FormCheckBox::setCfg( const Cfg::CheckBox & c )
{
	d->m_checked = c.isChecked();

	setObjectId( c.objectId() );
	setLink( c.link() );
	setObjectPen( Cfg::fromPen( c.pen() ) );

	d->setRect( QRectF( c.pos().x(), c.pos().y(),
		c.width(), c.size().height() ) );

	setText( c.text() );

	update();
}

Cfg::TextStyle
FormCheckBox::text() const
{
	Cfg::TextStyle textStyle = Cfg::textStyleFromFont( d->m_font );
	textStyle.style().append( Cfg::c_left );
	textStyle.setText( d->m_text );

	return textStyle;
}

void
FormCheckBox::setText( const Cfg::TextStyle & c )
{
	if( c.style().contains( Cfg::c_normalStyle ) )
	{
		d->m_font.setWeight( QFont::Normal );
		d->m_font.setItalic( false );
		d->m_font.setUnderline( false );
	}
	else
	{
		if( c.style().contains( Cfg::c_boldStyle ) )
			d->m_font.setWeight( QFont::Bold );
		else
			d->m_font.setWeight( QFont::Normal );

		if( c.style().contains( Cfg::c_italicStyle ) )
			d->m_font.setItalic( true );
		else
			d->m_font.setItalic( false );

		if( c.style().contains( Cfg::c_underlineStyle ) )
			d->m_font.setUnderline( true );
		else
			d->m_font.setUnderline( false );
	}

	d->m_font.setPointSize( c.fontSize() );

	d->m_text = c.text();
}

QRectF
FormCheckBox::boundingRect() const
{
	return QRectF( d->m_rect.topLeft().x(), d->m_rect.topLeft().y(),
		d->m_width, d->m_rect.height() );
}

void
FormCheckBox::positionElements( const QPointF & pos )
{
	QRectF r = boundingRect();
	r.moveTopLeft( pos );

	d->setRect( r );
}

QPointF
FormCheckBox::position() const
{
	return pos();
}

void
FormCheckBox::resize( const QRectF & rect )
{
	setPos( rect.topLeft() );

	QSizeF s = d->m_rect.size().scaled( rect.width(), rect.height(),
		( d->m_handles->isKeepAspectRatio() ? Qt::KeepAspectRatio :
			Qt::IgnoreAspectRatio ) );

	d->m_rect = QRectF( 0.0, 0.0, s.width(), s.height() );

	d->m_width = rect.width();

	QRectF r = boundingRect();
	r.moveTopLeft( rect.topLeft() );

	d->m_handles->setRect( r );

	update();

	form()->update();
}

void
FormCheckBox::moveResizable( const QPointF & delta )
{
	moveBy( delta.x(), delta.y() );
}

void
FormCheckBox::contextMenuEvent( QGraphicsSceneContextMenuEvent * event )
{
	QMenu menu;

	menu.addAction( QIcon( ":/Core/img/configure.png" ),
		tr( "Properties" ), this, &FormCheckBox::properties );

	menu.exec( event->screenPos() );
}

void
FormCheckBox::properties()
{
	CheckBoxProperties dlg;

	dlg.setCfg( cfg() );

	if( dlg.exec() == QDialog::Accepted )
	{
		Cfg::CheckBox c = dlg.cfg();

		setText( c.text() );

		d->m_checked = c.isChecked();

		update();
	}
}

} /* namespace Core */

} /* namespace Prototyper */
