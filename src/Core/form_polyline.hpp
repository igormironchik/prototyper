
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

#ifndef PROTOTYPER__CORE__FORM_POLYLINE_HPP__INCLUDED
#define PROTOTYPER__CORE__FORM_POLYLINE_HPP__INCLUDED

// Qt include.
#include <QGraphicsPathItem>

// C++ include.
#include <memory>

// Prototyper include.
#include "form_object.hpp"
#include "form_resizable.hpp"
#include "form_move_handle.hpp"
#include "project_cfg.hpp"


namespace Prototyper {

namespace Core {

class Page;


//
// FormPolyline
//

class FormPolylinePrivate;

//! Polyline on the form.
class FormPolyline final
	:	public QObject
	,	public QGraphicsPathItem
	,	public FormObject
	,	public FormResizable
	,	public FormWithHandle
{
	Q_OBJECT

public:
	explicit FormPolyline( Page * page, QGraphicsItem * parent = 0 );
	~FormPolyline() override;

	//! \return Type.
	static ObjectType staticObjectType()
		{ return PolylineType; }

	//! \return Cfg.
	Cfg::Polyline cfg() const;
	//! Set cfg.
	void setCfg( const Cfg::Polyline & c );

	//! Draw polyline.
	static void draw( QPainter * painter, const Cfg::Polyline & cfg, qreal dpi );

	//! \return Lines.
	const QList< QLineF > & lines() const;
	//! Set lines.
	void setLines( const QList< QLineF > & lns );
	//! Append line.
	void appendLine( const QLineF & line );
	//! Remove last line.
	void removeLine( const QLineF & line );
	//! \return Count of lines.
	int countOfLines() const;

	//! Show/hide handles.
	void showHandles( bool show = true );

	//! \return Is closed?
	bool isClosed() const;

	//! \return Point under handle if so.
	QPointF pointUnderHandle( const QPointF & p, bool & intersected ) const;

	//! Set pen.
	void setObjectPen( const QPen & p, bool pushUndoCommand = true ) override;
	//! Set brush.
	void setObjectBrush( const QBrush & b, bool pushUndoCommand = true ) override;

	QRectF boundingRect() const override;

	//! Handle mouse move in handles.
	void handleMouseMoveInHandles( const QPointF & p );

	void paint( QPainter * painter, const QStyleOptionGraphicsItem * option,
		QWidget * widget = 0 ) override;

	//! Position elements.
	void setPosition( const QPointF & pos, bool pushUndoCommand = true ) override;
	//! \return Position of the element.
	QPointF position() const override;

	//! \return Rectangle of the element.
	QRectF rectangle() const override;
	//! Set rectangle.
	void setRectangle( const QRectF & rect, bool pushUndoCommand = true ) override;

	//! Clone object.
	FormObject * clone() const override;

protected:
	//! Resize.
	void resize( const QRectF & rect ) override;
	//! Move resizable.
	void moveResizable( const QPointF & delta ) override;
	//! Handle moved.
	void handleMoved( const QPointF & delta, FormMoveHandle * handle ) override;

private slots:
	friend class FormPolylinePrivate;

	//! Mode changed.
	void modeChanged();

private:
	Q_DISABLE_COPY( FormPolyline )

	std::unique_ptr< FormPolylinePrivate > d;
}; // class FormPolyline

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__FORM_POLYLINE_HPP__INCLUDED
