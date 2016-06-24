
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

#ifndef PROTOTYPER__CORE__FORM_UNDO_COMMANDS_HPP__INCLUDED
#define PROTOTYPER__CORE__FORM_UNDO_COMMANDS_HPP__INCLUDED

// Qt include.
#include <QUndoCommand>
#include <QList>

// Prototyper include.
#include "form.hpp"
#include "form_object.hpp"
#include "form_button.hpp"
#include "form_checkbox.hpp"
#include "form_combobox.hpp"
#include "form_group.hpp"
#include "form_hslider.hpp"
#include "form_image.hpp"
#include "form_line.hpp"
#include "form_polyline.hpp"
#include "form_radio_button.hpp"
#include "form_rectangle.hpp"
#include "form_spinbox.hpp"
#include "form_text.hpp"
#include "form_vslider.hpp"
#include "top_gui.hpp"
#include "project_window.hpp"


QT_BEGIN_NAMESPACE
class QGraphicsItem;
QT_END_NAMESPACE


namespace Prototyper {

namespace Core {

template< class Elem >
FormObject * createElement( Form * form )
{
	switch( Elem::staticObjectType() )
	{
		case FormObject::LineType :
			return new FormLine( form, form );

		case FormObject::PolylineType :
			return new FormPolyline( form, form );

		case FormObject::TextType :
			return new FormText( QRectF(), form, form );

		case FormObject::ImageType :
			return new FormImage( form, form );

		case FormObject::RectType :
			return new FormRect( form, form );

		case FormObject::GroupType :
			return new FormGroup( form, form );

		case FormObject::ButtonType :
			return new FormButton( QRectF(), form, form );

		case FormObject::ComboBoxType :
			return new FormComboBox( QRectF(), form, form );

		case FormObject::RadioButtonType :
			return new FormRadioButton( QRectF(), form, form );

		case FormObject::CheckBoxType :
			return new FormCheckBox( QRectF(), form, form );

		case FormObject::HSliderType :
			return new FormHSlider( QRectF(), form, form );

		case FormObject::VSliderType :
			return new FormVSlider( QRectF(), form, form );

		case FormObject::SpinBoxType :
			return new FormSpinBox( QRectF(), form, form );

		default :
			return Q_NULLPTR;
	}
}


//
// UndoCreate
//

//! Undo create.
template< class Elem, class Config >
class UndoCreate
	:	public QUndoCommand
{
public:
	UndoCreate( Form * f, const QString & id )
		:	m_form( f )
		,	m_id( id )
	{
	}

	~UndoCreate()
	{
	}

	void undo() Q_DECL_OVERRIDE
	{
		TopGui::instance()->projectWindow()->switchToSelectMode();

		Elem * elem = dynamic_cast< Elem* > ( m_form->findItem( m_id ) );

		m_cfg = elem->cfg();

		m_form->deleteItems( QList< QGraphicsItem* > () << elem );
	}

	void redo() Q_DECL_OVERRIDE
	{
		TopGui::instance()->projectWindow()->switchToSelectMode();

		Elem * elem = dynamic_cast< Elem* > ( createElement< Elem >( m_form ) );

		elem->setCfg( m_cfg );
	}

private:
	//! Configuration.
	Config m_cfg;
	//! Form.
	Form * m_form;
	//! Id.
	QString m_id;
}; // class UndoCreate


//
// UndoMove
//

//! Undo move.
template< class Elem >
class UndoMove
	:	public QUndoCommand
{
public:
	UndoMove( Form * form, const QString & id, const QPointF & delta )
		:	m_id( id )
		,	m_delta( delta )
		,	m_form( form )
	{
	}

	~UndoMove()
	{
	}

	void undo() Q_DECL_OVERRIDE
	{
		TopGui::instance()->projectWindow()->switchToSelectMode();

		FormObject * elem = dynamic_cast< FormObject* > (
			m_form->findItem( m_id ) );

		elem->setPosition( elem->position() - m_delta );
	}

	void redo() Q_DECL_OVERRIDE
	{
		TopGui::instance()->projectWindow()->switchToSelectMode();

		FormObject * elem = dynamic_cast< FormObject* > (
			m_form->findItem( m_id ) );

		elem->setPosition( elem->position() + m_delta );
	}

private:
	//! Id.
	QString m_id;
	//! Delta.
	QPointF m_delta;
	//! Form.
	Form * m_form;
}; // class UndoMove


//
// UndoResize
//

//! Undo resize.
template< class Elem >
class UndoResize
	:	public QUndoCommand
{
public:
	UndoResize( Form * form, const QString & id, const QRectF & oldR,
		const QRectF & newR )
		:	m_form( form )
		,	m_id( id )
		,	m_oldRect( oldR )
		,	m_newRect( newR )
	{
	}

	~UndoResize()
	{
	}

	void undo() Q_DECL_OVERRIDE
	{
		TopGui::instance()->projectWindow()->switchToSelectMode();

		FormObject * elem = dynamic_cast< FormObject* > (
			m_form->findItem( m_id ) );

		elem->setRectangle( m_oldRect );
	}

	void redo() Q_DECL_OVERRIDE
	{
		TopGui::instance()->projectWindow()->switchToSelectMode();

		FormObject * elem = dynamic_cast< FormObject* > (
			m_form->findItem( m_id ) );

		elem->setRectangle( m_newRect );
	}

private:
	//! Form.
	Form * m_form;
	//! Id.
	QString m_id;
	//! Old rect.
	QRectF m_oldRect;
	//! New rect.
	QRectF m_newRect;
}; // class UndoResize


//
// UndoDelete
//

//! Undo delete.
template< class Elem, class Config >
class UndoDelete
	:	public QUndoCommand
{
public:
	UndoDelete( Form * form, const Config & c )
		:	m_cfg( c )
		,	m_form( form )
	{
	}

	~UndoDelete()
	{
	}

	void undo() Q_DECL_OVERRIDE
	{
		TopGui::instance()->projectWindow()->switchToSelectMode();

		Elem * elem = dynamic_cast< Elem* > ( createElement< Elem >( m_form ) );

		elem->setCfg( m_cfg );
	}

	void redo() Q_DECL_OVERRIDE
	{
		TopGui::instance()->projectWindow()->switchToSelectMode();

		QGraphicsItem * elem = m_form->findItem( m_cfg.objectId() );

		m_form->deleteItems( QList< QGraphicsItem* > () << elem );
	}

private:
	//! Configuration.
	Config m_cfg;
	//! Form.
	Form * m_form;
}; // class UndoDelete


//
// UndoGroup
//

//! Undo group.
class UndoGroup
	:	public QUndoCommand
{
public:
	UndoGroup( Form * form, const QString & id );
	~UndoGroup();

	void undo() Q_DECL_OVERRIDE;

	void redo() Q_DECL_OVERRIDE;

private:
	//! Form.
	Form * m_form;
	//! Items.
	QStringList m_items;
	//! Id.
	QString m_id;
}; // class UndoGroup


//
// UndoUngroup
//

//! Undo ungroup.
class UndoUngroup
	:	public QUndoCommand
{
public:
	UndoUngroup( const QStringList & items,
		const QString & groupId, Form * form );
	~UndoUngroup();

	void undo() Q_DECL_OVERRIDE;

	void redo() Q_DECL_OVERRIDE;

private:
	//! Items.
	QStringList m_items;
	//! Id.
	QString m_id;
	//! Form.
	Form * m_form;
}; // class UndoUngroup


//
// UndoAddLineToPoly
//

//! Undo adding line to polyline.
class UndoAddLineToPoly
	:	public QUndoCommand
{
public:
	UndoAddLineToPoly( Form * form,
		const QString & id, const QLineF & line );
	~UndoAddLineToPoly();

	void undo() Q_DECL_OVERRIDE;

	void redo() Q_DECL_OVERRIDE;

private:
	//! Line.
	QLineF m_line;
	//! Form.
	Form * m_form;
	//! Id.
	QString m_id;
}; // class UndoAddLineToPoly


//
// UndoRenameObject
//

//! Undo renaming of object.
class UndoRenameObject
	:	public QUndoCommand
{
public:
	UndoRenameObject( Form * form, const QString & oldName,
		const QString & newName );
	~UndoRenameObject();

	void undo() Q_DECL_OVERRIDE;

	void redo() Q_DECL_OVERRIDE;

private:
	//! Form.
	Form * m_form;
	//! Old name.
	QString m_oldName;
	//! New name.
	QString m_newName;
}; // class UndoRenameObject


//
// UndoChangeLine
//

//! Undo change line.
class UndoChangeLine
	:	public QUndoCommand
{
public:
	UndoChangeLine( Form * form, const QString & id, const QLineF & oldLine,
		const QLineF & newLine );
	~UndoChangeLine();

	void undo() Q_DECL_OVERRIDE;

	void redo() Q_DECL_OVERRIDE;

private:
	//! Form.
	Form * m_form;
	//! Id.
	QString m_id;
	//! Old line.
	QLineF m_oldLine;
	//! New line.
	QLineF m_newLine;
}; // class UndoChangeLine

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__FORM_UNDO_COMMANDS_HPP__INCLUDED
