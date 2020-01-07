
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

#ifndef PROTOTYPER__CORE__FORM_OBJECT_PROPERTIES_HPP__INCLUDED
#define PROTOTYPER__CORE__FORM_OBJECT_PROPERTIES_HPP__INCLUDED

// Qt include.
#include <QWidget>
#include <QScopedPointer>

// Prototyper include.
#include "project_cfg.hpp"

namespace Ui {
	class ObjectProperties;
}


namespace Prototyper {

namespace Core {

class FormObject;

//
// ObjectProperties
//

class ObjectPropertiesPrivate;

//! Properties of the button on the form.
class ObjectProperties
	:	public QWidget
{
	Q_OBJECT

public:
	explicit ObjectProperties( FormObject * obj, QWidget * parent = nullptr );
	~ObjectProperties();

	Ui::ObjectProperties * ui() const;

	//! Connect properties signals/slots.
	void connectProperties();
	//! Disconnect properties signals/slots.
	void disconnectProperties();

private:
	Q_DISABLE_COPY( ObjectProperties )

	QScopedPointer< ObjectPropertiesPrivate > d;
}; // class ObjectProperties

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__FORM_OBJECT_PROPERTIES_HPP__INCLUDED
