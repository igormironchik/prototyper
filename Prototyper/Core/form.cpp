
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
#include "project_widget.hpp"
#include "project_cfg.hpp"
#include "form_actions.hpp"
#include "form_line.hpp"
#include "form_grid_snap.hpp"
#include "form_polyline.hpp"
#include "form_group.hpp"
#include "form_rect_placer.hpp"
#include "form_text.hpp"
#include "form_image.hpp"
#include "form_hierarchy_model.hpp"
#include "top_gui.hpp"
#include "project_window.hpp"
#include "form_hierarchy_widget.hpp"
#include "name_dlg.hpp"

// Qt include.
#include <QPainter>
#include <QGraphicsSceneContextMenuEvent>
#include <QMenu>
#include <QAction>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>
#include <QGraphicsSceneHoverEvent>
#include <QApplication>
#include <QTextCursor>
#include <QGraphicsSceneDragDropEvent>
#include <QMimeData>
#include <QImage>
#include <QVariant>
#include <QGraphicsView>

// C++ include.
#include <algorithm>


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
		,	m_polyline( false )
		,	m_currentPoly( 0 )
		,	m_model( 0 )
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
		bool & intersected, bool & intersectedEnds,
		FormLine* & intersectedLine ) const;
	//! Clear current lines.
	void clearCurrentLines();
	//! Handle mouse move in current lines.
	void handleMouseMoveInCurrentLines( const QPointF & point );
	//! Handle mouse move in current polyline.
	void handleMouseMoveInCurrentPolyLine( const QPointF & point );
	//! Ungroup.
	void ungroup( QGraphicsItem * group );
	//! \return Next ID.
	QString id();
	//! Update form from the configuration.
	void updateFromCfg();
	//! Clear form.
	void clear();
	//! Create line.
	FormLine * createLine( const Cfg::Line & cfg );
	//! Create polyline.
	FormPolyline * createPolyline( const Cfg::Polyline & cfg );
	//! Create text.
	FormText * createText( const Cfg::Text & cfg );
	//! Create image.
	FormImage * createImage( const Cfg::Image & cfg );
	//! Create group.
	FormGroup * createGroup( const Cfg::Group & cfg );
	//! Clear IDs.
	void clearIds( FormGroup * group );
	//! Add IDs.
	void addIds( FormGroup * group );

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
	//! Make polyline.
	bool m_polyline;
	//! Current polyline.
	FormPolyline * m_currentPoly;
	//! Hierarchy model.
	FormHierarchyModel * m_model;
	//! IDs
	QStringList m_ids;
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

	q->setAcceptDrops( true );

	m_model = TopGui::instance()->projectWindow()->formHierarchy()->model();
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

		FormObject * obj = dynamic_cast< FormObject* > ( item );

		if( obj && item->zValue() > z )
			z = item->zValue();
	}
}

QPointF
FormPrivate::lineStartPoint( const QPointF & point, bool & intersected,
	bool & intersectedEnds, FormLine* & intersectedLine ) const
{
	if( m_currentPoly )
	{
		const QPointF tmp =
			m_currentPoly->pointUnderHandle( point, intersected );

		intersectedEnds = intersected;

		return tmp;
	}
	else
	{
		foreach( FormLine * line, m_currentLines )
		{
			const QPointF tmp = line->pointUnderHandle( point, intersected,
				intersectedEnds );

			if( intersected )
			{
				return tmp;

				intersectedLine = line;
			}
		}
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

void
FormPrivate::handleMouseMoveInCurrentPolyLine( const QPointF & point )
{
	m_currentPoly->handleMouseMoveInHandles( point );
}

void
FormPrivate::ungroup( QGraphicsItem * group )
{
	FormGroup * tmp = dynamic_cast< FormGroup* > ( group );

	if( tmp )
	{
		QList< QGraphicsItem* > items = tmp->childItems();

		foreach( QGraphicsItem * item, items )
		{
			FormObject * obj = dynamic_cast< FormObject* > ( item );

			if( obj )
				m_model->removeObject( obj, tmp );

			tmp->removeFromGroup( item );

			if( obj )
			{
				m_model->endRemoveObject();

				QApplication::processEvents();

				m_model->addObject( obj, q );
			}

			if( FormAction::instance()->mode() == FormAction::Select )
			{
				item->setFlag( QGraphicsItem::ItemIsSelectable, true );

				item->setSelected( true );
			}
		}

		m_model->removeObject( tmp, q );

		q->scene()->removeItem( tmp );

		m_model->endRemoveObject();

		tmp->destroyHandles();

		delete tmp;
	}
}

QString
FormPrivate::id()
{
	while( m_ids.contains( QString::number( ++m_id ) ) )
	{
	}

	return QString::number( m_id );
}

void
FormPrivate::updateFromCfg()
{
	clear();

	QApplication::processEvents();

	q->setSize( m_cfg.size() );

	q->setGridStep( m_cfg.gridStep() );

	q->setObjectId( m_cfg.tabName() );

	TopGui::instance()->projectWindow()->formHierarchy()->view()->update(
		m_model->index( q ) );

	foreach( const Cfg::Line & c, m_cfg.line() )
	{
		FormLine * line = createLine( c );

		m_model->addObject( line, q );
	}

	foreach( const Cfg::Polyline & c, m_cfg.polyline() )
	{
		FormPolyline * line = createPolyline( c );

		m_model->addObject( line, q );
	}

	foreach( const Cfg::Text & c, m_cfg.text() )
	{
		FormText * text = createText( c );

		m_model->addObject( text, q );
	}

	foreach( const Cfg::Image & c, m_cfg.image() )
	{
		FormImage * image = createImage( c );

		m_model->addObject( image, q );
	}

	foreach( const Cfg::Group & c, m_cfg.group() )
	{
		FormGroup * group = createGroup( c );

		m_model->addObject( group, q );
	}
}

void
FormPrivate::clear()
{
	m_ids.clear();

	QList< QGraphicsItem* > items = q->childItems();

	QList< FormObject* > objs;

	foreach( QGraphicsItem * item, items )
	{
		FormObject * obj = dynamic_cast< FormObject* > ( item );

		if( obj )
			objs.append( obj );
		else
		{
			GridSnap * snap = dynamic_cast< GridSnap* > ( item );

			if( !snap )
				q->scene()->removeItem( item );
			else
				items.removeOne( item );
		}
	}

	std::reverse( objs.begin(), objs.end() );

	foreach( FormObject * obj, objs )
	{
		m_model->removeObject( obj, q );

		QGraphicsItem * item = dynamic_cast< QGraphicsItem* > ( obj );

		if( item )
			q->scene()->removeItem( item );

		m_model->endRemoveObject();
	}

	foreach( QGraphicsItem * item, items )
		delete item;
}

FormLine *
FormPrivate::createLine( const Cfg::Line & cfg )
{
	FormLine * line = new FormLine( q, q );

	line->setCfg( cfg );

	m_ids.append( line->objectId() );

	return line;
}

FormPolyline *
FormPrivate::createPolyline( const Cfg::Polyline & cfg )
{
	FormPolyline * poly = new FormPolyline( q, q );

	poly->setCfg( cfg );

	m_ids.append( poly->objectId() );

	return poly;
}

FormText *
FormPrivate::createText( const Cfg::Text & cfg )
{
	const QRectF r( 0.0, 0.0, cfg.textWidth(), 0.0 );

	FormText * text = new FormText( r, q, q );

	text->setCfg( cfg );

	m_ids.append( text->objectId() );

	return text;
}

FormImage *
FormPrivate::createImage( const Cfg::Image & cfg )
{
	FormImage * image = new FormImage( q, q );

	image->setCfg( cfg );

	m_ids.append( image->objectId() );

	return image;
}

FormGroup *
FormPrivate::createGroup( const Cfg::Group & cfg )
{
	FormGroup * group = new FormGroup( q, q );

	group->setCfg( cfg );

	addIds( group );

	return group;
}

void
FormPrivate::clearIds( FormGroup * group )
{
	foreach( QGraphicsItem * item, group->childItems() )
	{
		FormObject * obj = dynamic_cast< FormObject* > ( item );

		if( obj )
		{
			m_ids.removeOne( obj->objectId() );

			FormGroup * childGroup = dynamic_cast< FormGroup* > ( item );

			if( childGroup )
				clearIds( childGroup );
		}
	}
}

void
FormPrivate::addIds( FormGroup * group )
{
	foreach( QGraphicsItem * item, group->childItems() )
	{
		FormObject * obj = dynamic_cast< FormObject* > ( item );

		if( obj )
		{
			m_ids.append( obj->objectId() );

			FormGroup * childGroup = dynamic_cast< FormGroup* > ( item );

			if( childGroup )
				addIds( childGroup );
		}
	}
}


//
// Form
//

Form::Form( Cfg::Form & c, QGraphicsItem * parent )
	:	QGraphicsObject( parent )
	,	FormObject( FormObject::FormType, this )
	,	d( 0 )
{
	QScopedPointer< FormPrivate > tmp( new FormPrivate( c, this ) );

	tmp->init();

	d.swap( tmp );

	d->m_model->addForm( this );
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

Cfg::Form
Form::cfg() const
{
	Cfg::Form c = d->m_cfg;

	c.line().clear();
	c.polyline().clear();
	c.text().clear();
	c.image().clear();
	c.group().clear();

	foreach( QGraphicsItem * item, childItems() )
	{
		FormObject * obj = dynamic_cast< FormObject* > ( item );

		if( obj )
		{
			switch( obj->type() )
			{
				case FormObject::LineType :
				{
					FormLine * line = dynamic_cast< FormLine* > ( item );

					if( line )
						c.line().append( line->cfg() );
				}
					break;

				case FormObject::PolylineType :
				{
					FormPolyline * poly = dynamic_cast< FormPolyline* > ( item );

					if( poly )
						c.polyline().append( poly->cfg() );
				}
					break;

				case FormObject::TextType :
				{
					FormText * text = dynamic_cast< FormText* > ( item );

					if( text )
						c.text().append( text->cfg() );
				}
					break;

				case FormObject::ImageType :
				{
					FormImage * image = dynamic_cast< FormImage* > ( item );

					if( image )
						c.image().append( image->cfg() );
				}
					break;

				case FormObject::GroupType :
				{
					FormGroup * group = dynamic_cast< FormGroup* > ( item );

					if( group )
						c.group().append( group->cfg() );
				}
					break;

				default :
					break;
			}
		}
	}

	c.setTabName( objectId() );

	return c;
}

void
Form::setCfg( const Cfg::Form & c )
{
	d->m_cfg = c;

	d->updateFromCfg();
}

void
Form::switchToSelectMode()
{
	foreach( FormLine * line, d->m_currentLines )
		line->setSelected( true );

	if( d->m_current )
		d->m_current->setSelected( true );

	if( d->m_currentPoly )
	{
		d->m_currentPoly->setSelected( true );
		d->m_currentPoly->showHandles( false );
	}

	d->clearCurrentLines();
}

void
Form::switchToLineDrawingMode()
{
	d->m_currentLines.clear();
	d->m_current = 0;
	d->m_currentPoly = 0;
	d->m_polyline = false;
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

void
Form::group()
{
	QList< QGraphicsItem* > items =
		scene()->selectedItems();

	FormGroup * group = 0;

	if( items.size() > 1 )
	{
		group = new FormGroup( this, this );

		const QString id = d->id();

		group->setObjectId( id );

		d->m_ids.append( id );

		d->m_model->addObject( group, this );

		foreach( QGraphicsItem * item, items )
		{
			item->setFlag( QGraphicsItem::ItemIsSelectable, false );

			item->setSelected( false );

			if( item->parentItem() == this )
			{
				group->addToGroup( item );

				FormObject * obj = dynamic_cast< FormObject* > ( item );

				if( obj )
					d->m_model->addObject( obj, group );
			}
		}
	}
	else if( d->m_currentLines.size() > 1 )
	{
		group = new FormGroup( this, this );

		const QString id = d->id();

		group->setObjectId( id );

		d->m_ids.append( id );

		d->m_model->addObject( group, this );

		foreach( FormLine * line, d->m_currentLines )
		{
			line->setFlag( QGraphicsItem::ItemIsSelectable, false );

			line->setSelected( false );

			group->addToGroup( line );

			d->m_model->addObject( line, group );
		}

		d->m_currentLines.clear();
	}

	if( group )
	{
		d->m_current = group;

		if( FormAction::instance()->mode() == FormAction::Select )
		{
			group->setFlag( QGraphicsItem::ItemIsSelectable );

			// Workaround for stay group selected.
			// Without this processEvents() after selecting group
			// its selected and next unknown to me event clears selection.
			QApplication::processEvents();

			group->setSelected( true );
		}
	}
}

void
Form::ungroup()
{
	QList< QGraphicsItem* > items =
		scene()->selectedItems();

	if( !items.isEmpty() )
	{
		foreach( QGraphicsItem * item, items )
			if( item->parentItem() == this )
				d->ungroup( item );
	}
	else if( d->m_current )
		d->ungroup( d->m_current );
}

void
Form::deleteItems( const QList< QGraphicsItem* > & items )
{
	foreach( QGraphicsItem * item, items )
	{
		FormObject * obj = dynamic_cast< FormObject* > ( item );

		if( obj )
			d->m_model->removeObject( obj, this );

		scene()->removeItem( item );

		if( obj )
		{
			d->m_model->endRemoveObject();

			d->m_ids.removeOne( obj->objectId() );

			switch( obj->type() )
			{
				case FormObject::GroupType :
				{
					FormGroup * group = dynamic_cast< FormGroup* > ( item );

					if( group )
					{
						d->clearIds( group );

						group->destroyHandles();
					}
				}
					break;

				case FormObject::TextType :
				{
					FormText * text = dynamic_cast< FormText* > ( item );

					if( text )
						text->destroyHandles();
				}
					break;

				case FormObject::ImageType :
				{
					FormImage * image = dynamic_cast< FormImage* > ( item );

					if( image )
						image->destroyHandles();
				}
					break;

				default :
					break;
			}
		}

		delete item;
	}
}

QRectF
Form::boundingRect() const
{
	if( !d.isNull() )
		return QRectF( 0, 0, d->m_cfg.size().width(),
			d->m_cfg.size().height() );
	else
		return QRectF();
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
Form::renameObject( FormObject * obj )
{
	if( obj->form() == this )
	{
		if( obj != this )
		{
			NameDlg dlg( d->m_ids, tr( "Enter New Name of the Object..." ),
				scene()->views().first() );

			if( dlg.exec() == QDialog::Accepted )
			{
				const QString old = obj->objectId();

				obj->setObjectId( dlg.name() );

				d->m_ids.removeOne( old );

				d->m_ids.append( dlg.name() );
			}
		}
		else
			TopGui::instance()->projectWindow()->projectWidget()->renameTab(
				obj->objectId() );
	}
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
			case FormAction::DrawLine :
			{
				FormLine * line = dynamic_cast< FormLine* > ( d->m_current );

				if( line )
				{
					const QLineF l = line->line();

					line->setLine( l.p1().x(), l.p1().y(),
						l.p2().x() + delta.x(), l.p2().y() + delta.y() );

					d->handleMouseMoveInCurrentLines( mouseEvent->pos() );
				}

				if( d->m_currentPoly )
					d->handleMouseMoveInCurrentPolyLine( mouseEvent->pos() );

				mouseEvent->accept();

				return;
			}
				break;

			case FormAction::InsertText :
			{
				FormRectPlacer * rect = dynamic_cast< FormRectPlacer* >
					( d->m_current );

				if( rect )
					rect->setEndPos( mouseEvent->pos() );

				mouseEvent->accept();

				update();

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
			case FormAction::DrawLine :
			{
				d->m_pressed = true;

				FormLine * line = new FormLine( this, this );

				bool intersected = false;
				bool intersectedEnds = false;
				FormLine * intersectedLine = 0;

				QPointF p = d->lineStartPoint( mouseEvent->pos(),
					intersected, intersectedEnds, intersectedLine );

				if( !intersected && FormAction::instance()->isSnapEnabled() )
					p = d->m_snap->snapPos();

				line->setLine( p.x(), p.y(), p.x(), p.y() );

				if( intersectedEnds && d->m_currentLines.size() == 1 &&
					FormAction::instance()->testFlag( FormAction::Polyline ) )
						d->m_polyline = true;

				const QString id = d->id();

				line->setObjectId( id );

				d->m_ids.append( id );

				d->m_model->addObject( line, this );

				d->m_current = line;

				if( !intersected )
				{
					d->clearCurrentLines();

					d->m_polyline = false;

					if( d->m_currentPoly )
					{
						d->m_currentPoly->showHandles( false );

						d->m_currentPoly = 0;
					}
				}

				mouseEvent->accept();

				return;
			}
				break;

			case FormAction::InsertText :
			{
				d->m_pressed = true;

				FormRectPlacer * rect = new FormRectPlacer( this );

				QPointF p = mouseEvent->pos();

				if( FormAction::instance()->isSnapEnabled() )
					p = d->m_snap->snapPos();

				rect->setStartPos( p );

				d->m_current = rect;

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
			case FormAction::DrawLine :
			{
				FormLine * line = dynamic_cast< FormLine* > ( d->m_current );

				if( line )
				{
					bool intersected = false;
					bool intersectedEnds = false;
					FormLine * intersectedLine = 0;

					QPointF p = d->lineStartPoint( mouseEvent->pos(),
						intersected, intersectedEnds, intersectedLine );

					if( !intersected && FormAction::instance()->isSnapEnabled() )
						p = d->m_snap->snapPos();

					const QLineF l = line->line();

					line->setLine( l.p1().x(), l.p1().y(), p.x(), p.y() );

					if( d->m_polyline )
					{
						if( !d->m_currentLines.isEmpty() )
						{
							d->m_currentPoly = new FormPolyline( this, this );

							const QString id = d->id();

							d->m_currentPoly->setObjectId( id );

							d->m_ids.append( id );

							d->m_model->addObject( d->m_currentPoly, this );

							d->m_currentPoly->appendLine(
								d->m_currentLines.first()->line() );
							d->m_currentPoly->showHandles( true );

							d->m_model->removeObject( d->m_currentLines.first(),
								this );

							scene()->removeItem( d->m_currentLines.first() );

							d->m_model->endRemoveObject();

							delete d->m_currentLines.first();

							d->m_currentLines.clear();
						}

						d->m_currentPoly->appendLine( line->line() );

						d->m_model->removeObject( line, this );

						scene()->removeItem( line );

						d->m_model->endRemoveObject();

						delete line;

						d->m_current = 0;

						if( d->m_currentPoly->isClosed() )
						{
							d->m_currentPoly->showHandles( false );

							d->m_current = d->m_currentPoly;

							d->m_currentPoly = 0;

							d->m_polyline = false;
						}
					}
					else
					{
						d->m_currentLines.append( line );

						line->showHandles( true );
					}
				}

				d->m_pressed = false;

				mouseEvent->accept();

				emit changed();

				return;
			}
				break;

			case FormAction::InsertText :
			{
				scene()->removeItem( d->m_current );

				FormRectPlacer * rect = dynamic_cast< FormRectPlacer* >
					( d->m_current );

				FormText * text = 0;

				if( rect )
				{
					text = new FormText( rect->rect(), this, this );

					const QString id = d->id();

					text->setObjectId( id );

					d->m_ids.append( id );

					d->m_model->addObject( text, this );

					text->setFocus();

					QTextCursor c = text->textCursor();
					c.select( QTextCursor::Document );
					text->setTextCursor( c );
				}

				delete d->m_current;

				d->m_current = text;

				mouseEvent->accept();

				d->m_pressed = false;

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

void
Form::dragEnterEvent( QGraphicsSceneDragDropEvent * event )
{
	d->m_snap->setSnapPos( event->pos() );

	if( event->mimeData()->hasImage() )
		event->acceptProposedAction();
	else
		QGraphicsObject::dragEnterEvent( event );
}

void
Form::dragMoveEvent( QGraphicsSceneDragDropEvent * event )
{
	d->m_snap->setSnapPos( event->pos() );

	if( event->mimeData()->hasImage() )
		event->acceptProposedAction();
	else
		QGraphicsObject::dragMoveEvent( event );
}

void
Form::dropEvent( QGraphicsSceneDragDropEvent * event )
{
	if( event->mimeData()->hasImage() )
	{
		FormImage * image = new FormImage( this, this );

		const QString id = d->id();

		image->setObjectId( id );

		d->m_ids.append( id );

		d->m_model->addObject( image, this );

		if( FormAction::instance()->mode() == FormAction::Select )
		{
			image->setFlag( QGraphicsItem::ItemIsSelectable, true );

			image->setSelected( true );
		}

		d->m_snap->setSnapPos( event->pos() );

		if( FormAction::instance()->isSnapEnabled() )
			image->setPos( d->m_snap->snapPos() );
		else
			image->setPos( event->pos() );

		image->setImage( qvariant_cast< QImage >
			( event->mimeData()->imageData() ) );

		event->acceptProposedAction();

		emit changed();
	}
	else
		QGraphicsObject::dropEvent( event );
}

} /* namespace Core */

} /* namespace Prototyper */
