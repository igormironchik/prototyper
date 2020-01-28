
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
#include "form_comment.hpp"
#include "utils.hpp"
#include "comments.hpp"
#include "constants.hpp"

// Qt include.
#include <QFontMetrics>
#include <QApplication>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>
#include <QGraphicsView>


namespace Prototyper {

namespace Core {

//
// PageCommentPrivate
//

class PageCommentPrivate {
public:
	PageCommentPrivate( PageComment * parent )
		:	q( parent )
		,	m_id( 0 )
		,	m_pressed( false )
		,	m_distance( 0 )
	{
	}

	//! Parent.
	PageComment * q;
	//! Comments.
	QStringList m_comments;
	//! Id.
	int m_id;
	//! Mouse pos.
	QPointF m_pos;
	//! Mouse pressed.
	bool m_pressed;
	//! Mouse distance.
	int m_distance;
}; // class PageCommentPrivate


//
// PageComment
//

PageComment::PageComment( QGraphicsItem * parent )
	:	QGraphicsObject( parent )
	,	d( new PageCommentPrivate( this ) )
{
	connect( this, &PageComment::showComments,
		this, &PageComment::showCommentsImpl );

	setZValue( c_mostTopZValue - 1 );
}

PageComment::~PageComment() = default;

Cfg::Comments
PageComment::cfg() const
{
	Cfg::Comments c;

	c.set_id( d->m_id );

	Cfg::Point p;
	p.set_x( MmPx::instance().toMmX( pos().x() ) );
	p.set_y( MmPx::instance().toMmY( pos().y() ) );

	c.set_pos( p );

	for( const auto & s : qAsConst( d->m_comments ) )
		c.comment().push_back( s );

	return c;
}

void
PageComment::setCfg( const Cfg::Comments & c )
{
	setPos( QPointF( MmPx::instance().fromMmX( c.pos().x() ),
		MmPx::instance().fromMmY( c.pos().y() ) ) );

	d->m_comments.clear();

	for( const auto & s : c.comment() )
		d->m_comments.append( s );

	d->m_id = c.id();

	update();
}

void
PageComment::setId( int id )
{
	d->m_id = id;

	scene()->update();
}

int
PageComment::id() const
{
	return d->m_id;
}

bool
PageComment::isItYou( const QPointF & point ) const
{
	auto r = boundingRect();
	r.moveTopLeft( pos() );

	return r.contains( point );
}

QRectF
PageComment::boundingRect() const
{
	QFontMetrics fm( QApplication::font() );
	const auto r = fm.boundingRect( QString::number( d->m_id < 10 ? 10 : d->m_id ) )
		.adjusted( -5, -5, 5, 5 );

	return QRectF( 0.0, 0.0, r.width(), r.height() );
}

void
PageComment::paint( QPainter * painter, const QStyleOptionGraphicsItem *, QWidget * )
{
	painter->setFont( QApplication::font() );
	painter->setPen( QColor( 33, 122, 255 ) );
	painter->setBrush( QColor( 33, 122, 255 ) );
	painter->drawRoundedRect( boundingRect(), 3, 3 );
	painter->setPen( Qt::white );
	painter->drawRoundedRect( boundingRect().adjusted( 3, 3, -3, -3 ), 3, 3 );
	painter->drawText( boundingRect(), Qt::AlignCenter, QString::number( d->m_id ) );
}

void
PageComment::mousePressEvent( QGraphicsSceneMouseEvent * e )
{
	if( e->button() == Qt::LeftButton )
	{
		d->m_pressed = true;

		d->m_pos = e->pos();

		e->accept();
	}
	else
		e->ignore();
}

void
PageComment::mouseMoveEvent( QGraphicsSceneMouseEvent * e )
{
	if( d->m_pressed )
	{
		d->m_distance += qRound( ( d->m_pos - e->pos() ).manhattanLength() );

		setPos( pos() - d->m_pos + e->pos() );

		e->accept();
	}
	else
		e->ignore();
}

void
PageComment::mouseReleaseEvent( QGraphicsSceneMouseEvent * e )
{
	if( e->button() == Qt::LeftButton )
	{
		d->m_pressed = false;

		if( d->m_distance < 5 )
			emit showComments();
		else
			emit changed();

		d->m_distance = 0;

		e->accept();
	}
	else
		e->ignore();
}

void
PageComment::showCommentsImpl()
{
	Comments dlg( d->m_comments, scene()->views().first() );

	dlg.exec();

	if( d->m_comments != dlg.comments() )
		emit changed();

	d->m_comments = dlg.comments();
}

} /* namespace Core */

} /* namespace Prototyper */
