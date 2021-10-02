
/*!
	\file

	\author Igor Mironchik (igor.mironchik at gmail dot com).

	Copyright (c) 2016-2020 Igor Mironchik

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
#include "form_combobox.hpp"
#include "utils.hpp"
#include "page.hpp"
#include "form_undo_commands.hpp"
#include "form_actions.hpp"
#include "constants.hpp"

// Qt include.
#include <QPainter>
#include <QPainterPath>
#include <QUndoStack>
#include <QGraphicsScene>


namespace Prototyper {

namespace Core {

//
// FormComboBoxPrivate
//

class FormComboBoxPrivate {
public:
	FormComboBoxPrivate( FormComboBox * parent, const QRectF & rect )
		:	q( parent )
		,	m_rect( QRectF( rect.x(), rect.y(), rect.width(), FormComboBox::boxHeight() ) )
		,	m_proxy( nullptr )
	{
	}

	//! Init.
	void init();
	//! Set rect.
	void setRect( const QRectF & rect );

	//! Parent.
	FormComboBox * q;
	//! Rect.
	QRectF m_rect;
	//! Resizable proxy.
	std::unique_ptr< FormResizableProxy > m_proxy;
}; // class FormComboBoxPrivate

void
FormComboBoxPrivate::init()
{
	m_proxy.reset( new FormResizableProxy( q, q->parentItem(), q->page() ) );

	setRect( m_rect );

	m_proxy->setMinSize( q->defaultSize() );

	q->setObjectPen( QPen( PageAction::instance()->strokeColor() ),
		false );

	q->setObjectBrush( QBrush( PageAction::instance()->fillColor() ), false );
}

void
FormComboBoxPrivate::setRect( const QRectF & rect )
{
	m_rect = rect;

	q->setPos( m_rect.topLeft() );

	m_proxy->setRect( m_rect );

	m_rect.moveTopLeft( QPointF( 0.0, 0.0 ) );
}


//
// FormComboBox
//

FormComboBox::FormComboBox( const QRectF & rect, Page * page,
	QGraphicsItem * parent )
	:	QGraphicsItem( parent )
	,	FormObject( FormObject::ComboBoxType, page )
	,	d( new FormComboBoxPrivate( this, rect ) )
{
	d->init();
}

FormComboBox::~FormComboBox() = default;

void
FormComboBox::paint( QPainter * painter, const QStyleOptionGraphicsItem * option,
	QWidget * widget )
{
	Q_UNUSED( widget )
	Q_UNUSED( option )

	draw( painter, d->m_rect, objectPen(), objectBrush() );

	if( isSelected() && !group() )
		d->m_proxy->show();
	else
		d->m_proxy->hide();
}

void
FormComboBox::draw( QPainter * painter, const QRectF & rect,
	const QPen & pen, const QBrush & brush, int dpi )
{
	painter->setPen( pen );
	painter->setBrush( brush );

	QRectF r = rect;

	if( r.height() > boxHeight( dpi ) )
	{
		r.setHeight( boxHeight( dpi ) );
		r.moveTopLeft( QPointF( rect.topLeft().x(), rect.topLeft().y() +
			( rect.height() - boxHeight( dpi ) ) / c_halfDivider ) );
	}

	const qreal ro = r.height() / 10.0;

	painter->drawRoundedRect( r, ro, ro );

	const qreal h = r.height();
	const qreal o = h / 5;
	const qreal leftX = r.x() + r.width() - h;

	painter->drawLine( QLineF( leftX, r.y(), leftX, r.y() + h ) );

	QPainterPath path;
	path.moveTo( leftX + o, r.y() + o );
	path.lineTo( leftX + h - o, r.y() + o );
	path.lineTo( leftX + h / c_halfDivider, r.y() + h - o );
	path.lineTo( leftX + o, r.y() + o );

	painter->setBrush( QBrush( pen.color() ) );

	painter->drawPath( path );
}

qreal
FormComboBox::boxHeight( int dpi )
{
	if( !dpi )
		return MmPx::instance().fromMmY( 4.0 );

	return MmPx::instance().fromMm( 4.0, dpi );
}

void
FormComboBox::setObjectPen( const QPen & p, bool pushUndoCommand )
{
	FormObject::setObjectPen( p, pushUndoCommand );

	update();
}

void
FormComboBox::setObjectBrush( const QBrush & b, bool pushUndoCommand )
{
	FormObject::setObjectBrush( b, pushUndoCommand );

	update();
}

Cfg::ComboBox
FormComboBox::cfg() const
{
	Cfg::ComboBox c;

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
	c.set_brush( Cfg::brush( objectBrush() ) );

	c.set_z( zValue() );

	return c;
}

void
FormComboBox::setCfg( const Cfg::ComboBox & c )
{
	setObjectId( c.objectId() );
	setObjectPen( Cfg::fromPen( c.pen() ), false );
	setObjectBrush( Cfg::fromBrush( c.brush() ), false );
	d->setRect( QRectF( MmPx::instance().fromMmX( c.pos().x() ),
		MmPx::instance().fromMmY( c.pos().y() ),
		MmPx::instance().fromMmX( c.size().width() ),
		MmPx::instance().fromMmY( c.size().height() ) ) );

	setZValue( c.z() );

	update();
}

QRectF
FormComboBox::boundingRect() const
{
	return d->m_rect;
}

void
FormComboBox::setPosition( const QPointF & pos, bool pushUndoCommand )
{
	FormObject::setPosition( pos, pushUndoCommand );

	QRectF r = boundingRect();
	r.moveTopLeft( pos );

	d->setRect( r );
}

QPointF
FormComboBox::position() const
{
	return pos();
}

QRectF
FormComboBox::rectangle() const
{
	QRectF r = boundingRect();
	r.moveTopLeft( position() );

	return r;
}

void
FormComboBox::setRectangle( const QRectF & rect, bool pushUndoCommand )
{
	FormObject::setRectangle( rect, pushUndoCommand );

	resize( rect );

	scene()->update();
}

void
FormComboBox::resize( const QRectF & rect )
{
	d->setRect( rect );

	page()->update();
}

void
FormComboBox::moveResizable( const QPointF & delta )
{
	moveBy( delta.x(), delta.y() );
}

QSizeF
FormComboBox::defaultSize() const
{
	return { MmPx::instance().fromMmX( 10.0 ), boxHeight() };
}

FormObject *
FormComboBox::clone() const
{
	auto * o = new FormComboBox( rectangle(), page(), parentItem() );

	o->setCfg( cfg() );

	o->setObjectId( page()->nextId() );

	return o;
}

} /* namespace Core */

} /* namespace Prototyper */
