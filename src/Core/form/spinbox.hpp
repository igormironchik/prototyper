
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

#ifndef PROTOTYPER__CORE__FORM_SPINBOX_HPP__INCLUDED
#define PROTOTYPER__CORE__FORM_SPINBOX_HPP__INCLUDED

// Qt include.
#include <QGraphicsObject>

// C++ include.
#include <memory>

// Prototyper include.
#include "object.hpp"
#include "resizable.hpp"
#include "project_cfg.hpp"


namespace Prototyper {

namespace Core {

class TextWithOpts;


//
// FormSpinBox
//

class FormSpinBoxPrivate;

//! Spinbox on the form.
class FormSpinBox final
	:	public QGraphicsObject
	,	public FormObject
	,	public FormResizable
{
	Q_OBJECT

public:
	FormSpinBox( const QRectF & rect, Page * page, QGraphicsItem * parent = 0 );
	~FormSpinBox() override;

	//! \return Type.
	static ObjectType staticObjectType()
		{ return SpinBoxType; }

	void paint( QPainter * painter, const QStyleOptionGraphicsItem * option,
		QWidget * widget = 0 ) override;

	static void draw( QPainter * painter, const QRectF & rect,
		const QPen & pen, const QBrush & brush, const QFont & font, const QString & text,
		int dpi = 0 );

	static qreal boxHeight( int dpi = 0 );

	//! Set pen.
	void setObjectPen( const QPen & p, bool pushUndoCommand = true ) override;
	//! Set brush.
	void setObjectBrush( const QBrush & b, bool pushUndoCommand = true ) override;

	//! \return Cfg.
	Cfg::SpinBox cfg() const;
	//! Set cfg.
	void setCfg( const Cfg::SpinBox & c );

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
	void setRectangle( const QRectF & rect, bool pushUndoCommand ) override;

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
	friend class FormSpinBoxPrivate;

	Q_DISABLE_COPY( FormSpinBox )

	std::unique_ptr< FormSpinBoxPrivate > d;
}; // class FormSpinBox

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__FORM_SPINBOX_HPP__INCLUDED
