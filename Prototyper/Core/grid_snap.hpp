
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

#ifndef PROTOTYPER__CORE__GRID_SNAP_H__INCLUDED
#define PROTOTYPER__CORE__GRID_SNAP_H__INCLUDED

// Qt include.
#include <QGraphicsItem>
#include <QScopedPointer>


namespace Prototyper {

namespace Core {

//
// GridSnap
//

class GridSnapPrivate;

//! Grid snap.
class GridSnap
	:	public QGraphicsItem
{
public:
	GridSnap( QGraphicsItem * parent );
	~GridSnap();

	//! \return Current position.
	const QPointF & snapPos() const;
	//! Set position, it will calculate snap position.
	void setSnapPos( const QPointF & p );

	//! Set grid step.
	void setGridStep( int step );

	//! \return Point snapped to grid.
	QPointF snapped( const QPointF & p ) const;

	QRectF boundingRect() const Q_DECL_OVERRIDE;

	void paint( QPainter * painter, const QStyleOptionGraphicsItem * option,
		QWidget * widget ) Q_DECL_OVERRIDE;

protected:
	void mouseMoveEvent( QGraphicsSceneMouseEvent * event )
		Q_DECL_OVERRIDE;
	void mousePressEvent( QGraphicsSceneMouseEvent * event )
		Q_DECL_OVERRIDE;
	void mouseReleaseEvent( QGraphicsSceneMouseEvent * event )
		Q_DECL_OVERRIDE;

private:
	Q_DISABLE_COPY( GridSnap )

	QScopedPointer< GridSnapPrivate > d;
}; // class GridSnap

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__GRID_SNAP_H__INCLUDED
