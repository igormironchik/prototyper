
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
#include <QtGlobal>
#include <QPen>
#include <QBrush>

QT_BEGIN_NAMESPACE
class QGraphicsItem;
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

	//! \return ID.
	quint64 objectId() const;
	//! Set ID.
	void setObjectId( quint64 i );

	//! \return Pen.
	virtual const QPen & objectPen() const;
	//! Set pen.
	virtual void setObjectPen( const QPen & p );

	//! \return Brush.
	virtual const QBrush & objectBrush() const;
	//! Set brush.
	virtual void setObjectBrush( const QBrush & b );

private:
	Q_DISABLE_COPY( FormObject )

	//! ID.
	quint64 m_id;
	//! Pen.
	QPen m_pen;
	//! Brush.
	QBrush m_brush;
}; // class FormObject

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__FORM_OBJECT_HPP__INCLUDED
