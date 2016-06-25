
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
#include "form_undo_commands.hpp"


namespace Prototyper {

namespace Core {

//
// UndoGroup
//

UndoGroup::UndoGroup( Form * form, const QString & id )
	:	QUndoCommand( QObject::tr( "Group" ) )
	,	m_form( form )
	,	m_id( id )
	,	m_undone( false )
{
}

UndoGroup::~UndoGroup()
{
}

void
UndoGroup::undo()
{
	m_undone = true;

	FormGroup * group = dynamic_cast< FormGroup* > ( m_form->findItem( m_id ) );

	QList< QGraphicsItem* > items = group->children();

	m_items.clear();

	foreach( QGraphicsItem * item, items )
		m_items.append( dynamic_cast< FormObject* > ( item )->objectId() );

	m_form->ungroup( group );

	TopGui::instance()->projectWindow()->switchToSelectMode();
}

void
UndoGroup::redo()
{
	if( m_undone )
	{
		QList< QGraphicsItem* > items;

		foreach( const QString & id, m_items )
		{
			QGraphicsItem * item = m_form->findItem( id );

			if( item )
				items.append( item );
		}

		FormGroup * group = m_form->group( items );

		m_form->renameObject( group, m_id );

		TopGui::instance()->projectWindow()->switchToSelectMode();
	}
}


//
// UndoUngroup
//

UndoUngroup::UndoUngroup( const QStringList & items,
	const QString & groupId, Form * form )
	:	QUndoCommand( QObject::tr( "Ungroup" ) )
	,	m_items( items )
	,	m_id( groupId )
	,	m_form( form )
	,	m_undone( false )
{
}

UndoUngroup::~UndoUngroup()
{
}

void
UndoUngroup::undo()
{
	m_undone = true;

	QList< QGraphicsItem* > items;

	foreach( const QString & id, m_items )
		items.append( m_form->findItem( id ) );

	FormGroup * group = m_form->group( items );

	m_form->renameObject( group, m_id );

	TopGui::instance()->projectWindow()->switchToSelectMode();
}

void
UndoUngroup::redo()
{
	if( m_undone )
	{
		FormGroup * group = dynamic_cast< FormGroup* > ( m_form->findItem( m_id ) );

		m_form->ungroup( group );

		TopGui::instance()->projectWindow()->switchToSelectMode();
	}
}


//
// UndoAddLineToPoly
//

UndoAddLineToPoly::UndoAddLineToPoly( Form * form,
	const QString & id, const QLineF & line )
	:	QUndoCommand( QObject::tr( "Add Line" ) )
	,	m_line( line )
	,	m_form( form )
	,	m_id( id )
	,	m_undone( false )
{
}

UndoAddLineToPoly::~UndoAddLineToPoly()
{
}

void
UndoAddLineToPoly::undo()
{
	m_undone = true;

	FormPolyline * poly = dynamic_cast< FormPolyline* > (
		m_form->findItem( m_id ) );

	poly->removeLine( m_line );

	if( poly->countOfLines() == 1 )
	{
		const QLineF line = poly->lines().first();

		m_form->deleteItems( QList< QGraphicsItem* > () << poly, false );

		FormLine * lineItem = dynamic_cast< FormLine* > (
			m_form->createElement< FormLine > ( m_id ) );

		lineItem->setLine( line );

		lineItem->showHandles( true );

		m_form->setCurrentLine( lineItem );
	}
	else
	{
		m_form->setCurrentPolyLine( poly );

		poly->showHandles( true );
	}

	TopGui::instance()->projectWindow()->switchToPolylineMode();
}

void
UndoAddLineToPoly::redo()
{
	if( m_undone )
	{
		FormLine * lineItem = dynamic_cast< FormLine* > (
			m_form->findItem( m_id ) );

		if( lineItem )
		{
			const QLineF line = lineItem->line();

			m_form->deleteItems( QList< QGraphicsItem* > () << lineItem,
				false );

			FormPolyline * poly = dynamic_cast< FormPolyline* > (
				m_form->createElement< FormPolyline > ( m_id ) );

			poly->appendLine( line );

			poly->appendLine( m_line );

			poly->showHandles( true );
		}
		else
		{
			FormPolyline * poly = dynamic_cast< FormPolyline* > (
				m_form->findItem( m_id ) );

			poly->appendLine( m_line );

			if( poly->isClosed() )
				poly->showHandles( false );
			else
				poly->showHandles( true );
		}

		TopGui::instance()->projectWindow()->switchToPolylineMode();
	}
}


//
// UndoRenameObject
//

UndoRenameObject::UndoRenameObject( Form * form,
	const QString & oldName, const QString & newName )
	:	QUndoCommand( QObject::tr( "Rename Object" ) )
	,	m_form( form )
	,	m_oldName( oldName )
	,	m_newName( newName )
	,	m_undone( false )
{
}

UndoRenameObject::~UndoRenameObject()
{
}

void
UndoRenameObject::undo()
{
	m_undone = true;

	FormObject * obj = dynamic_cast< FormObject* > (
		m_form->findItem( m_newName ) );

	m_form->renameObject( obj, m_oldName );

	TopGui::instance()->projectWindow()->switchToSelectMode();
}

void
UndoRenameObject::redo()
{
	if( m_undone )
	{
		FormObject * obj = dynamic_cast< FormObject* > (
			m_form->findItem( m_oldName ) );

		m_form->renameObject( obj, m_newName );

		TopGui::instance()->projectWindow()->switchToSelectMode();
	}
}


//
// UndoChangeLine
//

UndoChangeLine::UndoChangeLine( Form * form, const QString & id,
	const QLineF & oldLine, const QLineF & newLine )
	:	QUndoCommand( QObject::tr( "Change Line" ) )
	,	m_form( form )
	,	m_id( id )
	,	m_oldLine( oldLine )
	,	m_newLine( newLine )
	,	m_undone( false )
{
}

UndoChangeLine::~UndoChangeLine()
{
}

void
UndoChangeLine::undo()
{
	m_undone = true;

	FormLine * line = dynamic_cast< FormLine* > ( m_form->findItem( m_id ) );

	line->setLine( m_oldLine );

	line->placeHandles();

	TopGui::instance()->projectWindow()->switchToSelectMode();
}

void
UndoChangeLine::redo()
{
	if( m_undone )
	{
		FormLine * line = dynamic_cast< FormLine* > ( m_form->findItem( m_id ) );

		line->setLine( m_newLine );

		line->placeHandles();

		TopGui::instance()->projectWindow()->switchToSelectMode();
	}
}

} /* namespace Core */

} /* namespace Prototyper */
