
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

#ifndef PROTOTYPER__CORE__FORM_MOVE_HANDLER_HPP__INCLUDED
#define PROTOTYPER__CORE__FORM_MOVE_HANDLER_HPP__INCLUDED

// Qt include.
#include <QGraphicsItem>
#include <QScopedPointer>


namespace Prototyper {

namespace Core {

class FormObject;

//
// FormMoveHandle
//

class FormMoveHandlePrivate;

//! Move handler on the form.
class FormMoveHandle
	:	public QGraphicsItem
{
public:
	FormMoveHandle( FormObject * object, QGraphicsItem * parent );
	~FormMoveHandle();

	//! \return Half of the size of the edge.
	qreal halfOfSize() const;

	QRectF boundingRect() const Q_DECL_OVERRIDE;

	void paint( QPainter * painter, const QStyleOptionGraphicsItem * option,
		QWidget * widget = 0 ) Q_DECL_OVERRIDE;

	//! Ignore mouse events.
	void ignoreMouseEvents( bool on = true );

	//! Handle mouse move.
	bool handleMouseMove( const QPointF & point );

	//! Clear.
	void clear();

protected:
	//! Handle was moved.
	virtual void moved( const QPointF & delta );

protected:
	void hoverEnterEvent( QGraphicsSceneHoverEvent * event )
		Q_DECL_OVERRIDE;
	void hoverLeaveEvent( QGraphicsSceneHoverEvent * event )
		Q_DECL_OVERRIDE;
	void mouseMoveEvent( QGraphicsSceneMouseEvent * event )
		Q_DECL_OVERRIDE;
	void mousePressEvent( QGraphicsSceneMouseEvent * event )
		Q_DECL_OVERRIDE;
	void mouseReleaseEvent( QGraphicsSceneMouseEvent * event )
		Q_DECL_OVERRIDE;

private:
	Q_DISABLE_COPY( FormMoveHandle )

	QScopedPointer< FormMoveHandlePrivate > d;
}; // class FormMoveHandler

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__FORM_MOVE_HANDLER_HPP__INCLUDED
