
/*!
	\file

	\author Igor Mironchik (igor.mironchik at gmail dot com).

	Copyright (c) 2016-2021 Igor Mironchik

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

#ifndef PROTOTYPER__CORE__FORM_NODES_EDIT_RESIZE_HANDLE_HPP__INCLUDED
#define PROTOTYPER__CORE__FORM_NODES_EDIT_RESIZE_HANDLE_HPP__INCLUDED

// Qt include.
#include <QGraphicsObject>

// C++ include.
#include <memory>


namespace Prototyper {

namespace Core {

//
// NodesEditResizeHandle
//

class NodesEditResizeHandlePrivate;

//! Nodes edit / resize mode handle.
class NodesEditResizeHandle final
	:	public QGraphicsObject
{
	Q_OBJECT

signals:
	//! Current mode changed.
	void currentModeChanged();

public:
	explicit NodesEditResizeHandle( QGraphicsItem * parent );
	~NodesEditResizeHandle() override;

	//! Mode.
	enum Mode {
		NodesEditMode = 0,
		ResizeMode = 1
	}; // enum Mode

	//! \return Current mode.
	Mode currentMode() const;
	//! Set mode.
	void setCurrentMode( Mode m );

	QRectF boundingRect() const override;

	void paint( QPainter * painter, const QStyleOptionGraphicsItem * option,
		QWidget * widget = 0 ) override;

protected:
	void mousePressEvent( QGraphicsSceneMouseEvent * event ) override;
	void mouseReleaseEvent( QGraphicsSceneMouseEvent * event ) override;

private:
	Q_DISABLE_COPY( NodesEditResizeHandle )

	std::unique_ptr< NodesEditResizeHandlePrivate > d;
}; // class NodesEditResizeHandle

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__FORM_NODES_EDIT_RESIZE_HANDLE_HPP__INCLUDED
