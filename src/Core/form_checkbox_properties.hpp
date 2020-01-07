
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

#ifndef PROTOTYPER__CORE__FORM_CHECKBOX_PROPERTIES_HPP__INCLUDED
#define PROTOTYPER__CORE__FORM_CHECKBOX_PROPERTIES_HPP__INCLUDED

// Qt include.
#include <QWidget>
#include <QScopedPointer>

// Prototyper include.
#include "project_cfg.hpp"

namespace Ui {
	class CheckBoxProperties;
}


namespace Prototyper {

namespace Core {

//
// FormCheckBoxProperties
//

class CheckBoxPropertiesPrivate;

//! Properties of the checkbox on the form.
class CheckBoxProperties
	:	public QWidget
{
	Q_OBJECT

public:
	CheckBoxProperties( QWidget * parent = Q_NULLPTR );
	~CheckBoxProperties();

	Ui::CheckBoxProperties * ui() const;

private:
	Q_DISABLE_COPY( CheckBoxProperties )

	QScopedPointer< CheckBoxPropertiesPrivate > d;
}; // class ButtonProperties

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__FORM_CHECKBOX_PROPERTIES_HPP__INCLUDED
