
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
#include "page_view.hpp"
#include "page_scene.hpp"
#include "page.hpp"
#include "project_cfg.hpp"

// Qt include.
#include <QApplication>
#include <QWheelEvent>


namespace Prototyper {

namespace Core {

//
// FormViewPrivate
//

class PageViewPrivate {
public:
	PageViewPrivate( Cfg::Page & cfg, PageView * parent )
		:	q( parent )
		,	m_scene( 0 )
		,	m_form( 0 )
		,	m_cfg( cfg )
		,	m_scale( 1.0 )
	{
	}

	//! Init.
	void init();

	//! Parent.
	PageView * q;
	//! Scene.
	PageScene * m_scene;
	//! Form.
	Page * m_form;
	//! Cfg.
	Cfg::Page m_cfg;
	//! Scale.
	qreal m_scale;
}; // class FormViewPrivate

void
PageViewPrivate::init()
{
	q->setFrameStyle( QFrame::NoFrame );

	m_scene = new PageScene( m_cfg, q );

	q->setScene( m_scene );

	m_form = new Page( m_cfg );

	m_form->setCfg( m_cfg );

	m_scene->setPage( m_form );

	q->setRubberBandSelectionMode( Qt::ContainsItemShape );

	q->enableSelection( true );

	q->setRenderHints( QPainter::Antialiasing );

	q->setAcceptDrops( true );
}


//
// FormView
//

PageView::PageView( Cfg::Page & cfg, QWidget * parent )
	:	QGraphicsView( parent )
	,	d( new PageViewPrivate( cfg, this ) )
{
	d->init();
}

PageView::~PageView()
{
}

PageScene *
PageView::pageScene() const
{
	return d->m_scene;
}

Page *
PageView::page() const
{
	return d->m_form;
}

void
PageView::enableSelection( bool on )
{
	d->m_scene->enableSelection( on );

	if( on )
		setDragMode( QGraphicsView::RubberBandDrag );
	else
		setDragMode( QGraphicsView::NoDrag );
}

qreal
PageView::scaleValue() const
{
	return d->m_scale;
}

void
PageView::setScaleValue( qreal s )
{
	d->m_scale = s;

	QMatrix m;
	m.scale( d->m_scale, d->m_scale );

	setMatrix( m );
}

qreal
PageView::zoomFactor()
{
	return 0.05;
}

void
PageView::wheelEvent( QWheelEvent * event )
{
	if( event->modifiers() & Qt::ControlModifier )
	{
		if( event->angleDelta().y() != 0 )
		{
			const qreal dy = ( event->angleDelta().y() > 0 ? zoomFactor() : -zoomFactor() );
			const qreal scale = scaleValue() + dy;

			if( qAbs( scale - 0.1 ) < 0.01 || qAbs( scale - 4.0 ) < 0.01 )
				event->accept();
			else
			{
				setScaleValue( scale );

				ensureVisible( QRectF( event->position(), QSizeF( 10, 10 ) ) );

				event->accept();

				emit zoomChanged();
			}
		}
		else
			QGraphicsView::wheelEvent( event );
	}
	else
		QGraphicsView::wheelEvent( event );
}

} /* namespace Core */

} /* namespace Prototyper */
