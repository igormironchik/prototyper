
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
#include "form_image.hpp"
#include "form_image_handles.hpp"
#include "form.hpp"

// Qt include.
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>
#include <QByteArray>
#include <QBuffer>


namespace Prototyper {

namespace Core {

//
// FormImagePrivate
//

class FormImagePrivate {
public:
	FormImagePrivate( FormImage * parent )
		:	q( parent )
		,	m_handles( 0 )
	{
	}

	//! Init.
	void init();

	//! Parent.
	FormImage * q;
	//! Image.
	QImage m_image;
	//! Handles.
	FormImageHandles * m_handles;
}; // class FormImagePrivate

void
FormImagePrivate::init()
{
	m_handles = new FormImageHandles( q, q->parentItem(), q->form() );
}


//
// FormImage
//

FormImage::FormImage( Form * form, QGraphicsItem * parent )
	:	QGraphicsPixmapItem( parent )
	,	FormObject( FormObject::ImageType, form )
	,	d( new FormImagePrivate( this ) )
{
	d->init();
}

FormImage::~FormImage()
{
}

Cfg::Image
FormImage::cfg() const
{
	Cfg::Image c;

	c.setObjectId( objectId() );

	Cfg::Point p;
	p.setX( pos().x() );
	p.setY( pos().y() );

	c.setPos( p );

	Cfg::Size s;
	s.setWidth( pixmap().width() );
	s.setHeight( pixmap().height() );

	c.setSize( s );

	c.setKeepAspectRatio( d->m_handles->isKeepAspectRatio() );

	QByteArray byteArray;
	QBuffer buffer( &byteArray );
	d->m_image.save( &buffer, "PNG" );

	c.setData( QString::fromLatin1( byteArray.toBase64().data() ) );

	c.setLink( link() );

	return c;
}

void
FormImage::setCfg( const Cfg::Image & c )
{
	setObjectId( c.objectId() );

	const QSize s( c.size().width(), c.size().height() );

	const QByteArray data = QByteArray::fromBase64( c.data().toLatin1() );

	d->m_image = QImage::fromData( data, "PNG" );

	d->m_handles->setKeepAspectRatio( c.keepAspectRatio() );

	setPixmap( QPixmap::fromImage( d->m_image.scaled( s,
		( c.keepAspectRatio() ? Qt::KeepAspectRatio : Qt::IgnoreAspectRatio ),
		Qt::SmoothTransformation ) ) );

	setPos( QPointF( c.pos().x(), c.pos().y() ) );

	setLink( c.link() );

	QRectF r = pixmap().rect();
	r.moveTop( pos().y() );
	r.moveLeft( pos().x() );

	d->m_handles->setRect( r );
}

const QImage &
FormImage::image() const
{
	return d->m_image;
}

void
FormImage::setImage( const QImage & img )
{
	d->m_image = img;

	setPixmap( QPixmap::fromImage( d->m_image ) );

	QRectF r = d->m_image.rect();
	r.moveTop( pos().y() );
	r.moveLeft( pos().x() );

	d->m_handles->setRect( r );
}

void
FormImage::postDeletion()
{
	delete d->m_handles;
}

void
FormImage::paint( QPainter * painter, const QStyleOptionGraphicsItem * option,
	QWidget * widget )
{
	QGraphicsPixmapItem::paint( painter, option, widget );

	if( isSelected() && !group() )
		d->m_handles->show();
	else
		d->m_handles->hide();
}

void
FormImage::positionElements( const QPointF & pos )
{
	setPos( pos );

	QRectF r = boundingRect();
	r.moveTopLeft( pos );

	d->m_handles->setRect( r );
}

QPointF
FormImage::position() const
{
	return pos();
}

void
FormImage::resize( const QRectF & rect )
{
	setPos( rect.topLeft() );

	setPixmap( QPixmap::fromImage( d->m_image.scaled(
		QSize( rect.width(), rect.height() ),
		( d->m_handles->isKeepAspectRatio() ? Qt::KeepAspectRatio :
			Qt::IgnoreAspectRatio ), Qt::SmoothTransformation ) ) );

	QRectF r = pixmap().rect();
	r.moveTop( pos().y() );
	r.moveLeft( pos().x() );

	d->m_handles->setRect( r );

	form()->update();
}

void
FormImage::moveResizable( const QPointF & delta )
{
	moveBy( delta.x(), delta.y() );
}

} /* namespace Core */

} /* namespace Prototyper */
