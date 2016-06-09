
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
#include <QFlags>

QT_BEGIN_NAMESPACE
class QColor;
QT_END_NAMESPACE


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
		//! Insert text.
		InsertText,
		//! Draw line.
		DrawLine,
		//! Draw rectangle.
		DrawRect,
		//! Draw button.
		DrawButton,
		//! Draw combobox.
		DrawComboBox,
		//! Draw radio button.
		DrawRadioButton,
		//! Draw check box.
		DrawCheckBox,
		//! Draw horizontal slider.
		DrawHSlider,
		//! Draw vertical slider.
		DrawVSlider,
		//! Draw spinbox.
		DrawSpinBox
	}; // enum Mode

	//! Falgs.
	enum Flag {
		//! Polyline.
		Polyline = 1
	}; // enum Flag

	Q_DECLARE_FLAGS( Flags, Flag )

	//! \return Flags.
	const Flags & flags() const;
	//! Set flags.
	void setFlags( const Flags & f );
	//! Set flag.
	void setFlag( Flag f );
	//! Test flag.
	bool testFlag( Flag f ) const;
	//! Clear flag.
	void clearFlag( Flag f );

	//! \return Current mode.
	Mode mode() const;
	//! Set current mode.
	void setMode( Mode m );

	//! \return Current form.
	Form * form() const;
	//! Set current form.
	void setForm( Form * f );

	//! \return Fill color.
	const QColor & fillColor() const;
	//! Set fill color.
	void setFillColor( const QColor & c );

	//! \return Stroke color.
	const QColor & strokeColor() const;
	//! Set stroke color.
	void setStrokeColor( const QColor & c );

	//! Is snap to grid enaled?
	bool isSnapEnabled() const;
	//! Enable/disable snap to grid.
	void enableSnap( bool on = true );

private:
	Q_DISABLE_COPY( FormAction )

	QScopedPointer< FormActionPrivate > d;
}; // class FormAction

Q_DECLARE_OPERATORS_FOR_FLAGS( FormAction::Flags )

} /* namespace Core */

} /*  namespace Prototyper */

#endif // PROTOTYPER__CORE__FORM_ACTIONS_HPP__INCLUDED
