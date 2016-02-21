
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

#ifndef PROTOTYPER__CORE__FORM_RECTANGLE_HPP__INCLUDED
#define PROTOTYPER__CORE__FORM_RECTANGLE_HPP__INCLUDED

// Qt include.
#include <QGraphicsRectItem>
#include <QScopedPointer>

// Prototyper include.
#include "form_object.hpp"
#include "form_move_handle.hpp"
#include "project_cfg.hpp"


namespace Prototyper {

namespace Core {

class Form;


//
// FormRect
//

class FormRectPrivate;

//! Rect on the form.
class FormRect
	:	public QGraphicsRectItem
	,	public FormObject
	,	public FormWithHandle
{
public:
	explicit FormRect( Form * form, QGraphicsItem * parent = 0 );
	~FormRect();

	//! \return Cfg.
	Cfg::Rect cfg() const;
	//! Set cfg.
	void setCfg( const Cfg::Rect & c );

	QRectF boundingRect() const Q_DECL_OVERRIDE;

	void paint( QPainter * painter, const QStyleOptionGraphicsItem * option,
		QWidget * widget = 0 ) Q_DECL_OVERRIDE;

	//! Set pen.
	void setObjectPen( const QPen & p ) Q_DECL_OVERRIDE;

	//! Set brush.
	void setObjectBrush( const QBrush & b ) Q_DECL_OVERRIDE;

protected:
	//! Handle moved.
	void handleMoved( const QPointF & delta, FormMoveHandle * handle )
		Q_DECL_OVERRIDE;

private:
	Q_DISABLE_COPY( FormRect )

	QScopedPointer< FormRectPrivate > d;
}; // class FormRect

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__FORM_RECTANGLE_HPP__INCLUDED
