
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

#ifndef PROTOTYPER__CORE__FORM_MOVE_HANDLER_HPP__INCLUDED
#define PROTOTYPER__CORE__FORM_MOVE_HANDLER_HPP__INCLUDED

// Qt include.
#include <QGraphicsObject>
#include <QCursor>

// C++ include.
#include <memory>


namespace Prototyper {

namespace Core {

class FormMoveHandle;


//
// FormWithHandle
//

//! Base class for objects with handle.
class FormWithHandle {
public:
	FormWithHandle();
	virtual ~FormWithHandle();

protected:
	friend class FormMoveHandle;

	//! Handle moved.
	virtual void handleMoved( const QPointF & delta, FormMoveHandle * handle );
	//! Handle released.
	virtual void handleReleased( FormMoveHandle * handle );

private:
	Q_DISABLE_COPY( FormWithHandle )
}; // class FormWithHandle


class Page;
class FormMoveHandlePrivate;


//
// FormMoveHandle
//

//! Move handler on the form.
class FormMoveHandle
	:	public QGraphicsObject
{
public:
	FormMoveHandle( qreal halfSize, const QPointF & zero,
		FormWithHandle * object, QGraphicsItem * parent,
		Page * form, const QCursor & c = QCursor() );
	~FormMoveHandle() override;

	//! \return Half of the size of the edge.
	qreal halfOfSize() const;

	QRectF boundingRect() const override;

	void paint( QPainter * painter, const QStyleOptionGraphicsItem * option,
		QWidget * widget = 0 ) override;

	//! Ignore mouse events.
	void ignoreMouseEvents( bool on = true );

	//! Handle mouse move.
	bool handleMouseMove( const QPointF & point );

	//! Clear.
	void clear();

	//! \return Cursor.
	const QCursor & handleCursor() const;

	//! Set additional space to zero point.
	void setDeltaToZero( const QPointF & delta );

protected:
	//! Handle was moved.
	virtual void moved( const QPointF & delta );
	//! Handle released.
	virtual void released( FormMoveHandle * handle );

protected:
	void hoverEnterEvent( QGraphicsSceneHoverEvent * event ) override;
	void hoverMoveEvent( QGraphicsSceneHoverEvent * event ) override;
	void hoverLeaveEvent( QGraphicsSceneHoverEvent * event ) override;
	void mouseMoveEvent( QGraphicsSceneMouseEvent * event ) override;
	void mousePressEvent( QGraphicsSceneMouseEvent * event ) override;
	void mouseReleaseEvent( QGraphicsSceneMouseEvent * event ) override;
	bool eventFilter( QObject * watched, QEvent * event ) override;

protected:
	FormMoveHandle( std::unique_ptr< FormMoveHandlePrivate > && dd,
		QGraphicsItem * parent );

protected:
	std::unique_ptr< FormMoveHandlePrivate > d;

private:
	Q_DISABLE_COPY( FormMoveHandle )
}; // class FormMoveHandler

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__FORM_MOVE_HANDLER_HPP__INCLUDED
