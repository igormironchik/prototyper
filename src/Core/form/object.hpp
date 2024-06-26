
/*
	SPDX-FileCopyrightText: 2016-2024 Igor Mironchik <igor.mironchik@gmail.com>
	SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef PROTOTYPER__CORE__FORM_OBJECT_HPP__INCLUDED
#define PROTOTYPER__CORE__FORM_OBJECT_HPP__INCLUDED

// Qt include.
#include <QtGlobal>
#include <QPen>
#include <QBrush>

// C++ include.
#include <memory>


namespace Prototyper {

namespace Core {

class Page;
class ObjectProperties;


//
// FormObject
//

class FormObjectPrivate;

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

	enum ResizeProperty {
		ResizeWidth = 1,
		ResizeHeight = 2
	};

	FormObject( ObjectType t, Page * parent, int props = ResizeWidth | ResizeHeight );
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
	virtual void setObjectPen( const QPen & p, bool pushUndoCommand = true );

	//! \return Brush.
	virtual const QBrush & objectBrush() const;
	//! Set brush.
	virtual void setObjectBrush( const QBrush & b, bool pushUndoCommand = true );

	//! Position elements.
	virtual void setPosition( const QPointF & pos, bool pushUndoCommand = true );
	//! \return Position of the element.
	virtual QPointF position() const = 0;

	//! \return Rectangle of the element.
	virtual QRectF rectangle() const = 0;
	//! Set rectangle.
	virtual void setRectangle( const QRectF & rect, bool pushUndoCommand = true );

	//! \return Default size.
	virtual QSizeF defaultSize() const;
	//! \return Minimum allowed size.
	virtual QSizeF minimumSize() const;

	//! Update position of handles.
	void updateHandlesPos();

	//! \return Page.
	Page * page() const;

	//! Post deletion.
	virtual void postDeletion();

	//! \return Widget with properties of object.
	virtual QWidget * properties( QWidget * parent );

	//! Update values of properties.
	virtual void updatePropertiesValues();

	//! Clone object.
	virtual FormObject * clone() const = 0;

private:
	Q_DISABLE_COPY( FormObject )

	std::unique_ptr< FormObjectPrivate > d;
}; // class FormObject

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__FORM_OBJECT_HPP__INCLUDED
