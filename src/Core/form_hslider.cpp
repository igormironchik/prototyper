
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
#include "form_hslider.hpp"
#include "utils.hpp"
#include "page.hpp"
#include "form_undo_commands.hpp"
#include "form_actions.hpp"

// Qt include.
#include <QPainter>
#include <QUndoStack>
#include <QGraphicsScene>


namespace Prototyper {

namespace Core {

//
// FormHSliderPrivate
//

class FormHSliderPrivate {
public:
	FormHSliderPrivate( FormHSlider * parent, const QRectF & rect )
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
	FormHSlider * q;
	//! Rect.
	QRectF m_rect;
	//! Resizable proxy.
	QScopedPointer< FormResizableProxy > m_proxy;
}; // class FormHSliderPrivate

void
FormHSliderPrivate::init()
{
	m_proxy.reset( new FormResizableProxy( q, q->parentItem(), q->form() ) );

	setRect( m_rect );

	m_proxy->setMinSize( q->defaultSize() );

	q->setObjectPen( QPen( FormAction::instance()->strokeColor() ),
		false );

	q->setObjectBrush( Qt::transparent );
}

void
FormHSliderPrivate::setRect( const QRectF & rect )
{
	m_rect = rect;

	q->setPos( m_rect.topLeft() );

	m_proxy->setRect( m_rect );

	m_rect.moveTopLeft( QPointF( 0.0, 0.0 ) );
}


//
// FormHSlider
//

FormHSlider::FormHSlider( const QRectF & rect, Page * form,
	QGraphicsItem * parent )
	:	QGraphicsItem( parent )
	,	FormObject( FormObject::HSliderType, form )
	,	d( new FormHSliderPrivate( this, rect ) )
{
	d->init();
}

FormHSlider::~FormHSlider()
{
}

void
FormHSlider::paint( QPainter * painter, const QStyleOptionGraphicsItem * option,
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
FormHSlider::draw( QPainter * painter, const QRectF & rect,
	const QPen & pen, int dpi )
{
	painter->setPen( pen );
	painter->setBrush( QBrush( pen.color() ) );

	QRectF r = rect;

	if( r.height() > boxHeight( dpi ) )
	{
		r.setHeight( boxHeight( dpi ) );
		r.moveTopLeft( QPointF( rect.topLeft().x(), rect.topLeft().y() +
			( rect.height() - boxHeight( dpi ) ) / 2.0 ) );
	}

	const qreal gh = r.height() * 0.15;
	const qreal hw = r.height() * 0.5;
	const qreal sx = r.width() * 0.2 + rect.x();

	const QRectF groove( r.x(), r.y() + r.height() / 2.0 - gh / 2.0,
		r.width(), gh );

	const qreal ro = r.height() / 10.0;

	const QRectF handle( sx, r.y(), hw, r.height() );

	painter->drawRoundedRect( groove, ro, ro );

	painter->setBrush( Qt::white );

	painter->drawRoundedRect( handle, ro, ro );
}

qreal
FormHSlider::boxHeight( int dpi )
{
	if( !dpi )
		return MmPx::instance().fromMmY( 4.0 );
	else
		return MmPx::instance().fromMm( 4.0, dpi );
}

void
FormHSlider::setObjectPen( const QPen & p, bool pushUndoCommand )
{
	FormObject::setObjectPen( p, pushUndoCommand );

	update();
}

Cfg::HSlider
FormHSlider::cfg() const
{
	Cfg::HSlider c;

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

	c.set_z( zValue() );

	return c;
}

void
FormHSlider::setCfg( const Cfg::HSlider & c )
{
	setObjectId( c.objectId() );
	setObjectPen( Cfg::fromPen( c.pen() ), false );
	d->setRect( QRectF( MmPx::instance().fromMmX( c.pos().x() ),
		MmPx::instance().fromMmY( c.pos().y() ),
		MmPx::instance().fromMmX( c.size().width() ),
		MmPx::instance().fromMmY( c.size().height() ) ) );

	setZValue( c.z() );

	update();
}

QRectF
FormHSlider::boundingRect() const
{
	return d->m_rect;
}

void
FormHSlider::setPosition( const QPointF & pos, bool pushUndoCommand )
{
	if( pushUndoCommand )
		form()->undoStack()->push( new UndoMove< FormHSlider > ( form(),
			objectId(), pos - position() ) );

	QRectF r = boundingRect();
	r.moveTopLeft( pos );

	d->setRect( r );
}

QPointF
FormHSlider::position() const
{
	return pos();
}

QRectF
FormHSlider::rectangle() const
{
	QRectF r = boundingRect();
	r.moveTopLeft( position() );

	return r;
}

void
FormHSlider::setRectangle( const QRectF & rect, bool pushUndoCommand )
{
	Q_UNUSED( pushUndoCommand )

	resize( rect );

	scene()->update();
}

void
FormHSlider::resize( const QRectF & rect )
{
	d->setRect( rect );

	form()->update();
}

void
FormHSlider::moveResizable( const QPointF & delta )
{
	moveBy( delta.x(), delta.y() );
}

QSizeF
FormHSlider::defaultSize() const
{
	return QSizeF( MmPx::instance().fromMmX( 15.0 ), boxHeight() );
}

} /* namespace Core */

} /* namespace Prototyper */
