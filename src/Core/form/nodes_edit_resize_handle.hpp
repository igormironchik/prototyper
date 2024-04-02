
/*
	SPDX-FileCopyrightText: 2016-2024 Igor Mironchik <igor.mironchik@gmail.com>
	SPDX-License-Identifier: GPL-3.0-or-later
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
