
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
#include "project_cfg.hpp"
#include "form_actions.hpp"
#include "form_line.hpp"
#include "grid_snap.hpp"

// Qt include.
#include <QPainter>
#include <QGraphicsSceneContextMenuEvent>
#include <QMenu>
#include <QAction>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>
#include <QGraphicsSceneHoverEvent>


namespace Prototyper {

namespace Core {

//
// FormPrivate
//

class FormPrivate {
public:
	FormPrivate( Cfg::Form & cfg, Form * parent )
		:	q( parent )
		,	m_gridMode( Form::ShowGrid )
		,	m_gridStepAction( 0 )
		,	m_cfg( cfg )
		,	m_pressed( false )
		,	m_current( 0 )
		,	m_id( 0 )
		,	m_snap( 0 )
	{
	}

	//! Init.
	void init();
	//! \return Current Z-value.
	qreal currentZValue() const;
	//! \return Current Z-value.
	void currentZValue( const QList< QGraphicsItem* > & items,
		qreal & z ) const;
	//! \return Start point for line.
	QPointF lineStartPoint( const QPointF & point,
		bool & intersected ) const;
	//! Clear current lines.
	void clearCurrentLines();
	//! Handle mouse move in current lines.
	void handleMouseMoveInCurrentLines( const QPointF & point );

	//! Parent.
	Form * q;
	//! Grid mode.
	Form::GridMode m_gridMode;
	//! Grid step action.
	QAction * m_gridStepAction;
	//! Cfg.
	Cfg::Form & m_cfg;
	//! Pressed.
	bool m_pressed;
	//! Current item.
	QGraphicsItem * m_current;
	//! Mouse pos.
	QPointF m_pos;
	//! ID.
	quint64 m_id;
	//! Current lines.
	QList< FormLine* > m_currentLines;
	//! Grid snap.
	GridSnap * m_snap;
}; // class FormPrivate

void
FormPrivate::init()
{
	m_gridStepAction = new QAction( QIcon( ":/Core/img/measure.png" ),
		ProjectWindow::tr( "Grid Step" ), q );

	Form::connect( m_gridStepAction, &QAction::triggered,
		q, &Form::slotSetGridStep );

	m_snap = new GridSnap( q );
	m_snap->setGridStep( m_cfg.gridStep() );

	q->setAcceptHoverEvents( true );
}

qreal
FormPrivate::currentZValue() const
{
	qreal z = 0.0;

	currentZValue( q->childItems(), z );

	return z;
}

void
FormPrivate::currentZValue( const QList< QGraphicsItem* > & items,
	qreal & z ) const
{
	foreach( QGraphicsItem * item, items )
	{
		const QList< QGraphicsItem* > children = item->childItems();

		if( !children.isEmpty() )
			currentZValue( children, z );

		if( item->zValue() > z )
			z = item->zValue();
	}
}

QPointF
FormPrivate::lineStartPoint( const QPointF & point, bool & intersected ) const
{
	foreach( FormLine * line, m_currentLines )
	{
		const QPointF tmp = line->pointUnderHandle( point, intersected );

		if( intersected )
			return tmp;
	}

	return point;
}

void
FormPrivate::clearCurrentLines()
{
	foreach( FormLine * line, m_currentLines )
		line->showHandles( false );

	m_currentLines.clear();
}

void
FormPrivate::handleMouseMoveInCurrentLines( const QPointF & point )
{
	foreach( FormLine * line, m_currentLines )
		line->handleMouseMoveInHandles( point );
}


//
// Form
//

Form::Form( Cfg::Form & c, QGraphicsItem * parent )
	:	QGraphicsObject( parent )
	,	d( new FormPrivate( c, this ) )
{
	d->init();
}

Form::~Form()
{
}

const Cfg::Size &
Form::size() const
{
	return d->m_cfg.size();
}

void
Form::setSize( const Cfg::Size & s )
{
	d->m_cfg.setSize( s );

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
	return d->m_cfg.gridStep();
}

void
Form::setGridStep( int s )
{
	d->m_cfg.setGridStep( s );

	d->m_snap->setGridStep( s );

	update();
}

const Cfg::Form &
Form::cfg() const
{
	return d->m_cfg;
}

void
Form::switchToSelectMode()
{
	foreach( FormLine * line, d->m_currentLines )
		line->setSelected( true );

	if( d->m_current )
		d->m_current->setSelected( true );

	d->clearCurrentLines();
}

void
Form::switchToLineDrawingMode()
{
	const QList< QGraphicsItem* > selected = scene()->selectedItems();

	if( !selected.isEmpty() )
	{
		foreach( QGraphicsItem * item, selected )
		{
			FormLine * line = dynamic_cast< FormLine* > ( item );

			if( line )
			{
				line->showHandles( true );

				d->m_currentLines.append( line );
			}
		}
	}
}

void
Form::enableSnap( bool on )
{
	if( on )
		d->m_snap->show();
	else
		d->m_snap->hide();
}

GridSnap *
Form::snapItem() const
{
	return d->m_snap;
}

QRectF
Form::boundingRect() const
{
	return QRectF( 0, 0, d->m_cfg.size().width(),
		d->m_cfg.size().height() );
}

void
Form::paint( QPainter * painter, const QStyleOptionGraphicsItem * option,
	QWidget * widget )
{
	Q_UNUSED( option )
	Q_UNUSED( widget )

	const int w = d->m_cfg.size().width();
	const int h = d->m_cfg.size().height();

	painter->save();

	painter->setRenderHint( QPainter::Antialiasing, false );

	QRect r( 0, 0, w, h );

	static const QColor gridColor = Qt::gray;

	painter->setPen( gridColor );
	painter->setBrush( Qt::white );

	painter->drawRect( r );

	if( d->m_gridMode == ShowGrid )
	{
		for( int x = d->m_cfg.gridStep(); x < w; x += d->m_cfg.gridStep() )
			painter->drawLine( x, 0, x, h );

		for( int y = d->m_cfg.gridStep(); y < h; y += d->m_cfg.gridStep() )
			painter->drawLine( 0, y, w, y );
	}

	painter->restore();
}

void
Form::slotSetGridStep()
{
	TopGui::instance()->projectWindow()->setGridStep(
		d->m_cfg.gridStep(), false );
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

void
Form::mouseMoveEvent( QGraphicsSceneMouseEvent * mouseEvent )
{
	if( d->m_pressed )
	{
		const QPointF delta = mouseEvent->pos() - d->m_pos;

		d->m_pos = mouseEvent->pos();

		d->m_snap->setSnapPos( d->m_pos );

		switch( FormAction::instance()->mode() )
		{
			case FormAction::DrawPolyLine :
			{
				FormLine * line = dynamic_cast< FormLine* > ( d->m_current );

				if( line )
				{
					const QLineF l = line->line();

					line->setLine( l.p1().x(), l.p1().y(),
						l.p2().x() + delta.x(), l.p2().y() + delta.y() );

					d->handleMouseMoveInCurrentLines( mouseEvent->pos() );
				}

				mouseEvent->accept();

				return;
			}
				break;

			default :
				break;
		}
	}

	mouseEvent->ignore();
}

void
Form::mousePressEvent( QGraphicsSceneMouseEvent * mouseEvent )
{
	if( mouseEvent->button() == Qt::LeftButton )
	{
		d->m_pos = mouseEvent->pos();

		d->m_snap->setSnapPos( d->m_pos );

		switch( FormAction::instance()->mode() )
		{
			case FormAction::DrawPolyLine :
			{
				d->m_pressed = true;

				FormLine * line = new FormLine( this );

				bool intersected = false;

				QPointF p = d->lineStartPoint( mouseEvent->pos(),
					intersected );

				if( !intersected && FormAction::instance()->isSnapEnabled() )
					p = d->m_snap->snapPos();

				line->setLine( p.x(), p.y(), p.x(), p.y() );

				line->setObjectId( ++d->m_id );

				d->m_current = line;

				if( !intersected )
					d->clearCurrentLines();

				mouseEvent->accept();

				return;
			}
				break;

			default :
				break;
		}
	}

	mouseEvent->ignore();
}

void
Form::mouseReleaseEvent( QGraphicsSceneMouseEvent * mouseEvent )
{
	if( mouseEvent->button() == Qt::LeftButton && d->m_pressed )
	{
		d->m_pressed = false;

		d->m_snap->setSnapPos( mouseEvent->pos() );

		switch( FormAction::instance()->mode() )
		{
			case FormAction::DrawPolyLine :
			{
				FormLine * line = dynamic_cast< FormLine* > ( d->m_current );

				if( line )
				{
					bool intersected = false;

					QPointF p = d->lineStartPoint( mouseEvent->pos(),
						intersected );

					if( !intersected && FormAction::instance()->isSnapEnabled() )
						p = d->m_snap->snapPos();

					const QLineF l = line->line();

					line->setLine( l.p1().x(), l.p1().y(), p.x(), p.y() );

					line->showHandles( true );

					d->m_currentLines.append( line );
				}

				mouseEvent->accept();

				emit changed();

				return;
			}
				break;

			default :
				break;
		}

		d->m_current = 0;
	}

	mouseEvent->ignore();
}

void
Form::hoverEnterEvent( QGraphicsSceneHoverEvent * event )
{
	d->m_snap->setSnapPos( event->pos() );

	event->ignore();
}

void
Form::hoverMoveEvent( QGraphicsSceneHoverEvent * event )
{
	d->m_snap->setSnapPos( event->pos() );

	event->ignore();
}

} /* namespace Core */

} /* namespace Prototyper */
