
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

#ifndef PROTOTYPER__CORE__FORM_OBJECT_HPP__INCLUDED
#define PROTOTYPER__CORE__FORM_OBJECT_HPP__INCLUDED

// Qt include.
#include <QtGlobal>
#include <QPen>
#include <QBrush>


namespace Prototyper {

namespace Core {

class Page;


//
// FormObject
//

//! Object on the form.
class FormObject {
public:
	//! Type of the object.
	enum ObjectType {
		//! Line.
		LineType = 1,
		//! Polyline.
		PolylineType  = 2,
		//! Text.
		TextType = 3,
		//! Image.
		ImageType = 4,
		//! Rect.
		RectType = 5,
		//! Group.
		GroupType = 6,
		//! Form.
		PageType = 7,
		//! Button.
		ButtonType = 8,
		//! Combobox.
		ComboBoxType = 9,
		//! Radio button.
		RadioButtonType = 10,
		//! Checkbox.
		CheckBoxType = 11,
		//! Horizontal slider.
		HSliderType = 12,
		//! Vertical slider.
		VSliderType = 13,
		//! Spinbox.
		SpinBoxType = 14
	}; // enum ObjectType

	FormObject( ObjectType t, Page * parent );
	virtual ~FormObject();

	//! \return Type.
	ObjectType objectType() const;

	//! \return ID.
	const QString & objectId() const;
	//! Set ID.
	void setObjectId( const QString & i );

	//! \return Pen.
	virtual const QPen & objectPen() const;
	//! Set pen.
	virtual void setObjectPen( const QPen & p,
		bool pushUndoCommand = true );

	//! \return Brush.
	virtual const QBrush & objectBrush() const;
	//! Set brush.
	virtual void setObjectBrush( const QBrush & b );

	//! Position elements.
	virtual void setPosition( const QPointF & pos,
		bool pushUndoCommand = true ) = 0;

	//! \return Position of the element.
	virtual QPointF position() const = 0;

	//! \return Rectangle of the element.
	virtual QRectF rectangle() const = 0;

	//! Set rectangle.
	virtual void setRectangle( const QRectF & rect,
		bool pushUndoCommand = true ) = 0;

	//! \return Form.
	Page * form() const;

	//! Post deletion.
	virtual void postDeletion();

private:
	Q_DISABLE_COPY( FormObject )

	//! ID.
	QString m_id;
	//! Pen.
	QPen m_pen;
	//! Brush.
	QBrush m_brush;
	//! Form.
	Page * m_form;
	//! Type.
	ObjectType m_type;
}; // class FormObject

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__FORM_OBJECT_HPP__INCLUDED
