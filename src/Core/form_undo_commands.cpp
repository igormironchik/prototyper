
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
#include "form_undo_commands.hpp"

// Qt include.
#include <QTextDocument>

#include <QDebug>


namespace Prototyper {

namespace Core {

//
// UndoGroup
//

UndoGroup::UndoGroup( Page * form, const QString & id )
	:	QUndoCommand( QObject::tr( "Group" ) )
	,	m_form( form )
	,	m_id( id )
	,	m_undone( false )
{
}

void
UndoGroup::undo()
{
	m_undone = true;

	TopGui::instance()->projectWindow()->switchToSelectMode();

	auto * group = dynamic_cast< FormGroup* > ( m_form->findItem( m_id ) );

	QList< QGraphicsItem* > items = group->children();

	m_items.clear();

	foreach( QGraphicsItem * item, items )
		m_items.append( dynamic_cast< FormObject* > ( item )->objectId() );

	m_form->ungroup( group, false );
}

void
UndoGroup::redo()
{
	if( m_undone )
	{
		TopGui::instance()->projectWindow()->switchToSelectMode();

		QList< QGraphicsItem* > items;

		foreach( const QString & id, m_items )
		{
			QGraphicsItem * item = m_form->findItem( id );

			if( item )
				items.append( item );
		}

		m_form->group( items, false, m_id );
	}
}


//
// UndoUngroup
//

UndoUngroup::UndoUngroup( const QStringList & items,
	const QString & groupId, Page * form )
	:	QUndoCommand( QObject::tr( "Ungroup" ) )
	,	m_items( items )
	,	m_id( groupId )
	,	m_form( form )
	,	m_undone( false )
{
}

void
UndoUngroup::undo()
{
	m_undone = true;

	TopGui::instance()->projectWindow()->switchToSelectMode();

	QList< QGraphicsItem* > items;

	foreach( const QString & id, m_items )
		items.append( m_form->findItem( id ) );

	m_form->group( items, false, m_id );
}

void
UndoUngroup::redo()
{
	if( m_undone )
	{
		TopGui::instance()->projectWindow()->switchToSelectMode();

		auto * group = dynamic_cast< FormGroup* > ( m_form->findItem( m_id ) );

		m_form->ungroup( group, false );
	}
}


//
// UndoAddLineToPoly
//

UndoAddLineToPoly::UndoAddLineToPoly( Page * form,
	const QString & id, const QLineF & line )
	:	QUndoCommand( QObject::tr( "Add Line" ) )
	,	m_line( line )
	,	m_form( form )
	,	m_id( id )
	,	m_undone( false )
{
}

void
UndoAddLineToPoly::undo()
{
	m_undone = true;

	TopGui::instance()->projectWindow()->switchToPolylineMode();

	auto * poly = dynamic_cast< FormPolyline* > (
		m_form->findItem( m_id ) );

	poly->removeLine( m_line );

	if( poly->countOfLines() == 1 )
	{
		const QLineF line = poly->lines().first();

		m_form->deleteItems( QList< QGraphicsItem* > () << poly, false );

		auto * lineItem = dynamic_cast< FormLine* > (
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
}

void
UndoAddLineToPoly::redo()
{
	if( m_undone )
	{
		TopGui::instance()->projectWindow()->switchToPolylineMode();

		auto * lineItem = dynamic_cast< FormLine* > (
			m_form->findItem( m_id ) );

		if( lineItem )
		{
			const QLineF line = lineItem->line();

			m_form->deleteItems( QList< QGraphicsItem* > () << lineItem,
				false );

			auto * poly = dynamic_cast< FormPolyline* > (
				m_form->createElement< FormPolyline > ( m_id ) );

			poly->appendLine( line );

			poly->appendLine( m_line );

			poly->showHandles( true );

			m_form->setCurrentPolyLine( poly );
		}
		else
		{
			auto * poly = dynamic_cast< FormPolyline* > (
				m_form->findItem( m_id ) );

			poly->appendLine( m_line );

			if( poly->isClosed() )
				poly->showHandles( false );
			else
				poly->showHandles( true );
		}
	}
}


//
// UndoChangeLine
//

UndoChangeLine::UndoChangeLine( Page * form, const QString & id,
	const QLineF & oldLine, const QLineF & newLine )
	:	QUndoCommand( QObject::tr( "Change Line" ) )
	,	m_form( form )
	,	m_id( id )
	,	m_oldLine( oldLine )
	,	m_newLine( newLine )
	,	m_undone( false )
{
}

void
UndoChangeLine::undo()
{
	m_undone = true;

	auto * line = dynamic_cast< FormLine* > ( m_form->findItem( m_id ) );

	line->setLine( m_oldLine );

	line->placeHandles();

	TopGui::instance()->projectWindow()->switchToSelectMode();
}

void
UndoChangeLine::redo()
{
	if( m_undone )
	{
		auto * line = dynamic_cast< FormLine* > ( m_form->findItem( m_id ) );

		line->setLine( m_newLine );

		line->placeHandles();

		TopGui::instance()->projectWindow()->switchToSelectMode();
	}
}


//
// UndoChangePen
//

UndoChangePen::UndoChangePen( Page * form, const QString & id,
	const QPen & oldPen, const QPen & newPen )
	:	QUndoCommand( QObject::tr( "Change Pen" ) )
	,	m_form( form )
	,	m_id( id )
	,	m_oldPen( oldPen )
	,	m_newPen( newPen )
	,	m_undone( false )
{
}

void
UndoChangePen::undo()
{
	m_undone = true;

	auto * obj = dynamic_cast< FormObject* > ( m_form->findItem( m_id ) );

	if( obj )
		obj->setObjectPen( m_oldPen, false );

	TopGui::instance()->projectWindow()->switchToSelectMode();
}

void
UndoChangePen::redo()
{
	if( m_undone )
	{
		auto * obj = dynamic_cast< FormObject* > (
			m_form->findItem( m_id ) );

		if( obj )
			obj->setObjectPen( m_newPen, false );

		TopGui::instance()->projectWindow()->switchToSelectMode();
	}
}


//
// UndoChangeTextOnForm
//

UndoChangeTextOnForm::UndoChangeTextOnForm( Page * form, const QString & id )
	:	QUndoCommand( QObject::tr( "Change Text" ) )
	,	m_form( form )
	,	m_id( id )
	,	m_undone( false )
{
}

void
UndoChangeTextOnForm::undo()
{
	m_undone = true;

	auto * text = dynamic_cast< FormText* > (
		m_form->findItem( m_id ) );

	if( text )
		text->document()->undo();

	TopGui::instance()->projectWindow()->switchToSelectMode();
}

void
UndoChangeTextOnForm::redo()
{
	if( m_undone )
	{
		auto * text = dynamic_cast< FormText* > (
			m_form->findItem( m_id ) );

		if( text )
			text->document()->redo();

		TopGui::instance()->projectWindow()->switchToSelectMode();
	}
}


//
// UndoChangeTextWithOpts
//

UndoChangeTextWithOpts::UndoChangeTextWithOpts( Page * form, const QString & id,
	const Cfg::TextStyle & oldOpts, const Cfg::TextStyle & newOpts )
	:	QUndoCommand( QObject::tr( "Change Text Options" ) )
	,	m_form( form )
	,	m_id( id )
	,	m_oldOpts( oldOpts )
	,	m_newOpts( newOpts )
	,	m_undone( false )
{
}

void
UndoChangeTextWithOpts::undo()
{
	m_undone = true;

	setTextOpts( m_oldOpts );

	TopGui::instance()->projectWindow()->switchToSelectMode();
}

void
UndoChangeTextWithOpts::redo()
{
	if( m_undone )
	{
		setTextOpts( m_newOpts );

		TopGui::instance()->projectWindow()->switchToSelectMode();
	}
}

void
UndoChangeTextWithOpts::setTextOpts( const Cfg::TextStyle & opts )
{
	auto * obj = dynamic_cast< FormObject* > ( m_form->findItem( m_id ) );

	if( obj )
	{
		switch( obj->objectType() )
		{
			case FormObject::ButtonType :
			{
				auto * e = dynamic_cast< FormButton* > ( obj );

				if( e )
					e->setText( opts );
			}
				break;

			case FormObject::CheckBoxType :
			case FormObject::RadioButtonType :
			{
				auto * e = dynamic_cast< FormCheckBox* > ( obj );

				if( e )
					e->setText( opts );
			}
				break;

			case FormObject::SpinBoxType :
			{
				auto * e = dynamic_cast< FormSpinBox* > ( obj );

				if( e )
					e->setText( opts );
			}
				break;

			default :
				break;
		}
	}
}


//
// UndoChangeCheckState
//

UndoChangeCheckState::UndoChangeCheckState( Page * form, const QString & id )
	:	QUndoCommand( QObject::tr( "Change Check State" ) )
	,	m_form( form )
	,	m_id( id )
	,	m_undone( false )
	,	m_checked( false )
{
	auto * c = find();

	if( c )
		m_checked = c->isChecked();
}

void
UndoChangeCheckState::undo()
{
	m_undone = true;

	auto * c = find();

	if( c )
		c->setChecked( !m_checked );

	TopGui::instance()->projectWindow()->switchToSelectMode();
}

void
UndoChangeCheckState::redo()
{
	if( m_undone )
	{
		auto * c = find();

		if( c )
			c->setChecked( m_checked );

		TopGui::instance()->projectWindow()->switchToSelectMode();
	}
}

FormCheckBox *
UndoChangeCheckState::find() const
{
	auto * i = m_form->findItem( m_id );

	if( i )
		return dynamic_cast< FormCheckBox* > ( i );

	return nullptr;
}


//
// UndoDuplicate
//

UndoDuplicate::UndoDuplicate( Page * form, const QStringList & origIds,
	const QStringList & duplIds, int gridStep )
	:	QUndoCommand( QObject::tr( "Duplicate" ) )
	,	m_form( form )
	,	m_origIds( origIds )
	,	m_duplIds( duplIds )
	,	m_undone( false )
	,	m_gridStep( gridStep )
{
}

void
UndoDuplicate::undo()
{
	m_undone = true;

	QList< QGraphicsItem* > items;

	for( const auto & id : qAsConst( m_duplIds ) )
		items.append( m_form->findItem( id ) );

	m_form->deleteItems( items, false );

	TopGui::instance()->projectWindow()->switchToSelectMode();
}

void
UndoDuplicate::redo()
{
	if( m_undone )
	{
		m_duplIds.clear();

		for( const auto & id : qAsConst( m_origIds ) )
		{
			auto * o = dynamic_cast< FormObject* > ( m_form->findItem( id ) );

			if( o )
			{
				auto * n = o->clone();

				n->setPosition( n->position() + QPointF( m_gridStep, m_gridStep ), false );

				m_duplIds.append( n->objectId() );
			}
		}

		TopGui::instance()->projectWindow()->switchToSelectMode();
	}
}

} /* namespace Core */

} /* namespace Prototyper */
