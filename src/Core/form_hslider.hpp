
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

#ifndef PROTOTYPER__CORE__FORM_HSLIDER_HPP__INCLUDED
#define PROTOTYPER__CORE__FORM_HSLIDER_HPP__INCLUDED

// Qt include.
#include <QGraphicsItem>

// C++ include.
#include <memory>

// Prototyper include.
#include "form_object.hpp"
#include "form_resizable.hpp"
#include "project_cfg.hpp"


namespace Prototyper {

namespace Core {

//
// FormHSlider
//

class FormHSliderPrivate;

//! Horizontal slider on the form.
class FormHSlider final
	:	public QGraphicsItem
	,	public FormObject
	,	public FormResizable
{
public:
	FormHSlider( const QRectF & rect, Page * page, QGraphicsItem * parent = 0 );
	~FormHSlider() override;

	//! \return Type.
	static ObjectType staticObjectType()
		{ return HSliderType; }

	void paint( QPainter * painter, const QStyleOptionGraphicsItem * option,
		QWidget * widget = 0 ) override;

	static void draw( QPainter * painter, const QRectF & rect,
		const QPen & pen, int dpi = 0 );

	static qreal boxHeight( int dpi = 0 );

	//! Set pen.
	void setObjectPen( const QPen & p, bool pushUndoCommand = true ) override;

	//! \return Cfg.
	Cfg::HSlider cfg() const;
	//! Set cfg.
	void setCfg( const Cfg::HSlider & c );

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

	//! Clone object.
	FormObject * clone() const override;

protected:
	//! Resize.
	void resize( const QRectF & rect ) override;
	//! Move resizable.
	void moveResizable( const QPointF & delta ) override;

private:
	friend class FormHSliderPrivate;

	Q_DISABLE_COPY( FormHSlider )

	std::unique_ptr< FormHSliderPrivate > d;
}; // class FormHSlider

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__FORM_HSLIDER_HPP__INCLUDED
