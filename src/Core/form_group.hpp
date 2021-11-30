
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

#ifndef PROTOTYPER__CORE__FORM_GROUP_HPP__INCLUDED
#define PROTOTYPER__CORE__FORM_GROUP_HPP__INCLUDED

// Qt include.
#include <QGraphicsItemGroup>

// C++ include.
#include <memory>

// Prototyper include.
#include "form_object.hpp"
#include "form_move_handle.hpp"
#include "project_cfg.hpp"


namespace Prototyper {

namespace Core {

class Page;


//
// FormGroup
//

class FormGroupPrivate;

class FormGroup final
	:	public QGraphicsItemGroup
	,	public FormObject
	,	public FormWithHandle
{
public:
	explicit FormGroup( Page * page, QGraphicsItem * parent = 0 );
	~FormGroup() override;

	//! \return Type.
	static ObjectType staticObjectType()
		{ return GroupType; }

	//! \return Cfg.
	Cfg::Group cfg() const;
	//! Set cfg. \warning This group must be empty.
	void setCfg( const Cfg::Group & c );

	//! \return List of children.
	QList< QGraphicsItem* > children() const;

	QRectF boundingRect() const override;

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
	//! Handle moved.
	void handleMoved( const QPointF & delta, FormMoveHandle * handle ) override;
	//! Handle released.
	void handleReleased( FormMoveHandle * handle ) override;

private:
	//! Create elem with rect.
	template< class Elem, class Config >
	void createElemWithRect( const Config & cfg );

private:
	Q_DISABLE_COPY( FormGroup )

	std::unique_ptr< FormGroupPrivate > d;
}; // class FormGroup

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__FORM_GROUP_HPP__INCLUDED
