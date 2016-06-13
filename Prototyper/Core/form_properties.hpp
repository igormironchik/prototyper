
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

#ifndef PROTOTYPER__CORE__FORM_PROPERTIES_HPP__INCLUDED
#define PROTOTYPER__CORE__FORM_PROPERTIES_HPP__INCLUDED

// Qt include.
#include <QDialog>
#include <QScopedPointer>
#include <QFlags>


namespace Prototyper {

namespace Core {

//
// FormProperties
//

class FormPropertiesPrivate;

//! Properties of the form.
class FormProperties
	:	public QDialog
{
	Q_OBJECT

public:
	FormProperties( QWidget * parent = Q_NULLPTR,
		Qt::WindowFlags f = Qt::WindowFlags() );
	~FormProperties();

	enum Button {
		//! Minimize.
		MinimizeButton = 1,
		//! Maximize.
		MaximizeButton = 2,
		//! Close.
		CloseButton = 4
	}; // enum Button

	Q_DECLARE_FLAGS( Buttons, Button )

	//! \return Buttons.
	Buttons buttons() const;
	//! Set buttons.
	void setButtons( Buttons b );

private:
	Q_DISABLE_COPY( FormProperties )

	QScopedPointer< FormPropertiesPrivate > d;
}; // class FormProperties

Q_DECLARE_OPERATORS_FOR_FLAGS( FormProperties::Buttons )

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__FORM_PROPERTIES_HPP__INCLUDED
