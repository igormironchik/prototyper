
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
#include "form.hpp"
#include "top_gui.hpp"
#include "project_window.hpp"

// Qt include.
#include <QPainter>
#include <QGraphicsSceneContextMenuEvent>
#include <QMenu>
#include <QAction>


namespace Prototyper {

namespace Core {

//
// FormPrivate
//

class FormPrivate {
public:
	FormPrivate( Form * parent )
		:	q( parent )
		,	m_gridMode( Form::ShowGrid )
		,	m_size( 800, 600 )
		,	m_gridStep( 20 )
		,	m_gridStepAction( 0 )
	{
	}

	//! Init.
	void init();

	//! Parent.
	Form * q;
	//! Grid mode.
	Form::GridMode m_gridMode;
	//! Size.
	QSize m_size;
	//! Grid step.
	int m_gridStep;
	//! Grid step action.
	QAction * m_gridStepAction;
}; // class FormPrivate

void
FormPrivate::init()
{
	m_gridStepAction = new QAction( QIcon( ":/Core/img/measure.png" ),
		ProjectWindow::tr( "Grid Step" ), q );

	Form::connect( m_gridStepAction, &QAction::triggered,
		q, &Form::slotSetGridStep );
}


//
// Form
//

Form::Form( QGraphicsItem * parent )
	:	QGraphicsObject( parent )
	,	d( new FormPrivate( this ) )
{
	d->init();
}

Form::~Form()
{
}

const QSize &
Form::size() const
{
	return d->m_size;
}

void
Form::setSize( const QSize & s )
{
	d->m_size = s;

	update();
}

Form::GridMode
Form::gridMode() const
{
	return d->m_gridMode;
}

void
Form::setGridMode( GridMode m )
{
	d->m_gridMode = m;

	update();
}

int
Form::gridStep() const
{
	return d->m_gridStep;
}

void
Form::setGridStep( int s )
{
	d->m_gridStep = s;

	update();
}

QRectF
Form::boundingRect() const
{
	return QRectF( 0, 0, d->m_size.width(), d->m_size.height() );
}

void
Form::paint( QPainter * painter, const QStyleOptionGraphicsItem * option,
	QWidget * widget )
{
	Q_UNUSED( option )
	Q_UNUSED( widget )

	const int w = d->m_size.width();
	const int h = d->m_size.height();

	QRect r( 0, 0, w, h );

	static const QColor gridColor = Qt::darkGray;

	painter->setPen( gridColor );
	painter->setBrush( Qt::white );

	painter->drawRect( r );

	if( d->m_gridMode == ShowGrid )
	{
		for( int x = d->m_gridStep; x < w; x += d->m_gridStep )
			painter->drawLine( x, 0, x, h );

		for( int y = d->m_gridStep; y < h; y += d->m_gridStep )
			painter->drawLine( 0, y, w, y );
	}
}

void
Form::slotSetGridStep()
{
	TopGui::instance()->projectWindow()->setGridStep( d->m_gridStep, false );
}

void
Form::contextMenuEvent( QGraphicsSceneContextMenuEvent * event )
{
	QMenu menu;
	menu.addAction( TopGui::instance()->projectWindow()->showHideGridAction() );
	menu.addAction( d->m_gridStepAction );

	menu.exec( event->screenPos() );

	event->accept();
}

} /* namespace Core */

} /* namespace Prototyper */
