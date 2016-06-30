
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
#include "form_button.hpp"
#include "utils.hpp"
#include "form_button_properties.hpp"
#include "form.hpp"
#include "form_undo_commands.hpp"

// Qt include.
#include <QFontMetrics>
#include <QFont>
#include <QPainter>
#include <QApplication>
#include <QGraphicsSceneContextMenuEvent>
#include <QMenu>
#include <QAction>
#include <QUndoStack>


namespace Prototyper {

namespace Core {

//
// FormButtonPrivate
//

class FormButtonPrivate {
public:
	FormButtonPrivate( FormButton * parent, const QRectF & rect )
		:	q( parent )
		,	m_rect( rect )
		,	m_proxy( 0 )
		,	m_text( QLatin1String( "OK" ) )
	{
	}

	//! Init.
	void init();
	//! Set rect.
	void setRect( const QRectF & rect );

	//! Parent.
	FormButton * q;
	//! Rect.
	QRectF m_rect;
	//! Resizable proxy.
	QScopedPointer< FormResizableProxy > m_proxy;
	//! Text.
	QString m_text;
	//! Font.
	QFont m_font;
}; // class FormButtonPrivate

void
FormButtonPrivate::init()
{
	m_proxy.reset( new FormResizableProxy( q, q->parentItem(), q->form() ) );

	setRect( m_rect );

	m_font = QApplication::font();

	m_font.setPointSize( 10.0 );

	QFontMetrics m( m_font );
	m_proxy->setMinSize(
		QSizeF( m.boundingRect( QLatin1Char( 'a' ) ).size() ) );
}

void
FormButtonPrivate::setRect( const QRectF & rect )
{
	m_rect = rect;

	q->setPos( m_rect.topLeft() );

	m_proxy->setRect( m_rect );

	m_rect.moveTopLeft( QPointF( 0.0, 0.0 ) );
}


//
// FormButton
//

FormButton::FormButton( const QRectF & rect, Form * form,
	QGraphicsItem * parent )
	:	QGraphicsObject( parent )
	,	FormObject( FormObject::ButtonType, form )
	,	d( new FormButtonPrivate( this, rect ) )
{
	d->init();
}

FormButton::~FormButton()
{
}

void
FormButton::paint( QPainter * painter, const QStyleOptionGraphicsItem * option,
	QWidget * widget )
{
	Q_UNUSED( widget )
	Q_UNUSED( option )

	painter->setPen( objectPen() );
	painter->setFont( d->m_font );

	painter->drawRect( d->m_rect );
	painter->drawText( d->m_rect, Qt::AlignCenter, d->m_text );

	if( isSelected() && !group() )
		d->m_proxy->show();
	else
		d->m_proxy->hide();
}

void
FormButton::setObjectPen( const QPen & p, bool pushUndoCommand )
{
	FormObject::setObjectPen( p, pushUndoCommand );

	update();
}

Cfg::Button
FormButton::cfg() const
{
	Cfg::Button c;

	c.setObjectId( objectId() );

	Cfg::Point p;
	p.setX( pos().x() );
	p.setY( pos().y() );

	c.setPos( p );

	Cfg::Size s;
	s.setWidth( d->m_rect.width() );
	s.setHeight( d->m_rect.height() );

	c.setSize( s );

	c.setText( text() );

	c.setPen( Cfg::pen( objectPen() ) );

	c.setLink( link() );

	return c;
}

void
FormButton::setCfg( const Cfg::Button & c )
{
	setObjectId( c.objectId() );
	setLink( c.link() );
	setObjectPen( Cfg::fromPen( c.pen() ), false );
	d->setRect( QRectF( c.pos().x(), c.pos().y(),
		c.size().width(), c.size().height() ) );

	setText( c.text() );

	update();
}

Cfg::TextStyle
FormButton::text() const
{
	Cfg::TextStyle textStyle = Cfg::textStyleFromFont( d->m_font );
	textStyle.style().append( Cfg::c_center );
	textStyle.setText( d->m_text );

	return textStyle;
}

void
FormButton::setText( const Cfg::TextStyle & c )
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
FormButton::boundingRect() const
{
	return d->m_rect;
}

void
FormButton::setPosition( const QPointF & pos, bool pushUndoCommand )
{
	if( pushUndoCommand )
		form()->undoStack()->push( new UndoMove< FormButton > ( form(),
			objectId(), pos - position() ) );

	QRectF r = boundingRect();
	r.moveTopLeft( pos );

	d->setRect( r );
}

QPointF
FormButton::position() const
{
	return pos();
}

QRectF
FormButton::rectangle() const
{
	QRectF r = boundingRect();
	r.moveTopLeft( position() );

	return r;
}

void
FormButton::setRectangle( const QRectF & rect, bool pushUndoCommand )
{
	Q_UNUSED( pushUndoCommand )

	resize( rect );
}

void
FormButton::resize( const QRectF & rect )
{
	d->setRect( rect );

	form()->update();
}

void
FormButton::moveResizable( const QPointF & delta )
{
	moveBy( delta.x(), delta.y() );
}

void
FormButton::contextMenuEvent( QGraphicsSceneContextMenuEvent * event )
{
	QMenu menu;

	menu.addAction( QIcon( ":/Core/img/configure.png" ),
		tr( "Properties" ), this, &FormButton::properties );

	menu.exec( event->screenPos() );
}

void
FormButton::properties()
{
	ButtonProperties dlg;

	dlg.setCfg( text() );

	if( dlg.exec() == QDialog::Accepted )
	{
		setText( dlg.cfg() );

		update();
	}
}

} /* namespace Core */

} /* namespace Prototyper */
