
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
#include "page.hpp"
#include "form_undo_commands.hpp"

// Qt include.
#include <QPainter>
#include <QUndoStack>
#include <QGraphicsScene>


namespace Prototyper {

namespace Core {

//
// FormVSliderPrivate
//

class FormVSliderPrivate {
public:
	FormVSliderPrivate( FormVSlider * parent, const QRectF & rect )
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

	m_proxy->setMinSize( q->defaultSize() );
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

FormVSlider::FormVSlider( const QRectF & rect, Page * form,
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
	const QPen & pen, int dpi )
{
	painter->setPen( pen );
	painter->setBrush( QBrush( pen.color() ) );

	QRectF r = rect;

	if( r.width() > boxHeight( dpi ) )
	{
		r.setWidth( boxHeight( dpi ) );
		r.moveTopLeft( QPointF( rect.topLeft().x() +
			( rect.width() - boxHeight( dpi ) ) / 2.0, rect.topLeft().y() ) );
	}

	const qreal gw = r.width() * 0.15;
	const qreal hh = r.width() * 0.5;
	const qreal sy = r.height() * 0.2 + r.y();
	const qreal ro = r.width() / 10.0;

	const QRectF groove( r.x() + r.width() / 2.0 - gw / 2.0, r.y(),
		gw, r.height() );

	const QRectF handle( r.x(), sy, r.width(), hh );

	painter->drawRoundedRect( groove, ro, ro );

	painter->setBrush( Qt::white );

	painter->drawRoundedRect( handle, ro, ro );
}

qreal
FormVSlider::boxHeight( int dpi )
{
	if( !dpi )
		return MmPx::instance().fromMmX( 4.0 );
	else
		return MmPx::instance().fromMm( 4.0, dpi );
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

	return c;
}

void
FormVSlider::setCfg( const Cfg::VSlider & c )
{
	setObjectId( c.objectId() );
	setObjectPen( Cfg::fromPen( c.pen() ), false );
	d->setRect( QRectF( MmPx::instance().fromMmX( c.pos().x() ),
		MmPx::instance().fromMmY( c.pos().y() ),
		MmPx::instance().fromMmX( c.size().width() ),
		MmPx::instance().fromMmY( c.size().height() ) ) );

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

	scene()->update();
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

QSizeF
FormVSlider::defaultSize() const
{
	return QSizeF( boxHeight(), MmPx::instance().fromMmY( 15.0 ) );
}

} /* namespace Core */

} /* namespace Prototyper */
