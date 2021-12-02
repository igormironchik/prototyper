
/*!
	\file

	\author Igor Mironchik (igor.mironchik at gmail dot com).

	Copyright (c) 2016-2021 Igor Mironchik

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
class UndoCreate final
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

	void undo() override
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

	void redo() override
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
class UndoCreate< FormText, Cfg::Text > final
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

	~UndoCreate() override
	{
		if( m_doc )
			m_doc->deleteLater();
	}

	void undo() override
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

	void redo() override
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
class UndoMove final
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

	void undo() override
	{
		m_undone = true;

		FormObject * elem = dynamic_cast< FormObject* > (
			m_form->findItem( m_id ) );

		elem->setPosition( elem->position() - m_delta, false );

		TopGui::instance()->projectWindow()->switchToSelectMode();
	}

	void redo() override
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
class UndoResize final
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

	void undo() override
	{
		m_undone = true;

		FormObject * elem = dynamic_cast< FormObject* > (
			m_form->findItem( m_id ) );

		elem->setRectangle( m_oldRect, false );

		TopGui::instance()->projectWindow()->switchToSelectMode();
	}

	void redo() override
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
class UndoDelete final
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

	void undo() override
	{
		m_undone = true;

		Elem * elem = dynamic_cast< Elem* > (
			m_form->createElement< Elem > ( m_cfg.objectId() ) );

		elem->setCfg( m_cfg );

		TopGui::instance()->projectWindow()->switchToSelectMode();
	}

	void redo() override
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

//! Undo delete.
template<>
class UndoDelete< FormText, Cfg::Text > final
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

	~UndoDelete() override
	{
		if( m_doc )
			m_doc->deleteLater();
	}

	void undo() override
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

	void redo() override
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
class UndoGroup final
	:	public QUndoCommand
{
public:
	UndoGroup( Page * form, const QString & id );

	void undo() override;

	void redo() override;

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
class UndoUngroup final
	:	public QUndoCommand
{
public:
	UndoUngroup( const QStringList & items,
		const QString & groupId, Page * form );

	void undo() override;

	void redo() override;

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
class UndoAddLineToPoly final
	:	public QUndoCommand
{
public:
	UndoAddLineToPoly( Page * form,
		const QString & id, const QLineF & line );

	void undo() override;

	void redo() override;

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
class UndoChangeLine final
	:	public QUndoCommand
{
public:
	UndoChangeLine( Page * form, const QString & id, const QLineF & oldLine,
		const QLineF & newLine );

	void undo() override;

	void redo() override;

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
class UndoChangePen final
	:	public QUndoCommand
{
public:
	UndoChangePen( Page * form, const QString & id, const QPen & oldPen,
		const QPen & newPen );

	void undo() override;

	void redo() override;

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
// UndoChangeBrush
//

//! Undo changing of brush.
class UndoChangeBrush final
	:	public QUndoCommand
{
public:
	UndoChangeBrush( Page * form, const QString & id, const QBrush & oldBrush,
		const QBrush & newBrush );

	void undo() override;

	void redo() override;

private:
	//! Form.
	Page * m_form;
	//! Id.
	QString m_id;
	//! Old brush.
	QBrush m_oldBrush;
	//! New brush.
	QBrush m_newBrush;
	//! Undone?
	bool m_undone;
}; // class UndoChangeBrush


//
// UndoChangeTextOnForm
//

//! Undo text changing on the form.
class UndoChangeTextOnForm final
	:	public QUndoCommand
{
public:
	UndoChangeTextOnForm( Page * form, const QString & id );

	void undo() override;

	void redo() override;

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
class UndoChangeTextWithOpts final
	:	public QUndoCommand
{
public:
	UndoChangeTextWithOpts( Page * form, const QString & id,
		const Cfg::TextStyle & oldOpts, const Cfg::TextStyle & newOpts );

	void undo() override;

	void redo() override;

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


//
// UndoChangeCheckState
//

//! Undo changing of check state.
class UndoChangeCheckState final
	:	public QUndoCommand
{
public:
	UndoChangeCheckState( Page * form, const QString & id );

	void undo() override;

	void redo() override;

private:
	FormCheckBox * find() const;

private:
	//! Form.
	Page * m_form;
	//! Id.
	QString m_id;
	//! Undone?
	bool m_undone;
	//! Checked?
	bool m_checked;
}; // class UndoChangeCheckState


//
// UndoDuplicate
//

//! Undo duplicate.
class UndoDuplicate final
	:	public QUndoCommand
{
public:
	UndoDuplicate( Page * form, const QStringList & origIds,
		const QStringList & duplIds, int gridStep );

	void undo() override;

	void redo() override;

private:
	//! Form.
	Page * m_form;
	//! Original ids.
	QStringList m_origIds;
	//! Duplicated ids.
	QStringList m_duplIds;
	//! Undone?
	bool m_undone;
	//! Grid step.
	int m_gridStep;
}; // class UndoDuplicate


//
// UndoChangeZ
//

//! Undo changing of Z index.
class UndoChangeZ final
	:	public QUndoCommand
{
public:
	using ZAndIds = QVector< QPair< QString, qreal > >;

	UndoChangeZ( Page * form, const ZAndIds & origZ,
		const ZAndIds & newZ );

	void undo() override;

	void redo() override;

private:
	//! Form.
	Page * m_form;
	//! Original Z.
	ZAndIds m_orig;
	//! New Z.
	ZAndIds m_new;
	//! Undone?
	bool m_undone;
}; // class UndoChangeZ


//
// UndoEditPoly
//

//! Undo editing of polyline.
class UndoEditPoly final
	:	public QUndoCommand
{
public:
	UndoEditPoly( Page * form, const QLineF & oldL1, const QLineF & oldL2,
		const QLineF & newL1, const QLineF & newL2, int index,
		const QString & id );

	void undo() override;

	void redo() override;

private:
	//! Form.
	Page * m_form;
	//! Undone?
	bool m_undone;
	//! Old line 1.
	QLineF m_oldL1;
	//! Old line 2.
	QLineF m_oldL2;
	//! New line 1.
	QLineF m_newL1;
	//! New line 2.
	QLineF m_newL2;
	//! Node index.
	int m_index;
	//! Polyline ID.
	QString m_id;
}; // class UndoEditPoly

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__FORM_UNDO_COMMANDS_HPP__INCLUDED
