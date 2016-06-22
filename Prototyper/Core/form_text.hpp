
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

#ifndef PROTOTYPER__CORE__FORM_TEXT_HPP__INCLUDED
#define PROTOTYPER__CORE__FORM_TEXT_HPP__INCLUDED

// Qt include.
#include <QGraphicsTextItem>
#include <QScopedPointer>

// Prototyper include.
#include "form_object.hpp"
#include "form_resizable.hpp"
#include "project_cfg.hpp"


namespace Prototyper {

namespace Core {

class Form;


//
// FormText
//

class FormTextPrivate;

//! Text on the form.
class FormText
	:	public QGraphicsTextItem
	,	public FormObject
	,	public FormResizable
{
	Q_OBJECT

public:
	FormText( const QRectF & rect, Form * form, QGraphicsItem * parent = 0 );
	~FormText();

	//! \return Type.
	static ObjectType staticObjectType()
		{ return TextType; }

	//! \return Cfg.
	Cfg::Text cfg() const;
	//! Set cfg.
	void setCfg( const Cfg::Text & c );

	//! Enable/disable editing.
	void enableEditing( bool on = true );

	//! Clear selection.
	void clearSelection();

	void paint( QPainter * painter, const QStyleOptionGraphicsItem * option,
		QWidget * widget = 0 ) Q_DECL_OVERRIDE;

	//! Position elements.
	void setPosition( const QPointF & pos ) Q_DECL_OVERRIDE;

	//! \return Position of the element.
	QPointF position() const Q_DECL_OVERRIDE;

	//! \return Rectangle of the element.
	QRectF rectangle() const Q_DECL_OVERRIDE;

	//! Set rectangle.
	void setRectangle( const QRectF & rect ) Q_DECL_OVERRIDE;

public slots:
	//! Less font size.
	void lessFontSize();
	//! More font size.
	void moreFontSize();
	//! Bold.
	void bold( bool on );
	//! Italic.
	void italic( bool on );
	//! Underline.
	void underline( bool on );
	//! Change text color.
	void changeTextColor();
	//! Clear format.
	void clearFormat();
	//! Align left.
	void alignLeft();
	//! Align center.
	void alignCenter();
	//! Align right.
	void alignRight();

private slots:
	void p_cursorChanged( const QTextCursor & cursor );

protected:
	//! Resize.
	void resize( const QRectF & rect ) Q_DECL_OVERRIDE;
	//! Move resizable.
	void moveResizable( const QPointF & delta ) Q_DECL_OVERRIDE;

protected:
	void focusOutEvent( QFocusEvent * event ) Q_DECL_OVERRIDE;

private:
	friend class FormTextPrivate;

	Q_DISABLE_COPY( FormText )

	QScopedPointer< FormTextPrivate > d;
}; // class FormText

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__FORM_TEXT_HPP__INCLUDED
