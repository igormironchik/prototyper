
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
	:	m_form( form )
	,	m_id( id )
{
}

UndoGroup::~UndoGroup()
{
}

void
UndoGroup::undo()
{
	TopGui::instance()->projectWindow()->switchToSelectMode();

	FormGroup * group = dynamic_cast< FormGroup* > ( m_form->findItem( m_id ) );

	QList< QGraphicsItem* > items = group->children();

	m_items.clear();

	foreach( QGraphicsItem * item, items )
		m_items.append( dynamic_cast< FormObject* > ( item )->objectId() );

	m_form->ungroup( group );
}

void
UndoGroup::redo()
{
	TopGui::instance()->projectWindow()->switchToSelectMode();

	QList< QGraphicsItem* > items;

	foreach( const QString & id, m_items )
	{
		QGraphicsItem * item = m_form->findItem( id );

		if( item )
			items.append( item );
	}

	FormGroup * group = m_form->group( items );

	m_form->renameObject( group, m_id );
}


//
// UndoUngroup
//

UndoUngroup::UndoUngroup( const QStringList & items,
	const QString & groupId, Form * form )
	:	m_items( items )
	,	m_id( groupId )
	,	m_form( form )
{
}

UndoUngroup::~UndoUngroup()
{
}

void
UndoUngroup::undo()
{
	TopGui::instance()->projectWindow()->switchToSelectMode();

	QList< QGraphicsItem* > items;

	foreach( const QString & id, m_items )
		items.append( m_form->findItem( id ) );

	FormGroup * group = m_form->group( items );

	m_form->renameObject( group, m_id );
}

void
UndoUngroup::redo()
{
	TopGui::instance()->projectWindow()->switchToSelectMode();

	FormGroup * group = dynamic_cast< FormGroup* > ( m_form->findItem( m_id ) );

	m_form->ungroup( group );
}


//
// UndoAddLineToPoly
//

UndoAddLineToPoly::UndoAddLineToPoly( Form * form,
	const QString & id, const QLineF & line )
	:	m_line( line )
	,	m_form( form )
	,	m_id( id )
{
}

UndoAddLineToPoly::~UndoAddLineToPoly()
{
}

void
UndoAddLineToPoly::undo()
{
	FormPolyline * poly = dynamic_cast< FormPolyline* > (
		m_form->findItem( m_id ) );

	poly->removeLine( m_line );

	TopGui::instance()->projectWindow()->switchToPolylineMode();

	if( poly->countOfLines() == 1 )
	{
		const QLineF line = poly->lines().first();

		m_form->deleteItems( QList< QGraphicsItem* > () << poly );

		FormLine * lineItem = dynamic_cast< FormLine* > (
			createElement< FormLine > ( m_form ) );

		lineItem->setLine( line );

		m_form->renameObject( lineItem, m_id );

		lineItem->showHandles( true );

		m_form->setCurrentLine( lineItem );
	}
	else
	{
		m_form->setCurrentPolyLine( poly );

		poly->showHandles( true );
	}
}

void
UndoAddLineToPoly::redo()
{
	TopGui::instance()->projectWindow()->switchToPolylineMode();

	FormLine * lineItem = dynamic_cast< FormLine* > ( m_form->findItem( m_id ) );

	if( lineItem )
	{
		const QLineF line = lineItem->line();

		m_form->deleteItems( QList< QGraphicsItem* > () << lineItem );

		FormPolyline * poly = dynamic_cast< FormPolyline* > (
			createElement< FormPolyline > ( m_form ) );

		m_form->renameObject( poly, m_id );

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
}


//
// UndoRenameObject
//

UndoRenameObject::UndoRenameObject( Form * form,
	const QString & oldName, const QString & newName )
	:	m_form( form )
	,	m_oldName( oldName )
	,	m_newName( newName )
{
}

UndoRenameObject::~UndoRenameObject()
{
}

void
UndoRenameObject::undo()
{
	FormObject * obj = dynamic_cast< FormObject* > (
		m_form->findItem( m_newName ) );

	m_form->renameObject( obj, m_oldName );
}

void
UndoRenameObject::redo()
{
	FormObject * obj = dynamic_cast< FormObject* > (
		m_form->findItem( m_oldName ) );

	m_form->renameObject( obj, m_newName );
}


//
// UndoChangeLine
//

UndoChangeLine::UndoChangeLine( Form * form, const QString & id,
	const QLineF & oldLine, const QLineF & newLine )
	:	m_form( form )
	,	m_id( id )
	,	m_oldLine( oldLine )
	,	m_newLine( newLine )
{
}

UndoChangeLine::~UndoChangeLine()
{
}

void
UndoChangeLine::undo()
{
	FormLine * line = dynamic_cast< FormLine* > ( m_form->findItem( m_id ) );

	line->setLine( m_oldLine );

	line->placeHandles();
}

void
UndoChangeLine::redo()
{
	FormLine * line = dynamic_cast< FormLine* > ( m_form->findItem( m_id ) );

	line->setLine( m_newLine );

	line->placeHandles();
}

} /* namespace Core */

} /* namespace Prototyper */
