
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

#ifndef PROTOTYPER__CORE__FORM_IMAGE_HANDLES_HPP__INCLUDED
#define PROTOTYPER__CORE__FORM_IMAGE_HANDLES_HPP__INCLUDED

// Prototyper include.
#include "form_resizable.hpp"


namespace Prototyper {

namespace Core {

//
// FormImageHandles
//

class FormImageHandlesPrivate;

//! Image handles.
class FormImageHandles
	:	public FormResizableProxy
{
public:
	FormImageHandles( FormResizable * resizable,
		QGraphicsItem * parent, Page * form );
	~FormImageHandles();

	//! \return Is keep aspect ratio?
	bool isKeepAspectRatio() const;
	//! Set keep aspect ratio.
	void setKeepAspectRatio( bool on = true );

	QRectF boundingRect() const Q_DECL_OVERRIDE;

private:
	inline const FormImageHandlesPrivate * d_ptr() const
		{ return reinterpret_cast< const FormImageHandlesPrivate* > ( d.data() ); }
	inline FormImageHandlesPrivate * d_ptr()
		{ return reinterpret_cast< FormImageHandlesPrivate* > ( d.data() ); }

private:
	Q_DISABLE_COPY( FormImageHandles )
}; // class FormImageHandles

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__FORM_IMAGE_HANDLES_HPP__INCLUDED
