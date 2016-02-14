
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

// Qt include.
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>


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
	//! Rect.
	QRectF m_rect;
	//! Handles.
	FormImageHandles * m_handles;
}; // class FormImagePrivate

void
FormImagePrivate::init()
{
	m_handles = new FormImageHandles( q, q->parentItem() );
}


//
// FormImage
//

FormImage::FormImage( Form * form, QGraphicsItem * parent )
	:	QGraphicsPixmapItem( parent )
	,	FormObject( form )
	,	d( new FormImagePrivate( this ) )
{
	d->init();
}

FormImage::~FormImage()
{
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
}

void
FormImage::moveResizable( const QPointF & delta )
{
	moveBy( delta.x(), delta.y() );
}

} /* namespace Core */

} /* namespace Prototyper */
