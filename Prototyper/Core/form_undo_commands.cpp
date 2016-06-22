
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

UndoGroup::UndoGroup( Form * form, FormGroup * g )
	:	m_group( g )
	,	m_form( form )
{
}

UndoGroup::~UndoGroup()
{
}

void
UndoGroup::undo()
{
	TopGui::instance()->projectWindow()->switchToSelectMode();

	QList< QGraphicsItem* > items = m_group->children();

	m_id = m_group->objectId();

	m_items.clear();

	foreach( QGraphicsItem * item, items )
		m_items.append( dynamic_cast< FormObject* > ( item )->objectId() );

	m_form->ungroup( m_group );
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

	m_group = m_form->group( items );

	m_form->renameObject( m_group, m_id );
}


//
// UndoUngroup
//

UndoUngroup::UndoUngroup( const QList< QGraphicsItem* > & items,
	const QString & groupId, Form * form )
	:	m_items( items )
	,	m_group( Q_NULLPTR )
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

	m_group = m_form->group( m_items );

	m_form->renameObject( m_group, m_id );
}

void
UndoUngroup::redo()
{
	TopGui::instance()->projectWindow()->switchToSelectMode();

	m_form->ungroup( m_group );
}


//
// UndoAddLineToPoly
//

UndoAddLineToPoly::UndoAddLineToPoly( Form * form,
	const QString & id, const QLineF & line )
	:	m_poly( Q_NULLPTR )
	,	m_line( line )
	,	m_form( form )
	,	m_id( id )
	,	m_lineItem( Q_NULLPTR )
{
}

UndoAddLineToPoly::~UndoAddLineToPoly()
{
}

void
UndoAddLineToPoly::undo()
{
	m_poly = dynamic_cast< FormPolyline* > ( m_form->findItem( m_id ) );

	m_poly->removeLine( m_line );

	TopGui::instance()->projectWindow()->switchToPolylineMode();

	if( m_poly->countOfLines() == 1 )
	{
		const QLineF line = m_poly->lines().first();

		m_form->deleteItems( QList< QGraphicsItem* > () << m_poly );

		m_lineItem = dynamic_cast< FormLine* > (
			createElement< FormLine > ( m_form ) );

		m_lineItem->setLine( line );

		m_form->renameObject( m_lineItem, m_id );

		m_lineItem->showHandles( true );

		m_form->setCurrentLine( m_lineItem );
	}
	else
	{
		m_form->setCurrentPolyLine( m_poly );

		m_poly->showHandles( true );
	}
}

void
UndoAddLineToPoly::redo()
{
	TopGui::instance()->projectWindow()->switchToPolylineMode();

	if( m_lineItem )
	{
		const QLineF line = m_lineItem->line();

		m_form->deleteItems( QList< QGraphicsItem* > () << m_lineItem );

		m_lineItem = Q_NULLPTR;

		m_poly = dynamic_cast< FormPolyline* > (
			createElement< FormPolyline > ( m_form ) );

		m_form->renameObject( m_poly, m_id );

		m_poly->appendLine( line );

		m_poly->appendLine( m_line );

		m_poly->showHandles( true );
	}
	else
	{
		m_poly = dynamic_cast< FormPolyline* > ( m_form->findItem( m_id ) );

		m_poly->appendLine( m_line );

		if( m_poly->isClosed() )
			m_poly->showHandles( false );
		else
			m_poly->showHandles( true );
	}
}


//
// UndoRenameObject
//

UndoRenameObject::UndoRenameObject( Form * form, FormObject * obj,
	const QString & oldName, const QString & newName )
	:	m_form( form )
	,	m_obj( obj )
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
	m_form->renameObject( m_obj, m_oldName );
}

void
UndoRenameObject::redo()
{
	m_form->renameObject( m_obj, m_newName );
}

} /* namespace Core */

} /* namespace Prototyper */
