
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

#ifndef PROTOTYPER__CORE__FORM_CHECKBOX_PRIVATE_HPP__INCLUDED
#define PROTOTYPER__CORE__FORM_CHECKBOX_PRIVATE_HPP__INCLUDED

// Qt include.
#include <QPointer>

// Prototyper include.
#include "form_resizable.hpp"
#include "form_checkbox_properties.hpp"


namespace Prototyper {

namespace Core {

class FormCheckBox;


//
// FormCheckBoxPrivate
//

class FormCheckBoxPrivate {
public:
	FormCheckBoxPrivate( FormCheckBox * parent, const QRectF & rect );
	virtual ~FormCheckBoxPrivate();

	//! Init.
	void init();
	//! Set rect.
	void setRect( const QRectF & rect );
	//! Connect properties signals.
	void connectProperties();
	//! Disconnect properties signals.
	void disconnectProperties();

	//! Parent.
	FormCheckBox * q;
	//! Rect.
	QRectF m_rect;
	//! Checked?
	bool m_checked;
	//! Handles.
	QScopedPointer< FormResizableProxy > m_handles;
	//! Font.
	QFont m_font;
	//! Text.
	QString m_text;
	//! Width.
	qreal m_width;
	//! Properties.
	QPointer< CheckBoxProperties > m_props;
}; // class FormCheckBoxPrivate

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__FORM_CHECKBOX_PRIVATE_HPP__INCLUDED
