
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
#include "form_combobox.hpp"
#include "utils.hpp"
#include "page.hpp"
#include "form_undo_commands.hpp"

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
		,	m_rect( QRectF( rect.x(), rect.y(), rect.width(), 25.0 ) )
		,	m_proxy( 0 )
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
	QScopedPointer< FormResizableProxy > m_proxy;
}; // class FormComboBoxPrivate

void
FormComboBoxPrivate::init()
{
	m_proxy.reset( new FormResizableProxy( q, q->parentItem(), q->form() ) );

	setRect( m_rect );

	m_proxy->setMinSize( q->defaultSize() );
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

FormComboBox::FormComboBox( const QRectF & rect, Page * form,
	QGraphicsItem * parent )
	:	QGraphicsItem( parent )
	,	FormObject( FormObject::ComboBoxType, form )
	,	d( new FormComboBoxPrivate( this, rect ) )
{
	d->init();
}

FormComboBox::~FormComboBox()
{
}

void
FormComboBox::paint( QPainter * painter, const QStyleOptionGraphicsItem * option,
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
FormComboBox::draw( QPainter * painter, const QRectF & rect,
	const QPen & pen, int dpi )
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
	const qreal o = h / 5;
	const qreal leftX = r.x() + r.width() - h;

	painter->drawLine( QLineF( leftX, r.y(), leftX, r.y() + h ) );

	QPainterPath path;
	path.moveTo( leftX + o, r.y() + o );
	path.lineTo( leftX + h - o, r.y() + o );
	path.lineTo( leftX + h / 2.0, r.y() + h - o );

	painter->setBrush( QBrush( pen.color() ) );

	painter->drawPath( path );
}

qreal
FormComboBox::boxHeight( int dpi )
{
	if( !dpi )
		return MmPx::instance().fromMmY( 4.0 );
	else
		return MmPx::instance().fromMm( 4.0, dpi );
}

void
FormComboBox::setObjectPen( const QPen & p, bool pushUndoCommand )
{
	FormObject::setObjectPen( p, pushUndoCommand );

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

	return c;
}

void
FormComboBox::setCfg( const Cfg::ComboBox & c )
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
FormComboBox::boundingRect() const
{
	return d->m_rect;
}

void
FormComboBox::setPosition( const QPointF & pos, bool pushUndoCommand )
{
	if( pushUndoCommand )
		form()->undoStack()->push( new UndoMove< FormComboBox > ( form(),
			objectId(), pos - position() ) );

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
	Q_UNUSED( pushUndoCommand )

	resize( rect );

	scene()->update();
}

void
FormComboBox::resize( const QRectF & rect )
{
	d->setRect( rect );

	form()->update();
}

void
FormComboBox::moveResizable( const QPointF & delta )
{
	moveBy( delta.x(), delta.y() );
}

QSizeF
FormComboBox::defaultSize() const
{
	return QSizeF( MmPx::instance().fromMmX( 15.0 ), boxHeight() );
}

} /* namespace Core */

} /* namespace Prototyper */
