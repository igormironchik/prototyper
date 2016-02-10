
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
#include "with_resize_and_move_handles.hpp"
#include "form_resizable.hpp"

// Qt include.
#include <QStyleOptionGraphicsItem>
#include <QPainter>
#include <QTextDocument>
#include <QTextCursor>
#include <QFontMetrics>


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
		,	m_proxy( new FormResizableProxy( q, q->parentItem() ) )
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
}; // class FormTextPrivate

void
FormTextPrivate::init()
{
	q->enableEditing( true );

	q->setPlainText( FormText::tr( "Text" ) );

	setRect( m_rect );

	QFontMetrics m( q->font() );
	m_proxy->setMinSize(
		QSizeF( m.boundingRect( QLatin1Char( 'a' ) ).size() ) );
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

FormText::FormText( const QRectF & rect, QGraphicsItem * parent )
	:	QGraphicsTextItem( parent )
	,	d( new FormTextPrivate( rect, this ) )
{
	d->init();
}

FormText::~FormText()
{
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
	}
	else
	{
		setCursor( Qt::IBeamCursor );

		d->m_proxy->hide();
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
