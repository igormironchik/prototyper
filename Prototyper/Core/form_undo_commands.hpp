
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
#include <QObject>

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
		:	QUndoCommand( QObject::tr( "Create" ) )
		,	m_form( f )
		,	m_id( id )
		,	m_undone( false )
	{
	}

	~UndoCreate()
	{
	}

	void undo() Q_DECL_OVERRIDE
	{
		m_undone = true;

		Elem * elem = dynamic_cast< Elem* > ( m_form->findItem( m_id ) );

		m_cfg = elem->cfg();

		m_form->deleteItems( QList< QGraphicsItem* > () << elem, false );

		TopGui::instance()->projectWindow()->switchToSelectMode();
	}

	void redo() Q_DECL_OVERRIDE
	{
		if( m_undone )
		{
			Elem * elem = dynamic_cast< Elem* > (
				m_form->createElement< Elem >( m_id ) );

			elem->setCfg( m_cfg );

			TopGui::instance()->projectWindow()->switchToSelectMode();
		}
	}

private:
	//! Configuration.
	Config m_cfg;
	//! Form.
	Form * m_form;
	//! Id.
	QString m_id;
	//! Undone?
	bool m_undone;
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
		:	QUndoCommand( QObject::tr( "Move" ) )
		,	m_id( id )
		,	m_delta( delta )
		,	m_form( form )
		,	m_undone( false )
	{
	}

	~UndoMove()
	{
	}

	void undo() Q_DECL_OVERRIDE
	{
		m_undone = true;

		FormObject * elem = dynamic_cast< FormObject* > (
			m_form->findItem( m_id ) );

		elem->setPosition( elem->position() - m_delta );

		TopGui::instance()->projectWindow()->switchToSelectMode();
	}

	void redo() Q_DECL_OVERRIDE
	{
		if( m_undone )
		{
			FormObject * elem = dynamic_cast< FormObject* > (
				m_form->findItem( m_id ) );

			elem->setPosition( elem->position() + m_delta );

			TopGui::instance()->projectWindow()->switchToSelectMode();
		}
	}

private:
	//! Id.
	QString m_id;
	//! Delta.
	QPointF m_delta;
	//! Form.
	Form * m_form;
	//! Undone?
	bool m_undone;
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
		:	QUndoCommand( QObject::tr( "Resize" ) )
		,	m_form( form )
		,	m_id( id )
		,	m_oldRect( oldR )
		,	m_newRect( newR )
		,	m_undone( false )
	{
	}

	~UndoResize()
	{
	}

	void undo() Q_DECL_OVERRIDE
	{
		m_undone = true;

		FormObject * elem = dynamic_cast< FormObject* > (
			m_form->findItem( m_id ) );

		elem->setRectangle( m_oldRect );

		TopGui::instance()->projectWindow()->switchToSelectMode();
	}

	void redo() Q_DECL_OVERRIDE
	{
		if( m_undone )
		{
			FormObject * elem = dynamic_cast< FormObject* > (
				m_form->findItem( m_id ) );

			elem->setRectangle( m_newRect );

			TopGui::instance()->projectWindow()->switchToSelectMode();
		}
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
	//! Undone?
	bool m_undone;
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
		:	QUndoCommand( QObject::tr( "Delete" ) )
		,	m_cfg( c )
		,	m_form( form )
		,	m_undone( false )
	{
	}

	~UndoDelete()
	{
	}

	void undo() Q_DECL_OVERRIDE
	{
		m_undone = true;

		Elem * elem = dynamic_cast< Elem* > (
			m_form->createElement< Elem > ( m_cfg.objectId() ) );

		elem->setCfg( m_cfg );

		TopGui::instance()->projectWindow()->switchToSelectMode();
	}

	void redo() Q_DECL_OVERRIDE
	{
		if( m_undone )
		{
			QGraphicsItem * elem = m_form->findItem( m_cfg.objectId() );

			m_form->deleteItems( QList< QGraphicsItem* > () << elem, false );

			TopGui::instance()->projectWindow()->switchToSelectMode();
		}
	}

private:
	//! Configuration.
	Config m_cfg;
	//! Form.
	Form * m_form;
	//! Undone?
	bool m_undone;
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
	//! Undone?
	bool m_undone;
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
	//! Undone?
	bool m_undone;
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
	//! Undone?
	bool m_undone;
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
	//! Undone?
	bool m_undone;
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
	//! Undone?
	bool m_undone;
}; // class UndoChangeLine

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__FORM_UNDO_COMMANDS_HPP__INCLUDED
