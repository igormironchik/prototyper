
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
#include "page_scene.hpp"
#include "project_cfg.hpp"
#include "form_actions.hpp"
#include "top_gui.hpp"
#include "project_window.hpp"
#include "form_object.hpp"
#include "form_actions.hpp"
#include "page.hpp"
#include "form_aspect_ratio_handle.hpp"
#include "form_move_handle.hpp"

// Qt include.
#include <QKeyEvent>
#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>


namespace Prototyper {

namespace Core {

//
// FormScenePrivate
//

class PageScenePrivate {
public:
	PageScenePrivate( const Cfg::Page & cfg, PageScene * parent )
		:	q( parent )
		,	m_cfg( cfg )
		,	m_form( 0 )
		,	m_isPressed( false )
		,	m_dist( 0.0 )
		,	m_isSelectionEnabled( true )
		,	m_isHandlePressed( false )
		,	m_wasHandleHovered( false )
	{
	}

	//! Init.
	void init();
	//! Move by.
	void moveBy( const QPointF & delta );
	//! \return Is something under cursor?
	bool isSomethingUnderMouse() const;
	//! \return Item under mouse.
	QGraphicsItem * itemUnderMouse() const;
	//! \return Is handle under mouse?
	bool isHandleUnderMouse( const QList< QGraphicsItem* > & children ) const;

	//! Parent.
	PageScene * q;
	//! Cfg.
	const Cfg::Page & m_cfg;
	//! Form.
	Page * m_form;
	//! Is left button pressed?
	bool m_isPressed;
	//! Point.
	QPointF m_pos;
	//! Distance.
	qreal m_dist;
	//! Is selection enabled.
	bool m_isSelectionEnabled;
	//! Is handle pressed?
	bool m_isHandlePressed;
	//! Was handle hovered?
	bool m_wasHandleHovered;
}; // class FormScenePrivate;

void
PageScenePrivate::init()
{
	q->setItemIndexMethod( QGraphicsScene::NoIndex );
}

void
PageScenePrivate::moveBy( const QPointF & delta )
{
	foreach( QGraphicsItem * item, q->selectedItems() )
	{
		FormObject * obj = dynamic_cast< FormObject* > ( item );

		if( obj )
			obj->setPosition( obj->position() + delta );
	}
}

bool
PageScenePrivate::isSomethingUnderMouse() const
{
	const auto children = m_form->childItems();

	for( const auto & item : children )
	{
		if( item->isUnderMouse() )
			return true;
	}

	return false;
}

QGraphicsItem *
PageScenePrivate::itemUnderMouse() const
{
	const auto children = m_form->childItems();
	QGraphicsItem * selected = nullptr;

	for( const auto & item : children )
	{
		if( item->isUnderMouse() && item->isVisible() )
		{
			if( !selected )
				selected = item;
			else
			{
				auto br1 = selected->boundingRect();
				br1.moveTopLeft( selected->pos() );
				auto br2 = item->boundingRect();
				br2.moveTopLeft( item->pos() );
				const auto r = br1.intersected( br2 ).normalized();
				const auto s = r.width() * r.height();

				const auto r1 = selected->boundingRect().normalized();
				const auto r2 = item->boundingRect().normalized();
				const auto s1 = r1.width() * r1.height();
				const auto s2 = r2.width() * r2.height();

				if( s / s2 > s / s1 )
					selected = item;
			}
		}
	}

	return selected;
}

bool
PageScenePrivate::isHandleUnderMouse( const QList< QGraphicsItem* > & children ) const
{
	for( const auto & item : children )
	{
		if( item->isUnderMouse() && item->isVisible() )
		{
			auto * h = dynamic_cast< FormMoveHandle* > ( item );
			auto * a = dynamic_cast< AspectRatioHandle* > ( item );

			if( h || a )
				return true;

			const auto sub = item->childItems();

			if( !sub.isEmpty() )
			{
				const auto subRet = isHandleUnderMouse( sub );

				if( subRet )
					return true;
			}
		}
	}

	return false;
}



//
// FormScene
//

PageScene::PageScene( const Cfg::Page & cfg, QObject * parent )
	:	QGraphicsScene( parent )
	,	d( new PageScenePrivate( cfg, this ) )
{
	d->init();
}

PageScene::~PageScene()
{
	if( d->m_form )
		disconnect( this, 0, d->m_form, 0 );
}

Page *
PageScene::page() const
{
	return d->m_form;
}

void
PageScene::setPage( Page * f )
{
	if( d->m_form )
	{
		removeItem( d->m_form );

		delete d->m_form;
	}

	d->m_form = f;

	addItem( d->m_form );
}

void
PageScene::deleteSelected()
{
	QList< QGraphicsItem* > toDelete;

	foreach( QGraphicsItem * item, selectedItems() )
	{
		if( item->parentItem() == d->m_form )
			toDelete.append( item );
	}

	clearSelection();

	d->m_form->deleteItems( toDelete );

	emit changed();
}

void
PageScene::keyPressEvent( QKeyEvent * event )
{
	switch( FormAction::instance()->mode() )
	{
		case FormAction::Select :
		{
			qreal delta = 0;

			switch( event->modifiers() )
			{
				case Qt::ShiftModifier :
					delta = d->m_cfg.gridStep();
					break;

				case Qt::AltModifier :
					delta = 1.0;
					break;

				default :
					delta = (qreal) d->m_cfg.gridStep() / 2.0;
					break;
			}

			switch( event->key() )
			{
				case Qt::Key_Escape :
				{
					clearSelection();

					event->accept();

					emit changed();
				}
					break;

				case Qt::Key_Up :
				{
					d->moveBy( QPointF( 0, -delta ) );

					event->accept();

					emit changed();
				}
					break;

				case Qt::Key_Down :
				{
					d->moveBy( QPointF( 0, delta ) );

					event->accept();

					emit changed();
				}
					break;

				case Qt::Key_Left :
				{
					d->moveBy( QPointF( -delta, 0 ) );

					event->accept();

					emit changed();
				}
					break;

				case Qt::Key_Right :
				{
					d->moveBy( QPointF( delta, 0 ) );

					event->accept();

					emit changed();
				}
					break;

				case Qt::Key_Delete :
				{
					deleteSelected();

					event->accept();
				}
					break;

				default :
					event->ignore();
			}
		}
			break;

		default :
			QGraphicsScene::keyPressEvent( event );
	}
}

void
PageScene::enableSelection( bool on )
{
	d->m_isSelectionEnabled = on;
}

void
PageScene::mouseMoveEvent( QGraphicsSceneMouseEvent * event )
{
	if( d->m_isSelectionEnabled )
	{
		d->m_dist += qAbs( ( d->m_pos - event->scenePos() ).manhattanLength() );
		d->m_pos = event->scenePos();

		bool tmpWasHovered = d->m_wasHandleHovered;

		d->m_wasHandleHovered = d->isHandleUnderMouse( d->m_form->childItems() );

		if( !d->m_isHandlePressed && !d->m_wasHandleHovered && !tmpWasHovered )
			event->accept();
		else
			QGraphicsScene::mouseMoveEvent( event );
	}
	else
		QGraphicsScene::mouseMoveEvent( event );
}

void
PageScene::mousePressEvent( QGraphicsSceneMouseEvent * event )
{
	if( d->m_isSelectionEnabled && event->button() == Qt::LeftButton && d->isSomethingUnderMouse() )
	{
		d->m_isPressed = true;
		d->m_pos = event->scenePos();
		d->m_dist = 0.0;

		if( !d->isHandleUnderMouse( d->m_form->childItems() ) )
			event->accept();
		else
		{
			d->m_isHandlePressed = true;

			QGraphicsScene::mousePressEvent( event );
		}
	}
	else
		QGraphicsScene::mousePressEvent( event );
}

void
PageScene::mouseReleaseEvent( QGraphicsSceneMouseEvent * event )
{
	if( d->m_isSelectionEnabled && d->m_isPressed && event->button() == Qt::LeftButton )
	{
		d->m_isPressed = false;

		auto * item = d->itemUnderMouse();

		if( d->m_dist < 5.0 && item && !d->m_isHandlePressed )
		{
			clearSelection();
			item->setSelected( true );
		}

		d->m_dist = 0.0;

		if( !d->m_isHandlePressed )
			event->accept();
		else
			QGraphicsScene::mouseReleaseEvent( event );

		d->m_isHandlePressed = false;
	}
	else
		QGraphicsScene::mouseReleaseEvent( event );
}

void
PageScene::mouseDoubleClickEvent( QGraphicsSceneMouseEvent * event )
{
	event->accept();
}

} /* namespace Core */

} /* namespace Prototyper */
