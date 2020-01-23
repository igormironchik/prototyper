
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

#ifndef PROTOTYPER_CORE_FORM_COMMENT_HPP_INCLUDED
#define PROTOTYPER_CORE_FORM_COMMENT_HPP_INCLUDED

// Qt include.
#include <QScopedPointer>
#include <QGraphicsObject>

// Prototyper include.
#include "project_cfg.hpp"


namespace Prototyper {

namespace Core {

//
// PageComment
//

class PageCommentPrivate;

//! Comment on the page.
class PageComment final
	:	public QGraphicsObject
{
	Q_OBJECT

signals:
	void showComments();

public:
	PageComment( QGraphicsItem * parent = nullptr );
	~PageComment() override;

	//! \return Cfg.
	Cfg::Comments cfg() const;
	//! Set cfg.
	void setCfg( const Cfg::Comments & c );

	QRectF boundingRect() const override;

	void paint( QPainter * painter, const QStyleOptionGraphicsItem * option,
		QWidget * widget = 0 ) override;

	void setId( int id );
	int id() const;

	bool isItYou( const QPointF & point ) const;

private slots:
	void showCommentsImpl();

protected:
	void mousePressEvent( QGraphicsSceneMouseEvent * e ) override;
	void mouseMoveEvent( QGraphicsSceneMouseEvent * e ) override;
	void mouseReleaseEvent( QGraphicsSceneMouseEvent * e ) override;

private:
	Q_DISABLE_COPY( PageComment )

	QScopedPointer< PageCommentPrivate > d;
}; // class PageComment

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER_CORE_FORM_COMMENT_HPP_INCLUDED
