
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

#ifndef PROTOTYPER__CORE__FORM_GROUP_HPP__INCLUDED
#define PROTOTYPER__CORE__FORM_GROUP_HPP__INCLUDED

// Qt include.
#include <QGraphicsItemGroup>
#include <QScopedPointer>

// Prototyper include.
#include "form_object.hpp"
#include "form_move_handle.hpp"
#include "project_cfg.hpp"


namespace Prototyper {

namespace Core {

class Form;


//
// FormGroup
//

class FormGroupPrivate;

class FormGroup
	:	public QGraphicsItemGroup
	,	public FormObject
	,	public FormWithHandle
{
public:
	explicit FormGroup( Form * form, QGraphicsItem * parent = 0 );
	~FormGroup();

	//! \return Cfg.
	Cfg::Group cfg() const;
	//! Set cfg. \warning This group must be empty.
	void setCfg( const Cfg::Group & c );

	QRectF boundingRect() const Q_DECL_OVERRIDE;

	void paint( QPainter * painter, const QStyleOptionGraphicsItem * option,
		QWidget * widget = 0 ) Q_DECL_OVERRIDE;

	//! Destroy handles.
	void desrtoyHandles();

protected:
	//! Handle moved.
	void handleMoved( const QPointF & delta, FormMoveHandle * handle )
		Q_DECL_OVERRIDE;

private:
	Q_DISABLE_COPY( FormGroup )

	QScopedPointer< FormGroupPrivate > d;
}; // class FormGroup

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__FORM_GROUP_HPP__INCLUDED
