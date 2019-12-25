
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
#include "page.hpp"
#include "form_undo_commands.hpp"

// Qt include.
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>
#include <QByteArray>
#include <QBuffer>
#include <QUndoStack>
#include <QGraphicsScene>


namespace Prototyper {

namespace Core {

//
// FormImagePrivate
//

class FormImagePrivate {
public:
	explicit FormImagePrivate( FormImage * parent )
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
	QScopedPointer< FormImageHandles > m_handles;
}; // class FormImagePrivate

void
FormImagePrivate::init()
{
	m_handles.reset( new FormImageHandles( q, q->parentItem(), q->form() ) );
}


//
// FormImage
//

FormImage::FormImage( Page * form, QGraphicsItem * parent )
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

	c.set_objectId( objectId() );

	Cfg::Point p;
	p.set_x( pos().x() );
	p.set_y( pos().y() );

	c.set_pos( p );

	Cfg::Size s;
	s.set_width( pixmap().width() );
	s.set_height( pixmap().height() );

	c.set_size( s );

	c.set_keepAspectRatio( d->m_handles->isKeepAspectRatio() );

	QByteArray byteArray;
	QBuffer buffer( &byteArray );
	d->m_image.save( &buffer, "PNG" );

	c.set_data( QString::fromLatin1( byteArray.toBase64().data() ) );

	c.set_link( link() );

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
FormImage::setPosition( const QPointF & pos, bool pushUndoCommand )
{
	if( pushUndoCommand )
		form()->undoStack()->push( new UndoMove< FormImage > ( form(),
			objectId(), pos - position() ) );

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

QRectF
FormImage::rectangle() const
{
	QRectF r = boundingRect();
	r.moveTopLeft( position() );

	return r;
}

void
FormImage::setRectangle( const QRectF & rect, bool pushUndoCommand )
{
	Q_UNUSED( pushUndoCommand )

	resize( rect );

	scene()->update();
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
