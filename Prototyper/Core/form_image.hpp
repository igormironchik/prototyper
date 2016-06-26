
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

#ifndef PROTOTYPER__CORE__FORM_Pixmap_HPP__INCLUDED
#define PROTOTYPER__CORE__FORM_Pixmap_HPP__INCLUDED

// Qt include.
#include <QGraphicsPixmapItem>
#include <QScopedPointer>

// Prototyper include.
#include "form_object.hpp"
#include "form_resizable.hpp"
#include "project_cfg.hpp"


namespace Prototyper {

namespace Core {

class Form;


//
// FormImage
//

class FormImagePrivate;

//! Pixmap on the form.
class FormImage
	:	public QGraphicsPixmapItem
	,	public FormObject
	,	public FormResizable
{
public:
	explicit FormImage( Form * form, QGraphicsItem * parent = 0 );
	~FormImage();

	//! \return Type.
	static ObjectType staticObjectType()
		{ return ImageType; }

	//! \return Cfg.
	Cfg::Image cfg() const;
	//! Set cfg.
	void setCfg( const Cfg::Image & c );

	//! \return Image.
	const QImage & image() const;
	//! Set image.
	void setImage( const QImage & img );

	void paint( QPainter * painter, const QStyleOptionGraphicsItem * option,
		QWidget * widget = 0 ) Q_DECL_OVERRIDE;

	//! Position elements.
	void setPosition( const QPointF & pos,
		bool pushUndoCommand = true ) Q_DECL_OVERRIDE;

	//! \return Position of the element.
	QPointF position() const Q_DECL_OVERRIDE;

	//! \return Rectangle of the element.
	QRectF rectangle() const Q_DECL_OVERRIDE;

	//! Set rectangle.
	void setRectangle( const QRectF & rect,
		bool pushUndoCommand = true ) Q_DECL_OVERRIDE;

protected:
	//! Resize.
	void resize( const QRectF & rect ) Q_DECL_OVERRIDE;
	//! Move resizable.
	void moveResizable( const QPointF & delta ) Q_DECL_OVERRIDE;

private:
	Q_DISABLE_COPY( FormImage )

	QScopedPointer< FormImagePrivate > d;
}; // class FormImage

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__FORM_Pixmap_HPP__INCLUDED
