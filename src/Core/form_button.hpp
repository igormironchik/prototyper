
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

#ifndef PROTOTYPER__CORE__FORM_BUTTON_HPP__INCLUDED
#define PROTOTYPER__CORE__FORM_BUTTON_HPP__INCLUDED

// Qt include.
#include <QGraphicsObject>
#include <QScopedPointer>

// Prototyper include.
#include "form_object.hpp"
#include "form_resizable.hpp"
#include "project_cfg.hpp"


namespace Prototyper {

namespace Core {

class TextWithOpts;


//
// FormButton
//

class FormButtonPrivate;

//! Button on the form.
class FormButton final
	:	public QGraphicsObject
	,	public FormObject
	,	public FormResizable
{
	Q_OBJECT

public:
	FormButton( const QRectF & rect, Page * page, QGraphicsItem * parent = 0 );
	~FormButton() override;

	//! \return Type.
	static ObjectType staticObjectType()
		{ return ButtonType; }

	void paint( QPainter * painter, const QStyleOptionGraphicsItem * option,
		QWidget * widget = 0 ) override;

	//! Set pen.
	void setObjectPen( const QPen & p, bool pushUndoCommand = true ) override;
	//! Set brush.
	void setObjectBrush( const QBrush & b, bool pushUndoCommand = true ) override;

	//! \return Cfg.
	Cfg::Button cfg() const;
	//! Set cfg.
	void setCfg( const Cfg::Button & c );

	//! \return Text.
	Cfg::TextStyle text() const;
	//! Set text.
	void setText( const Cfg::TextStyle & c );

	QRectF boundingRect() const override;

	//! Position elements.
	void setPosition( const QPointF & pos, bool pushUndoCommand = true ) override;
	//! \return Position of the element.
	QPointF position() const override;

	//! \return Rectangle of the element.
	QRectF rectangle() const override;
	//! Set rectangle.
	void setRectangle( const QRectF & rect, bool pushUndoCommand = true ) override;

	//! \return Default size.
	virtual QSizeF defaultSize() const override;

	QWidget * properties( QWidget * parent ) override;
	void updatePropertiesValues() override;

	//! Clone object.
	FormObject * clone() const override;

protected:
	//! Resize.
	void resize( const QRectF & rect ) override;
	//! Move resizable.
	void moveResizable( const QPointF & delta ) override;

private:
	friend class FormButtonPrivate;

	Q_DISABLE_COPY( FormButton )

	QScopedPointer< FormButtonPrivate > d;
}; // class FormButton

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__FORM_BUTTON_HPP__INCLUDED
