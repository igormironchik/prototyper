
/*
	SPDX-FileCopyrightText: 2016-2024 Igor Mironchik <igor.mironchik@gmail.com>
	SPDX-License-Identifier: GPL-3.0-or-later
*/

// Prototyper include.
#include "undo_commands.hpp"

// Qt include.
#include <QTextDocument>


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

			m_form->setCurrentPolyLine( poly );

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
// UndoChangeBrush
//

UndoChangeBrush::UndoChangeBrush( Page * form, const QString & id,
	const QBrush & oldBrush, const QBrush & newBrush )
	:	QUndoCommand( QObject::tr( "Change Brush" ) )
	,	m_form( form )
	,	m_id( id )
	,	m_oldBrush( oldBrush )
	,	m_newBrush( newBrush )
	,	m_undone( false )
{
}

void
UndoChangeBrush::undo()
{
	m_undone = true;

	auto * obj = dynamic_cast< FormObject* > ( m_form->findItem( m_id ) );

	if( obj )
		obj->setObjectBrush( m_oldBrush, false );

	TopGui::instance()->projectWindow()->switchToSelectMode();
}

void
UndoChangeBrush::redo()
{
	if( m_undone )
	{
		auto * obj = dynamic_cast< FormObject* > (
			m_form->findItem( m_id ) );

		if( obj )
			obj->setObjectBrush( m_newBrush, false );

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

	for( const auto & id : std::as_const( m_duplIds ) )
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

		for( const auto & id : std::as_const( m_origIds ) )
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

//
// UndoChangeZ
//


UndoChangeZ::UndoChangeZ( Page * form, const ZAndIds & origZ,
	const ZAndIds & newZ )
	:	QUndoCommand( QObject::tr( "Change Z" ) )
	,	m_form( form )
	,	m_orig( origZ )
	,	m_new( newZ )
	,	m_undone( false )
{
}

void
UndoChangeZ::undo()
{
	m_undone = true;

	for( const auto & p : std::as_const( m_orig ) )
	{
		auto * i = m_form->findItem( p.first );

		if( i )
			i->setZValue( p.second );
	}

	TopGui::instance()->projectWindow()->switchToSelectMode();
}

void
UndoChangeZ::redo()
{
	if( m_undone )
	{
		for( const auto & p : std::as_const( m_new ) )
		{
			auto * i = m_form->findItem( p.first );

			if( i )
				i->setZValue( p.second );
		}

		TopGui::instance()->projectWindow()->switchToSelectMode();
	}
}


//
// UndoEditPoly
//

UndoEditPoly::UndoEditPoly( Page * form, const QLineF & oldL1, const QLineF & oldL2,
	const QLineF & newL1, const QLineF & newL2, int index,
	const QString & id )
	:	QUndoCommand( QObject::tr( "Change Polyline" ) )
	,	m_form( form )
	,	m_undone( false )
	,	m_oldL1( oldL1 )
	,	m_oldL2( oldL2 )
	,	m_newL1( newL1 )
	,	m_newL2( newL2 )
	,	m_index( index )
	,	m_id( id )
{
}

void
UndoEditPoly::undo()
{
	m_undone = true;

	auto * poly = dynamic_cast< FormPolyline* > (
		m_form->findItem( m_id ) );

	poly->moveNode( m_index, m_oldL1, m_oldL2 );

	m_form->setCurrentPolyLine( poly );

	TopGui::instance()->projectWindow()->switchToSelectMode();
}

void
UndoEditPoly::redo()
{
	if( m_undone )
	{
		auto * poly = dynamic_cast< FormPolyline* > (
			m_form->findItem( m_id ) );

		poly->moveNode( m_index, m_newL1, m_newL2 );

		m_form->setCurrentPolyLine( poly );

		TopGui::instance()->projectWindow()->switchToSelectMode();
	}
}

} /* namespace Core */

} /* namespace Prototyper */
