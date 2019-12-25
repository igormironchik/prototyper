
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

#ifndef PROTOTYPER__CORE__FORM_RADIOBUTTON_HPP__INCLUDED
#define PROTOTYPER__CORE__FORM_RADIOBUTTON_HPP__INCLUDED

// Prototyper include.
#include "form_checkbox.hpp"


namespace Prototyper {

namespace Core {

//
// FormRadioButton
//

//! Radio button on the form.
class FormRadioButton
	:	public FormCheckBox
{
	Q_OBJECT

public:
	FormRadioButton( const QRectF & rect, Page * form,
		QGraphicsItem * parent = 0 );
	~FormRadioButton();

	//! \return Type.
	static ObjectType staticObjectType()
		{ return RadioButtonType; }

	void paint( QPainter * painter, const QStyleOptionGraphicsItem * option,
		QWidget * widget = 0 ) Q_DECL_OVERRIDE;

	//! Draw radio button.
	static void draw( QPainter * painter, const QPen & pen, const QFont & font,
		const QRectF & rect, qreal width, bool isChecked, const QString & text );

protected:
	void contextMenuEvent( QGraphicsSceneContextMenuEvent * event )
		Q_DECL_OVERRIDE;

private slots:
	//! Properties.
	void properties();

private:
	Q_DISABLE_COPY( FormRadioButton )
}; // class FormRadioButton

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__FORM_RADIOBUTTON_HPP__INCLUDED
