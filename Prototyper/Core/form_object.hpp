
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

#ifndef PROTOTYPER__CORE__FORM_OBJECT_HPP__INCLUDED
#define PROTOTYPER__CORE__FORM_OBJECT_HPP__INCLUDED

// Qt include.
#include <QRect>

QT_BEGIN_NAMESPACE
class QPainter;
QT_END_NAMESPACE


namespace Prototyper {

namespace Core {

//
// FormObject
//

//! Object on the form.
class FormObject {
public:
	FormObject();
	virtual ~FormObject();

	FormObject( const FormObject & other );
	FormObject & operator = ( const FormObject & other );

	//! Mode.
	enum Mode {
		//! Normal.
		Normal,
		//! Selected.
		Selected
	}; // enum Mode

	//! \return Mode.
	virtual Mode mode() const;
	//! Set mode.
	virtual void setMode( Mode m );

	//! \return Z-order.
	virtual quint64 zValue() const;
	//! Set Z-order.
	virtual void setZValue( quint64 z );

	//! \return Bounding rect.
	virtual QRect boundingRect() const = 0;

	//! Paint object.
	virtual void paint( QPainter * p ) const = 0;

private:
	//! Mode.
	Mode m_mode;
	//! Z-order.
	quint64 m_z;
}; // class FormObject

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__FORM_OBJECT_HPP__INCLUDED
