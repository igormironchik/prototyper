
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

#ifndef PROTOTYPER__CORE__FORM_CHECKBOX_HPP__INCLUDED
#define PROTOTYPER__CORE__FORM_CHECKBOX_HPP__INCLUDED

// Qt include.
#include <QGraphicsObject>
#include <QFont>

// C++ include.
#include <memory>

// Prototyper include.
#include "object.hpp"
#include "resizable.hpp"
#include "project_cfg.hpp"


namespace Prototyper {

namespace Core {

class FormCheckBox;
class FormImageHandles;
class TextWithOpts;
class FormCheckBoxPrivate;


//
// FormCheckBox
//

//! Checkbox on the form.
class FormCheckBox
	:	public QGraphicsObject
	,	public FormObject
	,	public FormResizable
{
	Q_OBJECT

public:
	FormCheckBox( const QRectF & rect, Page * page, QGraphicsItem * parent = 0 );
	~FormCheckBox() override;

	//! \return Type.
	static ObjectType staticObjectType()
		{ return CheckBoxType; }

	void paint( QPainter * painter, const QStyleOptionGraphicsItem * option,
		QWidget * widget = 0 ) Q_DECL_OVERRIDE;

	//! Draw checkbox.
	static void draw( QPainter * painter, const QPen & pen, const QBrush & brush,
		const QFont & font, const QRectF & rect, qreal width, bool isChecked, const QString & text,
		const QRectF & boundingRect, int dpi = 0 );

	static qreal boxHeight( int dpi = 0 );

	//! Set pen.
	void setObjectPen( const QPen & p, bool pushUndoCommand = true ) override;
	//! Set brush.
	void setObjectBrush( const QBrush & b, bool pushUndoCommand = true ) override;

	//! \return Cfg.
	Cfg::CheckBox cfg() const;
	//! Set cfg.
	void setCfg( const Cfg::CheckBox & c );

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
	QSizeF defaultSize() const override;
	//! \return Minimum allowed size.
	QSizeF minimumSize() const override;

	QWidget * properties( QWidget * parent ) override;
	void updatePropertiesValues() override;

	virtual QString typeName() const;

	//! Is checked?
	bool isChecked() const;
	//! Set check state.
	void setChecked( bool on = true );

	//! Clone object.
	FormObject * clone() const override;

protected:
	//! Resize.
	void resize( const QRectF & rect ) override;
	//! Move resizable.
	void moveResizable( const QPointF & delta ) override;

protected:
	FormCheckBox( const QRectF & rect, Page * page,
		FormObject::ObjectType type, QGraphicsItem * parent = 0 );

protected:
	std::unique_ptr< FormCheckBoxPrivate > d;

private:
	friend class FormButtonPrivate;

	Q_DISABLE_COPY( FormCheckBox )
}; // class FormCheckBox

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__FORM_CHECKBOX_HPP__INCLUDED
