
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
#include "form_vslider.hpp"
#include "utils.hpp"
#include "form.hpp"
#include "form_undo_commands.hpp"

// Qt include.
#include <QPainter>
#include <QUndoStack>


namespace Prototyper {

namespace Core {

//
// FormVSliderPrivate
//

class FormVSliderPrivate {
public:
	FormVSliderPrivate( FormVSlider * parent, const QRectF & rect )
		:	q( parent )
		,	m_rect( QRectF( rect.x(), rect.y(), 25.0, rect.height() ) )
		,	m_proxy( 0 )
	{
	}

	//! Init.
	void init();
	//! Set rect.
	void setRect( const QRectF & rect );

	//! Parent.
	FormVSlider * q;
	//! Rect.
	QRectF m_rect;
	//! Resizable proxy.
	QScopedPointer< FormResizableProxy > m_proxy;
}; // class FormVSliderPrivate

void
FormVSliderPrivate::init()
{
	m_proxy.reset( new FormResizableProxy( q, q->parentItem(), q->form() ) );

	setRect( m_rect );

	m_proxy->setMinSize( QSizeF( 25.0, 45.0 ) );
}

void
FormVSliderPrivate::setRect( const QRectF & rect )
{
	m_rect = rect;

	q->setPos( m_rect.topLeft() );

	m_proxy->setRect( m_rect );

	m_rect.moveTopLeft( QPointF( 0.0, 0.0 ) );
}


//
// FormVSlider
//

FormVSlider::FormVSlider( const QRectF & rect, Form * form,
	QGraphicsItem * parent )
	:	QGraphicsItem( parent )
	,	FormObject( FormObject::VSliderType, form )
	,	d( new FormVSliderPrivate( this, rect ) )
{
	d->init();
}

FormVSlider::~FormVSlider()
{
}

void
FormVSlider::paint( QPainter * painter, const QStyleOptionGraphicsItem * option,
	QWidget * widget )
{
	Q_UNUSED( widget )
	Q_UNUSED( option )

	draw( painter, d->m_rect, objectPen() );

	if( isSelected() && !group() )
		d->m_proxy->show();
	else
		d->m_proxy->hide();
}

void
FormVSlider::draw( QPainter * painter, const QRectF & rect,
	const QPen & pen )
{
	painter->setPen( pen );
	painter->setBrush( QBrush( pen.color() ) );

	const qreal gw = rect.width() * 0.15;
	const qreal hh = rect.width() * 0.5;
	const qreal sy = rect.height() * 0.2 + rect.y();

	const QRectF groove( rect.x() + rect.width() / 2.0 - gw / 2.0, rect.y(),
		gw, rect.height() );

	const QRectF handle( rect.x(), sy, rect.width(), hh );

	painter->drawRoundedRect( groove, 2.0, 2.0 );

	painter->setBrush( Qt::white );

	painter->drawRoundedRect( handle, 2.0, 2.0 );
}

void
FormVSlider::setObjectPen( const QPen & p, bool pushUndoCommand )
{
	FormObject::setObjectPen( p, pushUndoCommand );

	update();
}

Cfg::VSlider
FormVSlider::cfg() const
{
	Cfg::VSlider c;

	c.setObjectId( objectId() );

	Cfg::Point p;
	p.setX( pos().x() );
	p.setY( pos().y() );

	c.setPos( p );

	Cfg::Size s;
	s.setWidth( d->m_rect.width() );
	s.setHeight( d->m_rect.height() );

	c.setSize( s );

	c.setPen( Cfg::pen( objectPen() ) );

	c.setLink( link() );

	return c;
}

void
FormVSlider::setCfg( const Cfg::VSlider & c )
{
	setObjectId( c.objectId() );
	setLink( c.link() );
	setObjectPen( Cfg::fromPen( c.pen() ), false );
	d->setRect( QRectF( c.pos().x(), c.pos().y(),
		c.size().width(), c.size().height() ) );

	update();
}

QRectF
FormVSlider::boundingRect() const
{
	return d->m_rect;
}

void
FormVSlider::setPosition( const QPointF & pos, bool pushUndoCommand )
{
	if( pushUndoCommand )
		form()->undoStack()->push( new UndoMove< FormVSlider > ( form(),
			objectId(), pos - position() ) );

	QRectF r = boundingRect();
	r.moveTopLeft( pos );

	d->setRect( r );
}

QPointF
FormVSlider::position() const
{
	return pos();
}

QRectF
FormVSlider::rectangle() const
{
	QRectF r = boundingRect();
	r.moveTopLeft( position() );

	return r;
}

void
FormVSlider::setRectangle( const QRectF & rect, bool pushUndoCommand )
{
	Q_UNUSED( pushUndoCommand )

	resize( rect );
}

void
FormVSlider::resize( const QRectF & rect )
{
	d->setRect( rect );

	form()->update();
}

void
FormVSlider::moveResizable( const QPointF & delta )
{
	moveBy( delta.x(), delta.y() );
}

} /* namespace Core */

} /* namespace Prototyper */
