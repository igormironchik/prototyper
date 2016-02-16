
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
#include "form_scene.hpp"
#include "project_cfg.hpp"
#include "form_actions.hpp"
#include "top_gui.hpp"
#include "project_window.hpp"
#include "form_hierarchy_widget.hpp"
#include "form_hierarchy_model.hpp"
#include "form_object.hpp"
#include "form_actions.hpp"
#include "form.hpp"

// Qt include.
#include <QKeyEvent>
#include <QGraphicsItem>


namespace Prototyper {

namespace Core {

//
// FormScenePrivate
//

class FormScenePrivate {
public:
	FormScenePrivate( const Cfg::Form & cfg, FormScene * parent )
		:	q( parent )
		,	m_cfg( cfg )
		,	m_form( 0 )
	{
	}

	//! Init.
	void init();
	//! Move by.
	void moveBy( const QPointF & delta );

	//! Parent.
	FormScene * q;
	//! Cfg.
	const Cfg::Form & m_cfg;
	//! Form.
	Form * m_form;
}; // class FormScenePrivate;

void
FormScenePrivate::init()
{

}

void
FormScenePrivate::moveBy( const QPointF & delta )
{
	foreach( QGraphicsItem * item, q->selectedItems() )
		item->moveBy( delta.x(), delta.y() );
}



//
// FormScene
//

FormScene::FormScene( const Cfg::Form & cfg, QObject * parent )
	:	QGraphicsScene( parent )
	,	d( new FormScenePrivate( cfg, this ) )
{
	d->init();
}

FormScene::~FormScene()
{
}

Form *
FormScene::form() const
{
	return d->m_form;
}

void
FormScene::setForm( Form * f )
{
	if( d->m_form )
	{
		TopGui::instance()->projectWindow()->
			formHierarchy()->model()->removeForm( d->m_form );

		removeItem( d->m_form );

		delete d->m_form;
	}

	d->m_form = f;

	addItem( d->m_form );
}

void
FormScene::deleteSelected()
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
FormScene::keyPressEvent( QKeyEvent * event )
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

} /* namespace Core */

} /* namespace Prototyper */
