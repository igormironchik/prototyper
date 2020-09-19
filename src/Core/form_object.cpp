
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

// Prototyper include.
#include "form_object.hpp"
#include "form_undo_commands.hpp"
#include "form_object_properties.hpp"
#include "ui_form_object_properties.h"

// Qt include.
#include <QUndoStack>


namespace Prototyper {

namespace Core {

//
// FormObjectPrivate
//

class FormObjectPrivate {
public:
	FormObjectPrivate( FormObject * parent, FormObject::ObjectType t, Page * form,
		int props )
		:	q( parent )
		,	m_id( QString::number( 0 ) )
		,	m_form( form )
		,	m_type( t )
		,	m_resizeProps( props )
	{
	}

	//! Parent.
	FormObject * q;
	//! ID.
	QString m_id;
	//! Pen.
	QPen m_pen;
	//! Brush.
	QBrush m_brush;
	//! Form.
	Page * m_form;
	//! Type.
	FormObject::ObjectType m_type;
	//! Default properties.
	QPointer< ObjectProperties > m_props;
	//! Default properties top widget.
	QPointer< QWidget > m_topProps;
	//! Enable resize in properties.
	int m_resizeProps;
}; // class FormObjectPrivate


//
// FormObject
//

FormObject::FormObject( ObjectType t, Page * parent, int props )
	:	d( new FormObjectPrivate( this, t, parent, props ) )
{
}

FormObject::~FormObject() = default;

QWidget *
FormObject::properties( QWidget * parent )
{
	d->m_topProps = new QWidget( parent );
	QVBoxLayout * layout = new QVBoxLayout( d->m_topProps );

	d->m_props = new ObjectProperties( this, d->m_topProps );

	layout->addWidget( d->m_props );
	layout->addSpacerItem( new QSpacerItem( 0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding ) );

	if( !( d->m_resizeProps & ResizeWidth ) )
	{
		d->m_props->ui()->m_width->setEnabled( false );
		d->m_props->ui()->m_width->setMinimum( 0 );
		d->m_props->ui()->m_width->setValue( rectangle().width() );
	}

	if( !( d->m_resizeProps & ResizeHeight ) )
	{
		d->m_props->ui()->m_height->setEnabled( false );
		d->m_props->ui()->m_height->setMinimum( 0 );
		d->m_props->ui()->m_height->setValue( rectangle().height() );
	}

	if( d->m_resizeProps & ResizeWidth )
	{
		d->m_props->ui()->m_width->setValue( rectangle().width() );
		d->m_props->ui()->m_width->setMinimum( minimumSize().width() );
	}

	if( d->m_resizeProps & ResizeHeight )
	{
		d->m_props->ui()->m_height->setValue( rectangle().height() );
		d->m_props->ui()->m_height->setMinimum( minimumSize().height() );
	}

	d->m_props->ui()->m_x->setValue( position().x() );
	d->m_props->ui()->m_y->setValue( position().y() );

	d->m_props->connectProperties();

	return d->m_topProps.data();
}

void
FormObject::updatePropertiesValues()
{
	if( d->m_props )
	{
		d->m_props->disconnectProperties();

		d->m_props->ui()->m_width->setValue( rectangle().width() );
		d->m_props->ui()->m_height->setValue( rectangle().height() );

		d->m_props->ui()->m_x->setValue( position().x() );
		d->m_props->ui()->m_y->setValue( position().y() );

		d->m_props->connectProperties();
	}
}

FormObject::ObjectType
FormObject::objectType() const
{
	return d->m_type;
}

const QString &
FormObject::objectId() const
{
	return d->m_id;
}

void
FormObject::setObjectId( const QString & i )
{
	d->m_id = i;
}

const QPen &
FormObject::objectPen() const
{
	return d->m_pen;
}

void
FormObject::setObjectPen( const QPen & p, bool pushUndoCommand )
{
	if( pushUndoCommand )
		d->m_form->undoStack()->push(
			new UndoChangePen( d->m_form, d->m_id, d->m_pen, p ) );

	d->m_pen = p;
}

const QBrush &
FormObject::objectBrush() const
{
	return d->m_brush;
}

void
FormObject::setObjectBrush( const QBrush & b, bool pushUndoCommand )
{
	if( pushUndoCommand )
		d->m_form->undoStack()->push(
			new UndoChangeBrush( d->m_form, d->m_id, d->m_brush, b ) );

	d->m_brush = b;
}

Page *
FormObject::page() const
{
	return d->m_form;
}

void
FormObject::postDeletion()
{
}

QSizeF
FormObject::defaultSize() const
{
	return { 1.0, 1.0 };
}

QSizeF
FormObject::minimumSize() const
{
	return defaultSize();
}

void
FormObject::updateHandlesPos()
{
	setPosition( position(), false );
}

void
FormObject::setPosition( const QPointF & pos,
	bool pushUndoCommand )
{
	if( pushUndoCommand )
		page()->undoStack()->push( new UndoMove( page(), objectId(), pos - position() ) );

	if( d->m_props )
	{
		d->m_props->disconnectProperties();
		d->m_props->ui()->m_x->setValue( pos.x() );
		d->m_props->ui()->m_y->setValue( pos.y() );
		d->m_props->connectProperties();
	}
}

void
FormObject::setRectangle( const QRectF & rect,
	bool pushUndoCommand )
{
	if( pushUndoCommand )
		page()->undoStack()->push( new UndoResize( page(), objectId(),
			rectangle(), rect ) );

	if( d->m_props )
	{
		d->m_props->disconnectProperties();
		d->m_props->ui()->m_width->setValue( rect.width() );
		d->m_props->ui()->m_height->setValue( rect.height() );
		d->m_props->connectProperties();
	}
}

} /* namespace Core */

} /* namespace Prototyper */
