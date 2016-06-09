
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

// Qt include.
#include <QTextDocument>
#include <QTextCursor>
#include <QFontMetrics>
#include <QFont>
#include <QPainter>
#include <QTextBlockFormat>


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
	FormResizableProxy * m_proxy;
}; // class FormButtonPrivate

void
FormButtonPrivate::init()
{
	m_proxy = new FormResizableProxy( q, q->parentItem(), q->form());

	setRect( m_rect );

	QFont f = q->font();

	f.setPointSize( 10.0 );

	q->setFont( f );

	q->document()->setDefaultFont( f );

	QTextCursor c = q->textCursor();

	QTextCharFormat fmt = c.charFormat();
	fmt.setFontPointSize( 10.0 );

	c.setCharFormat( fmt );

	QTextBlockFormat b = c.blockFormat();
	b.setAlignment( Qt::AlignCenter );
	c.setBlockFormat( b );

	q->setTextCursor( c );

	q->setPlainText( FormButton::tr( "OK" ) );

	QFontMetrics m( f );
	m_proxy->setMinSize(
		QSizeF( m.boundingRect( QLatin1Char( 'a' ) ).size() ) );
}

void
FormButtonPrivate::setRect( const QRectF & rect )
{
	m_rect = rect;

	q->setPos( m_rect.topLeft() );

	q->setTextWidth( m_rect.width() );

	QRectF r = q->boundingRect();
	r.moveTo( q->pos() );

	m_proxy->setRect( r );
}


//
// FormButton
//

FormButton::FormButton( const QRectF & rect, Form * form,
	QGraphicsItem * parent )
	:	QGraphicsTextItem( parent )
	,	FormObject( FormObject::ButtonType, form )
	,	d( new FormButtonPrivate( this, rect ) )
{
	d->init();
}

FormButton::~FormButton()
{
}

void
FormButton::destroyHandles()
{
	delete d->m_proxy;
}

void
FormButton::paint( QPainter * painter, const QStyleOptionGraphicsItem * option,
	QWidget * widget )
{
	QGraphicsTextItem::paint( painter, option, widget );

	painter->setPen( objectPen() );

	painter->drawRect( d->m_rect );

	if( isSelected() && !group() )
		d->m_proxy->show();
	else
		d->m_proxy->hide();
}

void
FormButton::setObjectPen( const QPen & p )
{
	FormObject::setObjectPen( p );

	setDefaultTextColor( p.color() );
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

	c.setText( Cfg::text( textCursor(), toPlainText() ).first() );

	c.setPen( Cfg::pen( objectPen() ) );

	c.setLink( link() );

	return c;
}

void
FormButton::setCfg( const Cfg::Button & c )
{
	setObjectId( c.objectId() );
	setLink( c.link() );
	setObjectPen( Cfg::fromPen( c.pen() ) );
	d->setRect( QRectF( c.pos().x(), c.pos().y(),
		c.size().width(), c.size().height() ) );
	setPlainText( QString() );

	QFont f = font();
	QTextCharFormat fmt = textCursor().charFormat();
	QTextBlockFormat b = textCursor().blockFormat();

	if( c.text().style().contains( Cfg::c_normalStyle ) )
	{
		f.setWeight( QFont::Normal );
		f.setItalic( false );
		f.setUnderline( false );

		fmt.setFontWeight( QFont::Normal );
		fmt.setFontItalic( false );
		fmt.setFontUnderline( false );
	}
	else
	{
		if( c.text().style().contains( Cfg::c_boldStyle ) )
		{
			f.setWeight( QFont::Bold );
			fmt.setFontWeight( QFont::Bold );
		}
		else
		{
			f.setWeight( QFont::Normal );
			fmt.setFontWeight( QFont::Normal );
		}

		if( c.text().style().contains( Cfg::c_italicStyle ) )
		{
			f.setItalic( true );
			fmt.setFontItalic( true );
		}
		else
		{
			f.setItalic( false );
			fmt.setFontItalic( false );
		}

		if( c.text().style().contains( Cfg::c_underlineStyle ) )
		{
			f.setUnderline( true );
			fmt.setFontUnderline( true );
		}
		else
		{
			f.setUnderline( false );
			fmt.setFontUnderline( false );
		}
	}

	Cfg::initBlockFormat( b, c.text() );

	f.setPointSize( c.text().fontSize() );

	fmt.setFontPointSize( c.text().fontSize() );

	setFont( f );

	QTextCursor cursor = textCursor();
	cursor.movePosition( QTextCursor::End );
	cursor.setCharFormat( fmt );
	cursor.setBlockFormat( b );
	cursor.insertText( c.text().text() );
	setTextCursor( cursor );
}

void
FormButton::resize( const QRectF & rect )
{
	d->setRect( rect );
}

void
FormButton::moveResizable( const QPointF & delta )
{
	moveBy( delta.x(), delta.y() );
}

} /* namespace Core */

} /* namespace Prototyper */
