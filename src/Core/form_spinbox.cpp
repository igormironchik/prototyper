
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
#include "form_spinbox.hpp"
#include "utils.hpp"
#include "page.hpp"
#include "form_spinbox_properties.hpp"
#include "form_undo_commands.hpp"

// Qt include.
#include <QPainter>
#include <QPainterPath>
#include <QApplication>
#include <QGraphicsSceneContextMenuEvent>
#include <QMenu>
#include <QAction>
#include <QUndoStack>
#include <QGraphicsScene>


namespace Prototyper {

namespace Core {

//
// FormSpinBoxPrivate
//

class FormSpinBoxPrivate {
public:
	FormSpinBoxPrivate( FormSpinBox * parent, const QRectF & rect )
		:	q( parent )
		,	m_rect( QRectF( rect.x(), rect.y(), rect.width(), 25.0 ) )
		,	m_proxy( 0 )
		,	m_text( FormSpinBox::tr( "1" ) )
	{
	}

	//! Init.
	void init();
	//! Set rect.
	void setRect( const QRectF & rect );

	//! Parent.
	FormSpinBox * q;
	//! Rect.
	QRectF m_rect;
	//! Resizable proxy.
	QScopedPointer< FormResizableProxy > m_proxy;
	//! Font.
	QFont m_font;
	//! Text.
	QString m_text;
}; // class FormSpinBoxPrivate

void
FormSpinBoxPrivate::init()
{
	m_proxy.reset( new FormResizableProxy( q, q->parentItem(), q->form() ) );

	setRect( m_rect );

	m_proxy->setMinSize( q->defaultSize() );

	m_font = QApplication::font();

	m_font.setPointSize( 14.0 );
}

void
FormSpinBoxPrivate::setRect( const QRectF & rect )
{
	m_rect = rect;

	q->setPos( m_rect.topLeft() );

	m_proxy->setRect( m_rect );

	m_rect.moveTopLeft( QPointF( 0.0, 0.0 ) );
}


//
// FormSpinBox
//

FormSpinBox::FormSpinBox( const QRectF & rect, Page * form,
	QGraphicsItem * parent )
	:	QGraphicsObject( parent )
	,	FormObject( FormObject::SpinBoxType, form )
	,	d( new FormSpinBoxPrivate( this, rect ) )
{
	d->init();
}

FormSpinBox::~FormSpinBox()
{
}

void
FormSpinBox::paint( QPainter * painter, const QStyleOptionGraphicsItem * option,
	QWidget * widget )
{
	Q_UNUSED( widget )
	Q_UNUSED( option )

	draw( painter, d->m_rect, objectPen(), d->m_font, d->m_text );

	if( isSelected() && !group() )
		d->m_proxy->show();
	else
		d->m_proxy->hide();
}

void
FormSpinBox::draw( QPainter * painter, const QRectF & rect,
	const QPen & pen, const QFont & font, const QString & text, int dpi )
{
	painter->setPen( pen );

	QRectF r = rect;

	if( r.height() > boxHeight( dpi ) )
	{
		r.setHeight( boxHeight( dpi ) );
		r.moveTopLeft( QPointF( rect.topLeft().x(), rect.topLeft().y() +
			( rect.height() - boxHeight( dpi ) ) / 2.0 ) );
	}

	painter->drawRoundedRect( r, 2.0, 2.0 );

	const qreal h = r.height();
	const qreal w = h * 0.75;
	const qreal leftX = r.x() + r.width() - w;

	painter->drawLine( QLineF( leftX, r.y(), leftX, r.y() + h ) );

	QPainterPath top;
	top.moveTo( leftX + 5.0, r.y() + r.height() / 2.0 - 2.5 );
	top.lineTo( leftX + w - 5.0, r.y() + r.height() / 2.0 - 2.5 );
	top.lineTo( leftX + w / 2.0, r.y() + 5.0 );

	painter->setBrush( QBrush( pen.color() ) );

	painter->drawPath( top );

	QPainterPath bottom;
	bottom.moveTo( leftX + 5.0, r.y() + r.height() / 2.0 + 2.5 );
	bottom.lineTo( leftX + w - 5.0, r.y() + r.height() / 2.0 + 2.5 );
	bottom.lineTo( leftX + w / 2.0, r.y() + h - 5.0 );

	painter->drawPath( bottom );

	QRectF textR = r;
	textR.setRight( leftX - 5.0 );

	painter->setFont( font );

	painter->drawText( textR, Qt::AlignRight | Qt::AlignVCenter,
		text );
}

qreal
FormSpinBox::boxHeight( int dpi )
{
	if( !dpi )
		return MmPx::instance().fromMmY( 4.0 );
	else
		return MmPx::instance().fromMm( 4.0, dpi );
}

void
FormSpinBox::setObjectPen( const QPen & p, bool pushUndoCommand )
{
	FormObject::setObjectPen( p, pushUndoCommand );

	update();
}

Cfg::SpinBox
FormSpinBox::cfg() const
{
	Cfg::SpinBox c;

	c.set_objectId( objectId() );

	Cfg::Point p;
	p.set_x( MmPx::instance().toMmX( pos().x() ) );
	p.set_y( MmPx::instance().toMmY( pos().y() ) );

	c.set_pos( p );

	Cfg::Size s;
	s.set_width( MmPx::instance().toMmX( d->m_rect.width() ) );
	s.set_height( MmPx::instance().toMmY( d->m_rect.height() ) );

	c.set_size( s );

	c.set_pen( Cfg::pen( objectPen() ) );

	c.set_text( text() );

	return c;
}

void
FormSpinBox::setCfg( const Cfg::SpinBox & c )
{
	setObjectId( c.objectId() );
	setObjectPen( Cfg::fromPen( c.pen() ), false );
	d->setRect( QRectF( MmPx::instance().fromMmX( c.pos().x() ),
		MmPx::instance().fromMmY( c.pos().y() ),
		MmPx::instance().fromMmX( c.size().width() ),
		MmPx::instance().fromMmY( c.size().height() ) ) );

	setText( c.text() );

	update();
}

Cfg::TextStyle
FormSpinBox::text() const
{
	Cfg::TextStyle textStyle = Cfg::textStyleFromFont( d->m_font );
	textStyle.style().push_back( Cfg::c_right );
	textStyle.set_text( d->m_text );

	return textStyle;
}

void
FormSpinBox::setText( const Cfg::TextStyle & c )
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

	d->m_font.setPointSize( c.fontSize() );

	d->m_text = c.text();

	update();
}

QRectF
FormSpinBox::boundingRect() const
{
	return d->m_rect;
}

void
FormSpinBox::setPosition( const QPointF & pos, bool pushUndoCommand )
{
	if( pushUndoCommand )
		form()->undoStack()->push( new UndoMove< FormSpinBox > ( form(),
			objectId(), pos - position() ) );

	QRectF r = boundingRect();
	r.moveTopLeft( pos );

	d->setRect( r );
}

QPointF
FormSpinBox::position() const
{
	return pos();
}

QRectF
FormSpinBox::rectangle() const
{
	QRectF r = boundingRect();
	r.moveTopLeft( position() );

	return r;
}

void
FormSpinBox::setRectangle( const QRectF & rect,
	bool pushUndoCommand )
{
	Q_UNUSED( pushUndoCommand )

	resize( rect );

	scene()->update();
}

void
FormSpinBox::resize( const QRectF & rect )
{
	d->setRect( rect );

	form()->update();
}

void
FormSpinBox::moveResizable( const QPointF & delta )
{
	moveBy( delta.x(), delta.y() );
}

void
FormSpinBox::contextMenuEvent( QGraphicsSceneContextMenuEvent * event )
{
	QMenu menu;

	menu.addAction( QIcon( ":/Core/img/configure.png" ),
		tr( "Properties" ), this, &FormSpinBox::properties );

	menu.exec( event->screenPos() );
}

void
FormSpinBox::properties()
{
	SpinBoxProperties dlg;

	dlg.setCfg( cfg() );

	if( dlg.exec() == QDialog::Accepted )
	{
		form()->undoStack()->push( new UndoChangeTextWithOpts( form(),
			objectId(), text(), dlg.cfg().text() ) );

		setText( dlg.cfg().text() );

		update();
	}
}

QSizeF
FormSpinBox::defaultSize() const
{
	return QSizeF( MmPx::instance().fromMmX( 15.0 ), boxHeight() );
}

} /* namespace Core */

} /* namespace Prototyper */
