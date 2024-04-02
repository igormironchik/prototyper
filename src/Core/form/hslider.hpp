
/*
	SPDX-FileCopyrightText: 2016-2024 Igor Mironchik <igor.mironchik@gmail.com>
	SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef PROTOTYPER__CORE__FORM_HSLIDER_HPP__INCLUDED
#define PROTOTYPER__CORE__FORM_HSLIDER_HPP__INCLUDED

// Qt include.
#include <QGraphicsItem>

// C++ include.
#include <memory>

// Prototyper include.
#include "object.hpp"
#include "resizable.hpp"
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
