
/*
	SPDX-FileCopyrightText: 2016-2024 Igor Mironchik <igor.mironchik@gmail.com>
	SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef PROTOTYPER__CORE__FORM_ACTIONS_HPP__INCLUDED
#define PROTOTYPER__CORE__FORM_ACTIONS_HPP__INCLUDED

// Qt include.
#include <QObject>
#include <QFlags>

// C++ include.
#include <memory>

QT_BEGIN_NAMESPACE
class QColor;
QT_END_NAMESPACE


namespace Prototyper {

namespace Core {

class Page;

//
// PageAction
//

class PageActionPrivate;

//! Page action - is a singleton with current page mode and so on...
class PageAction final
	:	public QObject
{
	Q_OBJECT

private:
	PageAction();
	~PageAction();

	static void cleanup();

public:
	static PageAction * instance();

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

	//! \return Current page.
	Page * page() const;
	//! Set current page.
	void setPage( Page * f );

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
	Q_DISABLE_COPY( PageAction )

	std::unique_ptr< PageActionPrivate > d;
}; // class FormAction

Q_DECLARE_OPERATORS_FOR_FLAGS( PageAction::Flags )

} /* namespace Core */

} /*  namespace Prototyper */

#endif // PROTOTYPER__CORE__FORM_ACTIONS_HPP__INCLUDED
