
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

#ifndef PROTOTYPER__CORE__FORM_CHECKBOX_HPP__INCLUDED
#define PROTOTYPER__CORE__FORM_CHECKBOX_HPP__INCLUDED

// Qt include.
#include <QGraphicsObject>
#include <QScopedPointer>
#include <QFont>

// Prototyper include.
#include "form_object.hpp"
#include "form_resizable.hpp"
#include "project_cfg.hpp"


namespace Prototyper {

namespace Core {

class FormCheckBox;
class FormImageHandles;


//
// FormCheckBoxPrivate
//

class FormCheckBoxPrivate {
public:
	FormCheckBoxPrivate( FormCheckBox * parent, const QRectF & rect,
		qreal defaultSize = 20.0 );
	virtual ~FormCheckBoxPrivate();

	//! Init.
	void init();
	//! Set rect.
	void setRect( const QRectF & rect );

	//! Parent.
	FormCheckBox * q;
	//! Rect.
	QRectF m_rect;
	//! Checked?
	bool m_checked;
	//! Handles.
	FormImageHandles * m_handles;
	//! Font.
	QFont m_font;
	//! Text.
	QString m_text;
	//! Width.
	qreal m_width;
}; // class FormCheckBoxPrivate


//
// FormCheckBox
//

//! Checkbox on the form.
class FormCheckBox
	:	public QGraphicsObject
	,	public FormObject
	,	public FormResizable
{
	Q_OBJECT

public:
	FormCheckBox( const QRectF & rect, Form * form, QGraphicsItem * parent = 0 );
	~FormCheckBox();

	//! Post deletion.
	void postDeletion() Q_DECL_OVERRIDE;

	void paint( QPainter * painter, const QStyleOptionGraphicsItem * option,
		QWidget * widget = 0 ) Q_DECL_OVERRIDE;

	//! Draw checkbox.
	static void draw( QPainter * painter, const QPen & pen, const QFont & font,
		const QRectF & rect, qreal width, bool isChecked, const QString & text );

	//! Set pen.
	void setObjectPen( const QPen & p ) Q_DECL_OVERRIDE;

	//! \return Cfg.
	Cfg::CheckBox cfg() const;
	//! Set cfg.
	void setCfg( const Cfg::CheckBox & c );

	//! \return Text.
	Cfg::TextStyle text() const;
	//! Set text.
	void setText( const Cfg::TextStyle & c );

	QRectF boundingRect() const Q_DECL_OVERRIDE;

	//! Position elements.
	void positionElements( const QPointF & pos ) Q_DECL_OVERRIDE;

	//! \return Position of the element.
	QPointF position() const Q_DECL_OVERRIDE;

protected:
	//! Resize.
	void resize( const QRectF & rect ) Q_DECL_OVERRIDE;
	//! Move resizable.
	void moveResizable( const QPointF & delta ) Q_DECL_OVERRIDE;

	void contextMenuEvent( QGraphicsSceneContextMenuEvent * event )
		Q_DECL_OVERRIDE;

private slots:
	//! Properties.
	void properties();

protected:
	FormCheckBox( const QRectF & rect, Form * form, qreal defaultSize,
		FormObject::ObjectType type, QGraphicsItem * parent = 0 );

protected:
	QScopedPointer< FormCheckBoxPrivate > d;

private:
	friend class FormButtonPrivate;

	Q_DISABLE_COPY( FormCheckBox )
}; // class FormCheckBox

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__FORM_CHECKBOX_HPP__INCLUDED
