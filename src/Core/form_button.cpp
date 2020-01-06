
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
#include "page.hpp"
#include "form_undo_commands.hpp"
#include "form_actions.hpp"

// Qt include.
#include <QFontMetrics>
#include <QFont>
#include <QPainter>
#include <QApplication>
#include <QGraphicsSceneContextMenuEvent>
#include <QMenu>
#include <QAction>
#include <QUndoStack>
#include <QGraphicsScene>


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

	m_font.setPixelSize( MmPx::instance().fromPtY( 10.0 ) );

	QFontMetrics m( m_font );
	m_proxy->setMinSize(
		QSizeF( m.boundingRect( QLatin1Char( 'a' ) ).size() ) );

	q->setObjectPen( QPen( FormAction::instance()->strokeColor() ),
		false );

	q->setObjectBrush( Qt::transparent );
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

FormButton::FormButton( const QRectF & rect, Page * form,
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

	c.set_objectId( objectId() );

	Cfg::Point p;
	p.set_x( MmPx::instance().toMmX( pos().x() ) );
	p.set_y( MmPx::instance().toMmY( pos().y() ) );

	c.set_pos( p );

	Cfg::Size s;
	s.set_width( MmPx::instance().toMmX( d->m_rect.width() ) );
	s.set_height( MmPx::instance().toMmY( d->m_rect.height() ) );

	c.set_size( s );

	c.set_text( text() );

	c.set_pen( Cfg::pen( objectPen() ) );

	c.set_z( zValue() );

	return c;
}

void
FormButton::setCfg( const Cfg::Button & c )
{
	setObjectId( c.objectId() );
	setObjectPen( Cfg::fromPen( c.pen() ), false );
	d->setRect( QRectF( MmPx::instance().fromMmX( c.pos().x() ),
		MmPx::instance().fromMmY( c.pos().y() ),
		MmPx::instance().fromMmX( c.size().width() ),
		MmPx::instance().fromMmY( c.size().height() ) ) );

	setText( c.text() );

	setZValue( c.z() );

	update();
}

Cfg::TextStyle
FormButton::text() const
{
	Cfg::TextStyle textStyle = Cfg::textStyleFromFont( d->m_font );
	textStyle.style().push_back( Cfg::c_center );
	textStyle.set_text( d->m_text );

	return textStyle;
}

void
FormButton::setText( const Cfg::TextStyle & c )
{
	if( std::find( c.style().cbegin(), c.style().cend(),
		Cfg::c_normalStyle ) != c.style().cend() )
	{
		d->m_font.setWeight( QFont::Normal );
		d->m_font.setItalic( false );
		d->m_font.setUnderline( false );
	}
	else
	{
		if( std::find( c.style().cbegin(), c.style().cend(),
			Cfg::c_boldStyle ) != c.style().cend() )
				d->m_font.setWeight( QFont::Bold );
		else
			d->m_font.setWeight( QFont::Normal );

		if( std::find( c.style().cbegin(), c.style().cend(),
			Cfg::c_italicStyle ) != c.style().cend() )
				d->m_font.setItalic( true );
		else
			d->m_font.setItalic( false );

		if( std::find( c.style().cbegin(), c.style().cend(),
			Cfg::c_underlineStyle ) != c.style().cend() )
				d->m_font.setUnderline( true );
		else
			d->m_font.setUnderline( false );
	}

	d->m_font.setPixelSize( MmPx::instance().fromPtY( c.fontSize() ) );

	d->m_text = c.text();

	update();
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
		form()->undoStack()->push( new UndoMove( form(),
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

	scene()->update();
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
		form()->undoStack()->push( new UndoChangeTextWithOpts( form(),
			objectId(), text(), dlg.cfg() ) );

		setText( dlg.cfg() );

		update();
	}
}

QSizeF
FormButton::defaultSize() const
{
	return QSizeF( MmPx::instance().fromMmX( 10.0 ), MmPx::instance().fromMmY( 5.0 ) );
}

} /* namespace Core */

} /* namespace Prototyper */
