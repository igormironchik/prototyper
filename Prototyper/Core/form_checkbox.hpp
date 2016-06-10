
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

// Prototyper include.
#include "form_object.hpp"
#include "form_resizable.hpp"
#include "project_cfg.hpp"


namespace Prototyper {

namespace Core {

//
// FormCheckBox
//

class FormCheckBoxPrivate;

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
	static void draw( QPainter * painter, const QPen & pen,
		const QRectF & rect, bool isChecked );

	//! Set pen.
	void setObjectPen( const QPen & p ) Q_DECL_OVERRIDE;

	//! \return Cfg.
	Cfg::CheckBox cfg() const;
	//! Set cfg.
	void setCfg( const Cfg::CheckBox & c );

	QRectF boundingRect() const Q_DECL_OVERRIDE;

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

private:
	friend class FormButtonPrivate;

	Q_DISABLE_COPY( FormCheckBox )

	QScopedPointer< FormCheckBoxPrivate > d;
}; // class FormCheckBox

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__FORM_CHECKBOX_HPP__INCLUDED
