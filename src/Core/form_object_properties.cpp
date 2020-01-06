
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

// Prototyper include.
#include "form_object_properties.hpp"
#include "ui_form_object_properties.h"
#include "utils.hpp"
#include "form_object.hpp"
#include "page.hpp"

// Qt include.
#include <QSpinBox>
#include <QCheckBox>


namespace Prototyper {

namespace Core {

//
// ObjectPropertiesPrivate
//

class ObjectPropertiesPrivate {
public:
	explicit ObjectPropertiesPrivate( FormObject * obj, ObjectProperties * parent )
		:	q( parent )
		,	m_obj( obj )
	{
	}

	//! Init.
	void init();

	//! Parent.
	ObjectProperties * q;
	//! Form's object.
	FormObject * m_obj;
	//! Ui.
	Ui::ObjectProperties m_ui;
}; // class ObjectPropertiesPrivate

void
ObjectPropertiesPrivate::init()
{
	m_ui.setupUi( q );
}


//
// ObjectProperties
//

ObjectProperties::ObjectProperties( FormObject * obj, QWidget * parent )
	:	QWidget( parent )
	,	d( new ObjectPropertiesPrivate( obj, this ) )
{
	d->init();
}

ObjectProperties::~ObjectProperties()
{
}

Ui::ObjectProperties *
ObjectProperties::ui() const
{
	return &d->m_ui;
}

void
ObjectProperties::connectProperties()
{
	connect( d->m_ui.m_x,
		QOverload< int >::of( &QSpinBox::valueChanged ),
		[this]( int v ) {
			d->m_obj->setPosition( QPointF( v, d->m_obj->position().y() ) );
			d->m_obj->form()->emitChanged();
		} );

	connect( d->m_ui.m_y,
		QOverload< int >::of( &QSpinBox::valueChanged ),
		[this]( int v ) {
			d->m_obj->setPosition( QPointF( d->m_obj->position().x(), v ) );
			d->m_obj->form()->emitChanged();
		} );

	connect( d->m_ui.m_width,
		QOverload< int >::of( &QSpinBox::valueChanged ),
		[this]( int v ) {
			QRectF r = d->m_obj->rectangle();
			r.setWidth( v );
			d->m_obj->setRectangle( r, true );
			d->m_obj->form()->emitChanged();
		} );

	connect( d->m_ui.m_height,
		QOverload< int >::of( &QSpinBox::valueChanged ),
		[this]( int v ) {
			QRectF r = d->m_obj->rectangle();
			r.setHeight( v );
			d->m_obj->setRectangle( r, true );
			d->m_obj->form()->emitChanged();
		} );
}

void
ObjectProperties::disconnectProperties()
{
	disconnect( d->m_ui.m_x,
		QOverload< int >::of( &QSpinBox::valueChanged ), 0, 0 );

	disconnect( d->m_ui.m_y,
		QOverload< int >::of( &QSpinBox::valueChanged ), 0, 0 );

	disconnect( d->m_ui.m_width,
		QOverload< int >::of( &QSpinBox::valueChanged ), 0, 0 );

	disconnect( d->m_ui.m_height,
		QOverload< int >::of( &QSpinBox::valueChanged ), 0, 0 );
}

} /* namespace Core */

} /* namespace Prototyper */
