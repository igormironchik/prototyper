
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

#ifndef PROTOTYPER__CORE__FORM_TEXT_HPP__INCLUDED
#define PROTOTYPER__CORE__FORM_TEXT_HPP__INCLUDED

// Qt include.
#include <QGraphicsTextItem>

// C++ include.
#include <memory>

// Prototyper include.
#include "form_object.hpp"
#include "form_resizable.hpp"
#include "project_cfg.hpp"


namespace Prototyper {

namespace Core {

class Page;


//
// FormText
//

class FormTextPrivate;

//! Text on the form.
class FormText final
	:	public QGraphicsTextItem
	,	public FormObject
	,	public FormResizable
{
	Q_OBJECT

public:
	FormText( const QRectF & rect, Page * page, QGraphicsItem * parent = 0 );
	~FormText() override;

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
		QWidget * widget = 0 ) override;

	//! Position elements.
	void setPosition( const QPointF & pos, bool pushUndoCommand = true ) override;
	//! \return Position of the element.
	QPointF position() const override;

	//! \return Rectangle of the element.
	QRectF rectangle() const override;
	//! Set rectangle.
	void setRectangle( const QRectF & rect, bool pushUndoCommand = true ) override;

	//! Set object's pen.
	void setObjectPen( const QPen & p, bool pushUndoCommand = true ) override;

	//! \return Default size.
	QSizeF defaultSize() const override;
	//! \return Minimum allowed size.
	QSizeF minimumSize() const override;

	//! Clone object.
	FormObject * clone() const override;

public slots:
	//! Set font size.
	void setFontSize( int s );
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

	//! Clear edit mode.
	void clearEditMode();

private slots:
	void p_cursorChanged( const QTextCursor & cursor );
	void p_contentChanged();

protected:
	//! Resize.
	void resize( const QRectF & rect ) override;
	//! Move resizable.
	void moveResizable( const QPointF & delta ) override;

protected:
	void focusInEvent( QFocusEvent * e ) override;
	void keyPressEvent( QKeyEvent * e ) override;
	bool sceneEvent( QEvent * e ) override;
	void mouseReleaseEvent( QGraphicsSceneMouseEvent * event ) override;
	void keyReleaseEvent( QKeyEvent * event ) override;

private:
	friend class FormTextPrivate;

	Q_DISABLE_COPY( FormText )

	std::unique_ptr< FormTextPrivate > d;
}; // class FormText

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__FORM_TEXT_HPP__INCLUDED
