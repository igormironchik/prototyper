
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
#include "form_checkbox.hpp"
#include "utils.hpp"
#include "form_image_handles.hpp"

// Qt include.
#include <QPainter>


namespace Prototyper {

namespace Core {

//
// FormCheckBoxPrivate
//

class FormCheckBoxPrivate {
public:
	FormCheckBoxPrivate( FormCheckBox * parent, const QRectF & rect )
		:	q( parent )
		,	m_rect( QRectF( rect.topLeft().x(), rect.topLeft().y(),
				20.0, 20.0 ) )
		,	m_checked( true )
		,	m_handles( 0 )
	{
	}

	//! Init.
	void init();

	//! Parent.
	FormCheckBox * q;
	//! Rect.
	QRectF m_rect;
	//! Checked?
	bool m_checked;
	//! Handles.
	FormImageHandles * m_handles;
}; // class FormCheckBoxPrivate;

void
FormCheckBoxPrivate::init()
{
	q->setPos( m_rect.topLeft() );

	m_handles = new FormImageHandles( q, q->parentItem(), q->form() );

	m_handles->setMinSize( QSizeF( 20.0, 20.0 ) );

	m_handles->hide();

	m_rect.moveTopLeft( QPointF( 0.0, 0.0 ) );
}


//
// FormCheckBox
//

FormCheckBox::FormCheckBox( const QRectF & rect, Form * form,
	QGraphicsItem * parent )
	:	QGraphicsObject( parent )
	,	FormObject( FormObject::CheckBoxType, form )
	,	d( new FormCheckBoxPrivate( this, rect ) )
{
	d->init();
}

FormCheckBox::~FormCheckBox()
{
}

void
FormCheckBox::postDeletion()
{
	delete d->m_handles;
}

void
FormCheckBox::paint( QPainter * painter, const QStyleOptionGraphicsItem * option,
	QWidget * widget )
{
	Q_UNUSED( option )
	Q_UNUSED( widget )

	draw( painter,
		objectPen(),
		QRectF( 0.0, 0.0, d->m_rect.width(), d->m_rect.height() ),
		d->m_checked );

	if( isSelected() && !group() )
		d->m_handles->show();
	else
		d->m_handles->hide();
}

void
FormCheckBox::draw( QPainter * painter, const QPen & pen,
	const QRectF & rect, bool isChecked )
{
	painter->setPen( pen );

	painter->drawRect( rect );

	if( isChecked )
	{
		painter->drawLine( QLineF( rect.topLeft().x() + 4.0,
			rect.topLeft().y() + rect.height() / 2.0,
			rect.topLeft().x() + rect.width() / 2.0,
			rect.bottomLeft().y() - 4.0 ) );

		painter->drawLine( QLineF(
			rect.topLeft().x() + rect.width() / 2.0,
			rect.bottomLeft().y() - 4.0,
			rect.topRight().x() - 4.0,
			rect.topRight().y() + 4.0 ) );
	}
}

void
FormCheckBox::setObjectPen( const QPen & p )
{
	FormObject::setObjectPen( p );

	update();
}

Cfg::CheckBox
FormCheckBox::cfg() const
{
	Cfg::CheckBox c;

	c.setIsChecked( d->m_checked );
	c.setObjectId( objectId() );
	c.setLink( link() );
	c.setPen( Cfg::pen( objectPen() ) );

	Cfg::Point p;
	p.setX( pos().x() );
	p.setY( pos().y() );

	c.setPos( p );

	Cfg::Size s;
	s.setWidth( d->m_rect.width() );
	s.setHeight( d->m_rect.height() );

	c.setSize( s );

	return c;
}

void
FormCheckBox::setCfg( const Cfg::CheckBox & c )
{
	d->m_checked = c.isChecked();

	setObjectId( c.objectId() );
	setLink( c.link() );
	setObjectPen( Cfg::fromPen( c.pen() ) );

	d->m_rect = QRectF( c.pos().x(), c.pos().y(),
		c.size().width(), c.size().height() );

	update();
}

QRectF
FormCheckBox::boundingRect() const
{
	return d->m_rect;
}

void
FormCheckBox::resize( const QRectF & rect )
{
	setPos( rect.topLeft() );

	QSizeF s = d->m_rect.size().scaled( rect.width(), rect.height(),
		( d->m_handles->isKeepAspectRatio() ? Qt::KeepAspectRatio :
			Qt::IgnoreAspectRatio ) );

	d->m_rect = QRectF( 0.0, 0.0, s.width(), s.height() );

	d->m_handles->setRect( d->m_rect );

	update();
}

void
FormCheckBox::moveResizable( const QPointF & delta )
{
	moveBy( delta.x(), delta.y() );
}

void
FormCheckBox::contextMenuEvent( QGraphicsSceneContextMenuEvent * event )
{

}

void
FormCheckBox::properties()
{

}

} /* namespace Core */

} /* namespace Prototyper */
