
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

#ifndef PROTOTYPER__CORE__FORM_ACTIONS_HPP__INCLUDED
#define PROTOTYPER__CORE__FORM_ACTIONS_HPP__INCLUDED

// Qt include.
#include <QObject>
#include <QScopedPointer>


namespace Prototyper {

namespace Core {

class Form;

//
// FormAction
//

class FormActionPrivate;

//! Form action - is a singleton with current form mode and so on...
class FormAction
	:	public QObject
{
	Q_OBJECT

private:
	FormAction();
	~FormAction();

	static void cleanup();

public:
	static FormAction * instance();

	//! Mode.
	enum Mode {
		//! Select.
		Select,
		//! Move.
		Move,
		//! Insert text.
		InsertText,
		//! Draw polyline.
		DrawPolyLine
	}; // enum Mode

	//! \return Current mode.
	Mode mode() const;
	//! Set current mode.
	void setMode( Mode m );

	//! \return Current form.
	Form * form() const;
	//! Set current form.
	void setForm( Form * f );

private:
	Q_DISABLE_COPY( FormAction )

	QScopedPointer< FormActionPrivate > d;
}; // class FormAction

} /* namespace Core */

} /*  namespace Prototyper */

#endif // PROTOTYPER__CORE__FORM_ACTIONS_HPP__INCLUDED
