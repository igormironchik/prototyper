
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
#include <QPointer>

// Prototyper include.
#include "page.hpp"
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
#include "project_cfg.hpp"


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
	UndoCreate( Page * f, const QString & id )
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

		switch( Elem::staticObjectType() )
		{
			case FormObject::LineType :
			{
				m_form->setCurrentLine( Q_NULLPTR );
			}
				break;

			case FormObject::PolylineType :
			{
				m_form->setCurrentPolyLine( Q_NULLPTR );
			}
				break;

			default :
				break;
		}

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
	Page * m_form;
	//! Id.
	QString m_id;
	//! Undone?
	bool m_undone;
}; // class UndoCreate



//
// UndoCreate
//

//! Undo create.
template<>
class UndoCreate< FormText, Cfg::Text >
	:	public QUndoCommand
{
public:
	UndoCreate( Page * f, const QString & id )
		:	QUndoCommand( QObject::tr( "Create" ) )
		,	m_form( f )
		,	m_id( id )
		,	m_undone( false )
		,	m_doc( nullptr )
	{
	}

	~UndoCreate()
	{
		if( m_doc )
			m_doc->deleteLater();
	}

	void undo() Q_DECL_OVERRIDE
	{
		m_undone = true;

		FormText * elem = dynamic_cast< FormText* > ( m_form->findItem( m_id ) );

		m_cfg = elem->cfg();

		m_form->removeDocFromMap( elem->document() );

		m_doc = elem->document();
		m_doc->setParent( nullptr );

		QObject::disconnect( m_doc, 0, m_form, 0 );

		elem->setDocument( m_doc->clone() );

		m_form->deleteItems( QList< QGraphicsItem* > () << elem, false );

		TopGui::instance()->projectWindow()->switchToSelectMode();
	}

	void redo() Q_DECL_OVERRIDE
	{
		if( m_undone )
		{
			FormText * elem = dynamic_cast< FormText* > (
				m_form->createElement< FormText >( m_id ) );

			elem->setCfg( m_cfg );

			elem->setDocument( m_doc );

			m_doc = nullptr;

			m_form->updateDocItemInMap( elem->document(), elem );

			QObject::connect( elem->document(),
				&QTextDocument::undoCommandAdded,
				m_form, &Page::undoCommandInTextAdded );

			TopGui::instance()->projectWindow()->switchToSelectMode();
		}
	}

private:
	//! Configuration.
	Cfg::Text m_cfg;
	//! Form.
	Page * m_form;
	//! Id.
	QString m_id;
	//! Undone?
	bool m_undone;
	//! Document.
	QTextDocument * m_doc;
}; // class UndoCreate


//
// UndoMove
//

//! Undo move.
class UndoMove
	:	public QUndoCommand
{
public:
	UndoMove( Page * form, const QString & id, const QPointF & delta )
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

		elem->setPosition( elem->position() - m_delta, false );

		TopGui::instance()->projectWindow()->switchToSelectMode();
	}

	void redo() Q_DECL_OVERRIDE
	{
		if( m_undone )
		{
			FormObject * elem = dynamic_cast< FormObject* > (
				m_form->findItem( m_id ) );

			elem->setPosition( elem->position() + m_delta, false );

			TopGui::instance()->projectWindow()->switchToSelectMode();
		}
	}

private:
	//! Id.
	QString m_id;
	//! Delta.
	QPointF m_delta;
	//! Form.
	Page * m_form;
	//! Undone?
	bool m_undone;
}; // class UndoMove


//
// UndoResize
//

//! Undo resize.
class UndoResize
	:	public QUndoCommand
{
public:
	UndoResize( Page * form, const QString & id, const QRectF & oldR,
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

		elem->setRectangle( m_oldRect, false );

		TopGui::instance()->projectWindow()->switchToSelectMode();
	}

	void redo() Q_DECL_OVERRIDE
	{
		if( m_undone )
		{
			FormObject * elem = dynamic_cast< FormObject* > (
				m_form->findItem( m_id ) );

			elem->setRectangle( m_newRect, false );

			TopGui::instance()->projectWindow()->switchToSelectMode();
		}
	}

private:
	//! Form.
	Page * m_form;
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
	UndoDelete( Page * form, const Config & c )
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

			switch( Elem::staticObjectType() )
			{
				case FormObject::LineType :
				{
					m_form->setCurrentLine( Q_NULLPTR );
				}
					break;

				case FormObject::PolylineType :
				{
					m_form->setCurrentPolyLine( Q_NULLPTR );
				}
					break;

				default :
					break;
			}

			TopGui::instance()->projectWindow()->switchToSelectMode();
		}
	}

private:
	//! Configuration.
	Config m_cfg;
	//! Form.
	Page * m_form;
	//! Undone?
	bool m_undone;
}; // class UndoDelete


//
// UndoCreate
//

//! Undo create.
template<>
class UndoDelete< FormText, Cfg::Text >
	:	public QUndoCommand
{
public:
	UndoDelete( Page * form, const Cfg::Text & c )
		:	QUndoCommand( QObject::tr( "Delete" ) )
		,	m_cfg( c )
		,	m_form( form )
		,	m_undone( false )
		,	m_doc( nullptr )
	{
		auto * elem = m_form->findItem( m_cfg.objectId() );
		auto * text = dynamic_cast< FormText* > ( elem );

		m_doc = text->document();
		m_doc->setParent( nullptr );

		QObject::disconnect( m_doc, 0, m_form, 0 );

		text->setDocument( m_doc->clone() );

		m_form->removeDocFromMap( m_doc );
	}

	~UndoDelete()
	{
		if( m_doc )
			m_doc->deleteLater();
	}

	void undo() Q_DECL_OVERRIDE
	{
		m_undone = true;

		FormText * elem = dynamic_cast< FormText* > (
			m_form->createElement< FormText > ( m_cfg.objectId() ) );

		elem->setCfg( m_cfg );

		elem->setDocument( m_doc );

		m_doc = nullptr;

		m_form->updateDocItemInMap( elem->document(), elem );

		QObject::connect( elem->document(),
			&QTextDocument::undoCommandAdded,
			m_form, &Page::undoCommandInTextAdded );

		TopGui::instance()->projectWindow()->switchToSelectMode();
	}

	void redo() Q_DECL_OVERRIDE
	{
		if( m_undone )
		{
			QGraphicsItem * elem = m_form->findItem( m_cfg.objectId() );

			auto * text = dynamic_cast< FormText* > ( elem );

			m_form->removeDocFromMap( text->document() );

			m_doc = text->document();
			m_doc->setParent( nullptr );

			QObject::disconnect( m_doc, 0, m_form, 0 );

			text->setDocument( m_doc->clone() );

			m_form->deleteItems( QList< QGraphicsItem* > () << elem, false );

			TopGui::instance()->projectWindow()->switchToSelectMode();
		}
	}

private:
	//! Configuration.
	Cfg::Text m_cfg;
	//! Form.
	Page * m_form;
	//! Undone?
	bool m_undone;
	//! Document.
	QTextDocument * m_doc;
}; // class UndoCreate


//
// UndoGroup
//

//! Undo group.
class UndoGroup
	:	public QUndoCommand
{
public:
	UndoGroup( Page * form, const QString & id );
	~UndoGroup();

	void undo() Q_DECL_OVERRIDE;

	void redo() Q_DECL_OVERRIDE;

private:
	//! Form.
	Page * m_form;
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
		const QString & groupId, Page * form );
	~UndoUngroup();

	void undo() Q_DECL_OVERRIDE;

	void redo() Q_DECL_OVERRIDE;

private:
	//! Items.
	QStringList m_items;
	//! Id.
	QString m_id;
	//! Form.
	Page * m_form;
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
	UndoAddLineToPoly( Page * form,
		const QString & id, const QLineF & line );
	~UndoAddLineToPoly();

	void undo() Q_DECL_OVERRIDE;

	void redo() Q_DECL_OVERRIDE;

private:
	//! Line.
	QLineF m_line;
	//! Form.
	Page * m_form;
	//! Id.
	QString m_id;
	//! Undone?
	bool m_undone;
}; // class UndoAddLineToPoly


//
// UndoChangeLine
//

//! Undo change line.
class UndoChangeLine
	:	public QUndoCommand
{
public:
	UndoChangeLine( Page * form, const QString & id, const QLineF & oldLine,
		const QLineF & newLine );
	~UndoChangeLine();

	void undo() Q_DECL_OVERRIDE;

	void redo() Q_DECL_OVERRIDE;

private:
	//! Form.
	Page * m_form;
	//! Id.
	QString m_id;
	//! Old line.
	QLineF m_oldLine;
	//! New line.
	QLineF m_newLine;
	//! Undone?
	bool m_undone;
}; // class UndoChangeLine


//
// UndoChangePen
//

//! Undo changing of pen.
class UndoChangePen
	:	public QUndoCommand
{
public:
	UndoChangePen( Page * form, const QString & id, const QPen & oldPen,
		const QPen & newPen );
	~UndoChangePen();

	void undo() Q_DECL_OVERRIDE;

	void redo() Q_DECL_OVERRIDE;

private:
	//! Form.
	Page * m_form;
	//! Id.
	QString m_id;
	//! Old pen.
	QPen m_oldPen;
	//! New pen.
	QPen m_newPen;
	//! Undone?
	bool m_undone;
}; // class UndoChangePen


//
// UndoChangeTextOnForm
//

//! Undo text changing on the form.
class UndoChangeTextOnForm
	:	public QUndoCommand
{
public:
	UndoChangeTextOnForm( Page * form, const QString & id );
	~UndoChangeTextOnForm();

	void undo() Q_DECL_OVERRIDE;

	void redo() Q_DECL_OVERRIDE;

private:
	//! Form.
	Page * m_form;
	//! Id.
	QString m_id;
	//! Undone?
	bool m_undone;
}; // class UndoChangeTextOnForm


//
// UndoChangeTextWithOpts
//

//! Undo changing text with options.
class UndoChangeTextWithOpts
	:	public QUndoCommand
{
public:
	UndoChangeTextWithOpts( Page * form, const QString & id,
		const Cfg::TextStyle & oldOpts, const Cfg::TextStyle & newOpts );
	~UndoChangeTextWithOpts();

	void undo() Q_DECL_OVERRIDE;

	void redo() Q_DECL_OVERRIDE;

private:
	//! Set text options.
	void setTextOpts( const Cfg::TextStyle & opts );

private:
	//! Form.
	Page * m_form;
	//! Id.
	QString m_id;
	//! Old options.
	Cfg::TextStyle m_oldOpts;
	//! New options.
	Cfg::TextStyle m_newOpts;
	//! Undone?
	bool m_undone;
}; // class UndoChangeTextWithOpts

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__FORM_UNDO_COMMANDS_HPP__INCLUDED
