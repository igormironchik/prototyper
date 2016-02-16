
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
#include "form_text.hpp"
#include "form_with_resize_and_move_handles.hpp"
#include "form_resizable.hpp"
#include "form_text_opts.hpp"
#include "utils.hpp"

// Qt include.
#include <QStyleOptionGraphicsItem>
#include <QPainter>
#include <QTextDocument>
#include <QTextCursor>
#include <QFontMetrics>
#include <QFont>


namespace Prototyper {

namespace Core {

//
// FormTextPrivate
//

class FormTextPrivate {
public:
	FormTextPrivate( const QRectF & rect, FormText * parent )
		:	q( parent )
		,	m_rect( rect )
		,	m_proxy( 0 )
		,	m_opts( 0 )
	{
	}

	//! Init.
	void init();
	//! Set rect.
	void setRect( const QRectF & rect );

	//! Parent.
	FormText * q;
	//! Rect.
	QRectF m_rect;
	//! Resizable proxy.
	FormResizableProxy * m_proxy;
	//! Text options.
	FormTextOpts * m_opts;
}; // class FormTextPrivate

void
FormTextPrivate::init()
{
	q->enableEditing( true );

	m_proxy = new FormResizableProxy( q, q->parentItem() );

	m_opts = new FormTextOpts( q->parentItem() );

	m_opts->setFocusProxy( q );

	m_opts->hide();

	setRect( m_rect );

	QFont f = q->font();

	f.setPointSize( 10.0 );

	q->setFont( f );

	q->document()->setDefaultFont( f );

	QTextCursor c = q->textCursor();

	QTextCharFormat fmt = c.charFormat();
	fmt.setFontPointSize( 10.0 );

	c.setCharFormat( fmt );

	q->setTextCursor( c );

	q->setPlainText( FormText::tr( "Text" ) );

	QFontMetrics m( f );
	m_proxy->setMinSize(
		QSizeF( m.boundingRect( QLatin1Char( 'a' ) ).size() ) );

	FormText::connect( q->document(), &QTextDocument::cursorPositionChanged,
		q, &FormText::p_cursorChanged );
	FormText::connect( m_opts, &FormTextOpts::lessFontSize,
		q, &FormText::lessFontSize );
	FormText::connect( m_opts, &FormTextOpts::moreFontSize,
		q, &FormText::moreFontSize );
	FormText::connect( m_opts, &FormTextOpts::bold,
		q, &FormText::bold );
	FormText::connect( m_opts, &FormTextOpts::italic,
		q, &FormText::italic );
	FormText::connect( m_opts, &FormTextOpts::underline,
		q, &FormText::underline );
	FormText::connect( m_opts, &FormTextOpts::textColor,
		q, &FormText::changeTextColor );
	FormText::connect( m_opts, &FormTextOpts::clearFormat,
		q, &FormText::clearFormat );
}

void
FormTextPrivate::setRect( const QRectF & rect )
{
	m_rect = rect;

	q->setPos( m_rect.topLeft() );

	q->setTextWidth( m_rect.width() );

	QRectF r = q->boundingRect();
	r.moveTo( q->pos() );

	m_proxy->setRect( r );
}


//
// FormText
//

FormText::FormText( const QRectF & rect, Form * form, QGraphicsItem * parent )
	:	QGraphicsTextItem( parent )
	,	FormObject( form )
	,	d( new FormTextPrivate( rect, this ) )
{
	d->init();
}

FormText::~FormText()
{
	delete d->m_proxy;
	delete d->m_opts;
}

Cfg::Text
FormText::cfg() const
{
	Cfg::Text c;

	c.setObjectId( objectId() );

	Cfg::Point p;
	p.setX( pos().x() );
	p.setY( pos().y() );

	c.setPos( p );

	c.setTextWidth( textWidth() );

	c.setText( Cfg::text( textCursor(), toPlainText() ) );

	return c;
}

void
FormText::setCfg( const Cfg::Text & c )
{
	setPlainText( QString() );

	QFont f = font();

	foreach( const Cfg::TextStyle & s, c.text() )
	{
		if( s.style().contains( Cfg::c_normalStyle ) )
		{
			f.setWeight( QFont::Normal );
			f.setItalic( false );
			f.setUnderline( false );

			setFont( f );
		}
		else
		{
			if( s.style().contains( Cfg::c_boldStyle ) )
				f.setWeight( QFont::Bold );
			else
				f.setWeight( QFont::Normal );

			if( s.style().contains( Cfg::c_italicStyle ) )
				f.setItalic( true );
			else
				f.setItalic( false );

			if( s.style().contains( Cfg::c_underlineStyle ) )
				f.setUnderline( true );
			else
				f.setUnderline( false );
		}

		f.setPointSize( s.fontSize() );

		setFont( f );

		QTextCursor cursor = textCursor();
		cursor.movePosition( QTextCursor::End );
		cursor.insertText( s.text() );
		setTextCursor( cursor );
	}

	setObjectId( c.objectId() );

	setTextWidth( c.textWidth() );

	setPos( QPointF( c.pos().x(), c.pos().y() ) );

	QRectF r = boundingRect();
	r.moveTo( pos() );

	d->m_proxy->setRect( r );
}

void
FormText::enableEditing( bool on )
{
	if( on )
		setTextInteractionFlags( Qt::TextEditorInteraction );
	else
		setTextInteractionFlags( Qt::NoTextInteraction );
}

void
FormText::clearSelection()
{
	QTextCursor c = textCursor();
	c.clearSelection();
	setTextCursor( c );
}

void
FormText::paint( QPainter * painter, const QStyleOptionGraphicsItem * option,
	QWidget * widget )
{
	QGraphicsTextItem::paint( painter, option, widget );

	if( isSelected() && !group() )
	{
		unsetCursor();

		d->m_proxy->show();

		d->m_opts->hide();
	}
	else
	{
		setCursor( Qt::IBeamCursor );

		if( !group() && hasFocus() )
		{
			d->m_opts->setPos( pos() +
				QPointF( 0.0, -d->m_opts->size().height() ) );

			d->m_opts->show();
		}
		else
			d->m_opts->hide();

		d->m_proxy->hide();
	}
}

void
FormText::lessFontSize()
{
	QTextCursor c = textCursor();

	if( c.hasSelection() )
	{
		if( c.position() != c.selectionEnd() )
			c.setPosition( c.selectionEnd() );

		QTextCharFormat fmt = c.charFormat();

		qreal s = fmt.fontPointSize();
		s -= 1.0;

		if( s < 5.0 )
			s = 5.0;

		fmt.setFontPointSize( s );

		textCursor().setCharFormat( fmt );
	}
	else
	{
		QFont f = font();

		qreal s = f.pointSize();
		s -= 1.0;

		if( s < 5.0 )
			s = 5.0;

		f.setPointSize( s );

		setFont( f );
	}
}

void
FormText::moreFontSize()
{
	QTextCursor c = textCursor();

	if( c.hasSelection() )
	{
		if( c.position() != c.selectionEnd() )
			c.setPosition( c.selectionEnd() );

		QTextCharFormat fmt = c.charFormat();

		qreal s = fmt.fontPointSize();

		s += 1.0;
		fmt.setFontPointSize( s );

		textCursor().setCharFormat( fmt );
	}
	else
	{
		QFont f = font();

		qreal s = f.pointSize();

		s += 1.0;

		f.setPointSize( s );

		setFont( f );
	}
}

void
FormText::bold( bool on )
{
	QTextCursor c = textCursor();

	if( c.hasSelection() )
	{
		if( c.position() != c.selectionEnd() )
			c.setPosition( c.selectionEnd() );

		QTextCharFormat fmt = c.charFormat();

		fmt.setFontWeight( on ? QFont::Bold : QFont::Normal );

		textCursor().setCharFormat( fmt );
	}
	else
	{
		QFont f = font();

		f.setWeight( on ? QFont::Bold : QFont::Normal );

		setFont( f );
	}
}

void
FormText::italic( bool on )
{
	QTextCursor c = textCursor();

	if( c.hasSelection() )
	{
		if( c.position() != c.selectionEnd() )
			c.setPosition( c.selectionEnd() );

		QTextCharFormat fmt = c.charFormat();

		fmt.setFontItalic( on );

		textCursor().setCharFormat( fmt );
	}
	else
	{
		QFont f = font();

		f.setItalic( on );

		setFont( f );
	}
}

void
FormText::underline( bool on )
{
	QTextCursor c = textCursor();

	if( c.hasSelection() )
	{
		if( c.position() != c.selectionEnd() )
			c.setPosition( c.selectionEnd() );

		QTextCharFormat fmt = c.charFormat();

		fmt.setFontUnderline( on );

		textCursor().setCharFormat( fmt );
	}
	else
	{
		QFont f = font();

		f.setUnderline( on );

		setFont( f );
	}
}

void
FormText::changeTextColor()
{
}

void
FormText::p_cursorChanged( const QTextCursor & cursor )
{
	d->m_opts->updateState( cursor );
}

void
FormText::clearFormat()
{
	QTextCursor c = textCursor();

	if( c.hasSelection() )
	{
		QTextCharFormat fmt = c.charFormat();

		fmt.setFontUnderline( false );
		fmt.setFontItalic( false );
		fmt.setFontWeight( QFont::Normal );
		fmt.setFontPointSize( 10 );

		textCursor().setCharFormat( fmt );
	}
	else
	{
		QFont f = font();
		f.setUnderline( false );
		f.setItalic( false );
		f.setWeight( QFont::Normal );
		f.setPointSize( 10.0 );

		setFont( f );
	}
}

void
FormText::resize( const QRectF & rect )
{
	d->setRect( rect );
}

void
FormText::moveResizable( const QPointF & delta )
{
	moveBy( delta.x(), delta.y() );
}

void
FormText::focusOutEvent( QFocusEvent * event )
{
	QTextCursor c = textCursor();
	c.clearSelection();
	setTextCursor( c );

	QGraphicsTextItem::focusOutEvent( event );
}

} /* namespace Core */

} /* namespace Prototyper */
