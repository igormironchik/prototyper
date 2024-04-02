
/*
	SPDX-FileCopyrightText: 2016-2024 Igor Mironchik <igor.mironchik@gmail.com>
	SPDX-License-Identifier: GPL-3.0-or-later
*/

// Prototyper include.
#include "page_view.hpp"
#include "page_scene.hpp"
#include "page.hpp"
#include "project_cfg.hpp"
#include "../constants.hpp"

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
	PageViewPrivate( const Cfg::Page & cfg, PageView * parent )
		:	q( parent )
		,	m_scene( nullptr )
		,	m_form( nullptr )
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

PageView::PageView( const Cfg::Page & cfg, QWidget * parent )
	:	QGraphicsView( parent )
	,	d( new PageViewPrivate( cfg, this ) )
{
	d->init();
}

PageView::~PageView() = default;

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

	QTransform t;
	t.scale( d->m_scale, d->m_scale );

	setTransform( t );
}

qreal
PageView::zoomFactor()
{
	return c_zoomFactor;
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

			if( qAbs( scale - c_minZoom ) < c_maxZero || qAbs( scale - c_maxZoom ) < c_maxZero )
				event->accept();
			else
			{
				setScaleValue( scale );

				ensureVisible( QRectF( event->position(),
					QSizeF( c_ensureVisibleRegion, c_ensureVisibleRegion ) ) );

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
