
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
#include "page.hpp"
#include "top_gui.hpp"
#include "project_window.hpp"
#include "project_widget.hpp"
#include "project_cfg.hpp"
#include "form_actions.hpp"
#include "form_rect_placer.hpp"
#include "top_gui.hpp"
#include "project_window.hpp"
#include "name_dlg.hpp"
#include "utils.hpp"
#include "form_undo_commands.hpp"
#include "constants.hpp"
#include "page_private.hpp"
#include "form_line.hpp"
#include "form_polyline.hpp"
#include "form_group.hpp"
#include "form_text.hpp"
#include "form_image.hpp"
#include "form_rectangle.hpp"
#include "form_button.hpp"
#include "form_checkbox.hpp"
#include "form_radio_button.hpp"
#include "form_combobox.hpp"
#include "form_spinbox.hpp"
#include "form_hslider.hpp"
#include "form_vslider.hpp"
#include "form_grid_snap.hpp"

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
#include <QSharedPointer>
#include <QUndoStack>
#include <QUndoGroup>
#include <QMap>

// C++ include.
#include <algorithm>
#include <type_traits>


namespace Prototyper {

namespace Core {

//
// FormPrivate
//

void
PagePrivate::init()
{
	m_gridStepAction = new QAction( QIcon( QStringLiteral( ":/Core/img/measure.png" ) ),
		ProjectWindow::tr( "Grid Step" ), q );

	Page::connect( m_gridStepAction, &QAction::triggered,
		q, &Page::slotSetGridStep );

	m_snap = new GridSnap( q );
	m_snap->setGridStep( m_cfg.gridStep() );

	q->setAcceptHoverEvents( true );

	q->setAcceptDrops( true );

	m_undoStack = new QUndoStack(
		TopGui::instance()->projectWindow()->projectWidget()->undoGroup() );
}

qreal
PagePrivate::currentZValue() const
{
	qreal z = 0.0;

	currentZValue( q->childItems(), z );

	return z;
}

void
PagePrivate::currentZValue( const QList< QGraphicsItem* > & items,
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
PagePrivate::lineStartPoint( const QPointF & point, bool & intersected,
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
PagePrivate::clearCurrentLines()
{
	foreach( FormLine * line, m_currentLines )
		line->showHandles( false );

	m_currentLines.clear();
}

void
PagePrivate::handleMouseMoveInCurrentLines( const QPointF & point )
{
	foreach( FormLine * line, m_currentLines )
		line->handleMouseMoveInHandles( point );
}

void
PagePrivate::handleMouseMoveInCurrentPolyLine( const QPointF & point )
{
	m_currentPoly->handleMouseMoveInHandles( point );
}

void
PagePrivate::ungroup( QGraphicsItem * group, bool pushUndoCommand )
{
	FormGroup * tmp = dynamic_cast< FormGroup* > ( group );

	if( tmp )
	{
		m_current = Q_NULLPTR;

		QList< QGraphicsItem* > items = tmp->childItems();

		QStringList ids;

		foreach( QGraphicsItem * i, tmp->children() )
			ids.append( dynamic_cast< FormObject* > ( i )->objectId() );

		if( pushUndoCommand )
			m_undoStack->push( new UndoUngroup( ids,
				tmp->objectId(), q ) );

		foreach( QGraphicsItem * item, items )
		{
			tmp->removeFromGroup( item );

			if( PageAction::instance()->mode() == PageAction::Select )
			{
				item->setFlag( QGraphicsItem::ItemIsSelectable, true );

				item->setSelected( true );

				FormText * text = dynamic_cast< FormText* > ( item );

				if( text )
					text->enableEditing( false );
			}
		}

		tmp->postDeletion();

		delete tmp;

		q->scene()->setSceneRect( q->scene()->itemsBoundingRect() );

		emit q->changed();
	}
}

QString
PagePrivate::id()
{
	while( m_ids.contains( QString::number( ++m_id ) ) )
	{
	}

	return QString::number( m_id );
}

void
PagePrivate::updateFromCfg()
{
	clear();

	QApplication::processEvents();

	Cfg::Size size;
	size.set_width( MmPx::instance().fromMmX( m_cfg.size().width() ) );
	size.set_height( MmPx::instance().fromMmY( m_cfg.size().height() ) );

	q->setSize( size );

	q->setGridStep( m_cfg.gridStep() );

	q->setObjectId( m_cfg.tabName() );

	m_ids.append( m_cfg.tabName() );

	foreach( const Cfg::Line & c, m_cfg.line() )
		createElem< FormLine > ( c );

	foreach( const Cfg::Polyline & c, m_cfg.polyline() )
		createElem< FormPolyline > ( c );

	foreach( const Cfg::Text & c, m_cfg.text() )
		createText( c );

	foreach( const Cfg::Image & c, m_cfg.image() )
		createElem< FormImage > ( c );

	foreach( const Cfg::Rect & c, m_cfg.rect() )
		createElem< FormRect > ( c );

	foreach( const Cfg::Group & c, m_cfg.group() )
		createGroup( c );

	foreach( const Cfg::Button & c, m_cfg.button() )
		createElemWithRect< FormButton > ( c, QRectF() );

	foreach( const Cfg::ComboBox & c, m_cfg.combobox() )
		createElemWithRect< FormComboBox > ( c, QRectF() );

	foreach( const Cfg::CheckBox & c, m_cfg.radiobutton() )
		createElemWithRect< FormRadioButton > ( c, QRectF() );

	foreach( const Cfg::CheckBox & c, m_cfg.checkbox() )
		createElemWithRect< FormCheckBox > ( c, QRectF() );

	foreach( const Cfg::HSlider & c, m_cfg.hslider() )
		createElemWithRect< FormHSlider > ( c, QRectF() );

	foreach( const Cfg::VSlider & c, m_cfg.vslider() )
		createElemWithRect< FormVSlider > ( c, QRectF() );

	foreach( const Cfg::SpinBox & c, m_cfg.spinbox() )
		createElemWithRect< FormSpinBox > ( c, QRectF() );

	q->update();
}

void
PagePrivate::clear()
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
		QGraphicsItem * item = dynamic_cast< QGraphicsItem* > ( obj );

		if( item )
			q->scene()->removeItem( item );
	}

	foreach( QGraphicsItem * item, items )
		delete item;
}

template< class Elem, class Config >
inline
Elem *
PagePrivate::createElem( const Config & cfg )
{
	Elem * e = new Elem( q, q );

	e->setCfg( cfg );

	m_ids.append( e->objectId() );

	return e;
}

template< class Elem, class Config >
inline
Elem *
PagePrivate::createElemWithRect( const Config & cfg, const QRectF & rect )
{
	Elem * e = new Elem( rect, q, q );

	e->setCfg( cfg );

	m_ids.append( e->objectId() );

	return e;
}

FormText *
PagePrivate::createText( const Cfg::Text & cfg )
{
	const QRectF r( 0.0, 0.0, cfg.textWidth(), 0.0 );

	FormText * text = new FormText( r, q, q );

	text->setCfg( cfg );

	m_ids.append( text->objectId() );

	m_docs.insert( text->document(), text );

	Page::connect( text->document(), &QTextDocument::undoCommandAdded,
		q, &Page::undoCommandInTextAdded );

	return text;
}

FormGroup *
PagePrivate::createGroup( const Cfg::Group & cfg )
{
	FormGroup * group = new FormGroup( q, q );

	group->setCfg( cfg );

	addIds( group );

	return group;
}

void
PagePrivate::clearIds( FormGroup * group )
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
PagePrivate::addIds( FormGroup * group )
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

void
PagePrivate::setText( const QSharedPointer< QTextDocument > & doc,
	const std::vector< Cfg::TextStyle > & text )
{
	doc->clear();

	Cfg::fillTextDocument( doc.data(), text );
}

void
PagePrivate::hideHandlesOfCurrent()
{
	if( m_current )
	{
		FormObject * obj = dynamic_cast< FormObject* > ( m_current );

		if( obj )
		{
			switch( obj->objectType() )
			{
				case FormObject::LineType :
				{
					FormLine * line = dynamic_cast< FormLine* > ( m_current );

					if( line )
						line->showHandles( false );
				}
					break;

				case FormObject::PolylineType :
				{
					FormPolyline * line = dynamic_cast< FormPolyline* > ( m_current );

					if( line )
						line->showHandles( false );
				}
					break;

				default :
					break;
			}
		}
	}
}

QList< QGraphicsItem* >
PagePrivate::selection()
{
	QList< QGraphicsItem* > items =
		q->scene()->selectedItems();

	QList< QGraphicsItem* > res;

	foreach( QGraphicsItem * i, items )
	{
		if( dynamic_cast< FormObject* > ( i ) )
			res.append( i );
	}

	return res;
}

qreal
PagePrivate::searchAlignPoint( const QList< QGraphicsItem* > & items,
	AlignPoint point )
{
	qreal pos = 0.0;

	if( !items.isEmpty() )
	{
		switch( point )
		{
			case AlignHorLeftPoint :
			case AlignHorCenterPoint :
			case AlignHorRightPoint :
			{
				pos = dynamic_cast< FormObject* > ( items.first() )->
					position().x();
			}
				break;

			case AlignVertTopPoint :
			case AlignVertCenterPoint :
			case AlignVertBottomPoint :
			{
				pos = dynamic_cast< FormObject* > ( items.first() )->
					position().y();
			}
				break;

			default :
				break;
		}
	}

	int count = 0;
	QList< qreal > points;

	foreach( QGraphicsItem * item, items )
	{
		switch( point )
		{
			case AlignHorLeftPoint :
			{
				if( dynamic_cast< FormObject* > ( item )->position().x() < pos )
					pos = dynamic_cast< FormObject* > ( item )->position().x();
			}
				break;

			case AlignHorCenterPoint :
			{
				++count;

				points.append( dynamic_cast< FormObject* > ( item )->position().x() +
					item->boundingRect().width() / 2.0 );
			}
				break;

			case AlignHorRightPoint :
			{
				if( dynamic_cast< FormObject* > ( item )->position().x() +
					item->boundingRect().width() > pos )
						pos = dynamic_cast< FormObject* > ( item )->position().x() +
							item->boundingRect().width();
			}
				break;

			case AlignVertTopPoint :
			{
				if( dynamic_cast< FormObject* > ( item )->position().y() > pos )
					pos = dynamic_cast< FormObject* > ( item )->position().y();
			}
				break;

			case AlignVertCenterPoint :
			{
				++count;

				points.append( dynamic_cast< FormObject* > ( item )->position().y() +
					item->boundingRect().height() / 2.0 );
			}
				break;

			case AlignVertBottomPoint :
			{
				if( dynamic_cast< FormObject* > ( item )->position().y() +
					item->boundingRect().height() < pos )
						pos = dynamic_cast< FormObject* > ( item )->position().y() +
							item->boundingRect().height();
			}
				break;

			default :
				break;
		}
	}

	switch( point )
	{
		case AlignVertCenterPoint :
		case AlignHorCenterPoint :
		{
			const qreal total = std::accumulate( points.constBegin(),
				points.constEnd(), 0.0 );

			return total / count;
		}
			break;

		default :
			return pos;
	}
}


//
// Page
//

Page::Page( Cfg::Page & c, QGraphicsItem * parent )
	:	QGraphicsObject( parent )
	,	FormObject( FormObject::PageType, this )
	,	d( 0 )
{
	QScopedPointer< PagePrivate > tmp( new PagePrivate( c, this ) );

	tmp->init();

	d.swap( tmp );
}

Page::~Page()
{
}

template< class Elem >
FormObject *
Page::createElement( const QString & id )
{
	FormObject * obj = Q_NULLPTR;

	switch( Elem::staticObjectType() )
	{
		case FormObject::LineType :
		{
			obj = new FormLine( this, this );
		}
			break;

		case FormObject::PolylineType :
		{
			obj = new FormPolyline( this, this );
		}
			break;

		case FormObject::TextType :
		{
			obj = new FormText( QRectF(), this, this );
		}
			break;

		case FormObject::ImageType :
		{
			obj = new FormImage( this, this );
		}
			break;

		case FormObject::RectType :
		{
			obj = new FormRect( this, this );
		}
			break;

		case FormObject::GroupType :
		{
			obj = new FormGroup( this, this );
		}
			break;

		case FormObject::ButtonType :
		{
			obj = new FormButton( QRectF(), this, this );
		}
			break;

		case FormObject::ComboBoxType :
		{
			obj = new FormComboBox( QRectF(), this, this );
		}
			break;

		case FormObject::RadioButtonType :
		{
			obj = new FormRadioButton( QRectF(), this, this );
		}
			break;

		case FormObject::CheckBoxType :
		{
			obj = new FormCheckBox( QRectF(), this, this );
		}
			break;

		case FormObject::HSliderType :
		{
			obj = new FormHSlider( QRectF(), this, this );
		}
			break;

		case FormObject::VSliderType :
		{
			obj = new FormVSlider( QRectF(), this, this );
		}
			break;

		case FormObject::SpinBoxType :
		{
			obj = new FormSpinBox( QRectF(), this, this );
		}
			break;

		default :
			break;
	}

	obj->setObjectId( id );

	d->m_ids.append( id );

	return obj;
}

QUndoStack *
Page::undoStack() const
{
	return d->m_undoStack;
}

const Cfg::Size &
Page::size() const
{
	return d->m_cfg.size();
}

void
Page::setSize( const Cfg::Size & s )
{
	d->m_cfg.set_size( s );

	update();
}

GridMode
Page::gridMode() const
{
	return d->m_gridMode;
}

void
Page::setGridMode( GridMode m )
{
	d->m_gridMode = m;

	update();
}

int
Page::gridStep() const
{
	return d->m_cfg.gridStep();
}

void
Page::setGridStep( int s )
{
	d->m_cfg.set_gridStep( s );

	d->m_snap->setGridStep( s );

	update();
}

Cfg::Page
Page::cfg() const
{
	Cfg::Page c = d->m_cfg;

	Cfg::Size size;
	size.set_width( MmPx::instance().toMmX( d->m_cfg.size().width() ) );
	size.set_height( MmPx::instance().toMmY( d->m_cfg.size().height() ) );
	c.set_size( size );

	c.line().clear();
	c.polyline().clear();
	c.text().clear();
	c.image().clear();
	c.group().clear();
	c.rect().clear();
	c.button().clear();
	c.checkbox().clear();
	c.combobox().clear();
	c.radiobutton().clear();
	c.hslider().clear();
	c.vslider().clear();
	c.spinbox().clear();

	foreach( QGraphicsItem * item, childItems() )
	{
		FormObject * obj = dynamic_cast< FormObject* > ( item );

		if( obj )
		{
			switch( obj->objectType() )
			{
				case FormObject::LineType :
				{
					FormLine * line = dynamic_cast< FormLine* > ( item );

					if( line )
						c.line().push_back( line->cfg() );
				}
					break;

				case FormObject::PolylineType :
				{
					FormPolyline * poly = dynamic_cast< FormPolyline* > ( item );

					if( poly )
						c.polyline().push_back( poly->cfg() );
				}
					break;

				case FormObject::TextType :
				{
					FormText * text = dynamic_cast< FormText* > ( item );

					if( text )
						c.text().push_back( text->cfg() );
				}
					break;

				case FormObject::ImageType :
				{
					FormImage * image = dynamic_cast< FormImage* > ( item );

					if( image )
						c.image().push_back( image->cfg() );
				}
					break;

				case FormObject::RectType :
				{
					FormRect * rect = dynamic_cast< FormRect* > ( item );

					if( rect )
						c.rect().push_back( rect->cfg() );
				}
					break;

				case FormObject::GroupType :
				{
					FormGroup * group = dynamic_cast< FormGroup* > ( item );

					if( group )
						c.group().push_back( group->cfg() );
				}
					break;

				case FormObject::ButtonType :
				{
					FormButton * btn = dynamic_cast< FormButton* > ( item );

					if( btn )
						c.button().push_back( btn->cfg() );
				}
					break;

				case FormObject::CheckBoxType :
				{
					FormCheckBox * chk = dynamic_cast< FormCheckBox* > ( item );

					if( chk )
						c.checkbox().push_back( chk->cfg() );
				}
					break;

				case FormObject::RadioButtonType :
				{
					FormRadioButton * r = dynamic_cast< FormRadioButton* > ( item );

					if( r )
						c.radiobutton().push_back( r->cfg() );
				}
					break;

				case FormObject::ComboBoxType :
				{
					FormComboBox * cb = dynamic_cast< FormComboBox* > ( item );

					if( cb )
						c.combobox().push_back( cb->cfg() );
				}
					break;

				case FormObject::SpinBoxType :
				{
					FormSpinBox * sb = dynamic_cast< FormSpinBox* > ( item );

					if( sb )
						c.spinbox().push_back( sb->cfg() );
				}
					break;

				case FormObject::HSliderType :
				{
					FormHSlider * hs = dynamic_cast< FormHSlider* > ( item );

					if( hs )
						c.hslider().push_back( hs->cfg() );
				}
					break;

				case FormObject::VSliderType :
				{
					FormVSlider * vs = dynamic_cast< FormVSlider* > ( item );

					if( vs )
						c.vslider().push_back( vs->cfg() );
				}
					break;

				default :
					break;
			}
		}
	}

	c.set_tabName( objectId() );

	return c;
}

void
Page::setCfg( const Cfg::Page & c )
{
	d->m_cfg = c;

	d->updateFromCfg();
}

void
Page::switchToSelectMode()
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
Page::switchToLineDrawingMode()
{
	d->hideHandlesOfCurrent();

	d->m_currentLines.clear();
	d->m_current = 0;
	d->m_currentPoly = 0;
	d->m_polyline = false;
}

void
Page::enableSnap( bool on )
{
	if( on )
		d->m_snap->show();
	else
		d->m_snap->hide();
}

GridSnap *
Page::snapItem() const
{
	return d->m_snap;
}

const QStringList &
Page::ids() const
{
	return d->m_ids;
}

QGraphicsItem *
Page::findItem( const QString & id )
{
	if( id == objectId() )
		return this;

	QList< QGraphicsItem* > items = childItems();

	foreach( QGraphicsItem * item, items )
	{
		FormObject * obj = dynamic_cast< FormObject* > ( item );

		if( obj && obj->objectId() == id )
			return item;
	}

	return Q_NULLPTR;
}

void
Page::group()
{
	QList< QGraphicsItem* > items =
		scene()->selectedItems();

	group( items );
}

void
Page::ungroup()
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

FormGroup *
Page::group( const QList< QGraphicsItem* > & items,
	bool pushUndoCommand, const QString & id )
{
	FormGroup * group = Q_NULLPTR;

	QString i = id;

	if( i.isEmpty() )
		i = d->id();

	if( items.size() > 1 )
	{
		group = new FormGroup( this, this );

		group->setObjectId( i );

		d->m_ids.append( i );

		foreach( QGraphicsItem * item, items )
		{
			item->setFlag( QGraphicsItem::ItemIsSelectable, false );

			item->setSelected( false );

			if( item->parentItem() == this )
				group->addToGroup( item );
		}
	}
	else if( d->m_currentLines.size() > 1 )
	{
		group = new FormGroup( this, this );

		group->setObjectId( i );

		d->m_ids.append( i );

		foreach( FormLine * line, d->m_currentLines )
		{
			line->setFlag( QGraphicsItem::ItemIsSelectable, false );

			line->setSelected( false );

			group->addToGroup( line );
		}

		d->m_currentLines.clear();
	}

	if( group )
	{
		d->m_current = group;

		if( pushUndoCommand )
			d->m_undoStack->push( new UndoGroup( this, group->objectId() ) );

		if( PageAction::instance()->mode() == PageAction::Select )
		{
			group->setFlag( QGraphicsItem::ItemIsSelectable );

			// Workaround for stay group selected.
			// Without this processEvents() after selecting group
			// its selected and next unknown to me event clears selection.
			QApplication::processEvents();

			group->setSelected( true );
		}

		emit changed();
	}

	return group;
}

void
Page::ungroup( FormGroup * g, bool pushUndoCommand )
{
	d->ungroup( g, pushUndoCommand );
}

void
Page::alignVerticalTop()
{
	QList< QGraphicsItem* > items = d->selection();

	if( items.size() > 1 )
	{
		const qreal y = d->searchAlignPoint( items,
			PagePrivate::AlignVertTopPoint );

		foreach( QGraphicsItem * item, items )
		{
			dynamic_cast< FormObject* > ( item )->setPosition(
				QPointF( dynamic_cast< FormObject* > ( item )->
					position().x(), y ) );
		}

		emit changed();
	}
}

void
Page::alignVerticalCenter()
{
	QList< QGraphicsItem* > items = d->selection();

	if( items.size() > 1 )
	{
		const qreal y = d->searchAlignPoint( items,
			PagePrivate::AlignVertCenterPoint );

		foreach( QGraphicsItem * item, items )
		{
			const qreal iy = dynamic_cast< FormObject* > ( item )->
				position().y();

			const qreal cy = iy + item->boundingRect().height() / 2.0;

			dynamic_cast< FormObject* > ( item )->setPosition(
				QPointF( dynamic_cast< FormObject* > ( item )->
						position().x(),
					 iy + y - cy ) );
		}

		emit changed();
	}
}

void
Page::alignVerticalBottom()
{
	QList< QGraphicsItem* > items = d->selection();

	if( items.size() > 1 )
	{
		const qreal y = d->searchAlignPoint( items,
			PagePrivate::AlignVertBottomPoint );

		foreach( QGraphicsItem * item, items )
		{
			dynamic_cast< FormObject* > ( item )->setPosition(
				QPointF( dynamic_cast< FormObject* > ( item )->
					position().x(), dynamic_cast< FormObject* > ( item )->
						position().y() + y -
					( dynamic_cast< FormObject* > ( item )->
						position().y() + item->boundingRect().height() ) ) );
		}

		emit changed();
	}
}

void
Page::alignHorizontalLeft()
{
	QList< QGraphicsItem* > items = d->selection();

	if( items.size() > 1 )
	{
		const qreal x = d->searchAlignPoint( items,
			PagePrivate::AlignHorLeftPoint );

		foreach( QGraphicsItem * item, items )
		{
			dynamic_cast< FormObject* > ( item )->setPosition(
				QPointF( x, dynamic_cast< FormObject* > ( item )->position().y() ) );
		}

		emit changed();
	}
}

void
Page::alignHorizontalCenter()
{
	QList< QGraphicsItem* > items = d->selection();

	if( items.size() > 1 )
	{
		const qreal x = d->searchAlignPoint( items,
			PagePrivate::AlignHorCenterPoint );

		foreach( QGraphicsItem * item, items )
		{
			const qreal ix = dynamic_cast< FormObject* > ( item )->
				position().x();

			const qreal cx = ix +
				item->boundingRect().width() / 2.0;

			dynamic_cast< FormObject* > ( item )->setPosition(
				QPointF( ix + x - cx,
					dynamic_cast< FormObject* > ( item )->position().y() ) );
		}

		emit changed();
	}
}

void
Page::alignHorizontalRight()
{
	QList< QGraphicsItem* > items = d->selection();

	if( items.size() > 1 )
	{
		const qreal x = d->searchAlignPoint( items,
			PagePrivate::AlignHorRightPoint );

		foreach( QGraphicsItem * item, items )
		{
			dynamic_cast< FormObject* > ( item )->setPosition(
				QPointF( dynamic_cast< FormObject* > ( item )->position().x() + x -
						( dynamic_cast< FormObject* > ( item )->position().x() + item->boundingRect().width() ),
					dynamic_cast< FormObject* > ( item )->position().y() ) );
		}

		emit changed();
	}
}

static inline void pushUndoDeleteCommand( QUndoStack * stack,
	FormObject * obj, Page * form )
{
	switch( obj->objectType() )
	{
		case FormObject::LineType :
		{
			FormLine * item = dynamic_cast< FormLine* > ( obj );
			Cfg::Line cfg = item->cfg();

			stack->push( new UndoDelete< std::remove_pointer_t<
				decltype( item ) >, decltype( cfg ) >( form, cfg ) );
		}
			break;

		case FormObject::PolylineType :
		{
			FormPolyline * item = dynamic_cast< FormPolyline* > ( obj );
			Cfg::Polyline cfg = item->cfg();

			stack->push( new UndoDelete< std::remove_pointer_t<
				decltype( item ) >, decltype( cfg ) >( form, cfg ) );
		}
			break;

		case FormObject::TextType :
		{
			FormText * item = dynamic_cast< FormText* > ( obj );
			Cfg::Text cfg = item->cfg();

			stack->push( new UndoDelete< std::remove_pointer_t<
				decltype( item ) >, decltype( cfg ) >( form, cfg ) );
		}
			break;

		case FormObject::ImageType :
		{
			FormImage * item = dynamic_cast< FormImage* > ( obj );
			Cfg::Image cfg = item->cfg();

			stack->push( new UndoDelete< std::remove_pointer_t<
				decltype( item ) >, decltype( cfg ) >( form, cfg ) );
		}
			break;

		case FormObject::RectType :
		{
			FormRect * item = dynamic_cast< FormRect* > ( obj );
			Cfg::Rect cfg = item->cfg();

			stack->push( new UndoDelete< std::remove_pointer_t<
				decltype( item ) >, decltype( cfg ) >( form, cfg ) );
		}
			break;

		case FormObject::GroupType :
		{
			FormGroup * item = dynamic_cast< FormGroup* > ( obj );
			Cfg::Group cfg = item->cfg();

			stack->push( new UndoDelete< std::remove_pointer_t<
				decltype( item ) >, decltype( cfg ) >( form, cfg ) );
		}
			break;

		case FormObject::ButtonType :
		{
			FormButton * item = dynamic_cast< FormButton* > ( obj );
			Cfg::Button cfg = item->cfg();

			stack->push( new UndoDelete< std::remove_pointer_t<
				decltype( item ) >, decltype( cfg ) >( form, cfg ) );
		}
			break;

		case FormObject::ComboBoxType :
		{
			FormComboBox * item = dynamic_cast< FormComboBox* > ( obj );
			Cfg::ComboBox cfg = item->cfg();

			stack->push( new UndoDelete< std::remove_pointer_t<
				decltype( item ) >, decltype( cfg ) >( form, cfg ) );
		}
			break;

		case FormObject::RadioButtonType :
		{
			FormRadioButton * item = dynamic_cast< FormRadioButton* > ( obj );
			Cfg::CheckBox cfg = item->cfg();

			stack->push( new UndoDelete< std::remove_pointer_t<
				decltype( item ) >, decltype( cfg ) >( form, cfg ) );
		}
			break;

		case FormObject::CheckBoxType :
		{
			FormCheckBox * item = dynamic_cast< FormCheckBox* > ( obj );
			Cfg::CheckBox cfg = item->cfg();

			stack->push( new UndoDelete< std::remove_pointer_t<
				decltype( item ) >, decltype( cfg ) >( form, cfg ) );
		}
			break;

		case FormObject::HSliderType :
		{
			FormHSlider * item = dynamic_cast< FormHSlider* > ( obj );
			Cfg::HSlider cfg = item->cfg();

			stack->push( new UndoDelete< std::remove_pointer_t<
				decltype( item ) >, decltype( cfg ) >( form, cfg ) );
		}
			break;

		case FormObject::VSliderType :
		{
			FormVSlider * item = dynamic_cast< FormVSlider* > ( obj );
			Cfg::VSlider cfg = item->cfg();

			stack->push( new UndoDelete< std::remove_pointer_t<
				decltype( item ) >, decltype( cfg ) >( form, cfg ) );
		}
			break;

		case FormObject::SpinBoxType :
		{
			FormSpinBox * item = dynamic_cast< FormSpinBox* > ( obj );
			Cfg::SpinBox cfg = item->cfg();

			stack->push( new UndoDelete< std::remove_pointer_t<
				decltype( item ) >, decltype( cfg ) >( form, cfg ) );
		}
			break;

		default :
			break;
	}
}

void
Page::deleteItems( const QList< QGraphicsItem* > & items,
	bool makeUndoCommand )
{
	foreach( QGraphicsItem * item, items )
	{
		if( item == d->m_current )
			d->m_current = 0;

		FormObject * obj = dynamic_cast< FormObject* > ( item );

		if( obj )
		{
			if( makeUndoCommand )
				pushUndoDeleteCommand( d->m_undoStack, obj, this );

			d->m_ids.removeOne( obj->objectId() );

			switch( obj->objectType() )
			{
				case FormObject::GroupType :
				{
					FormGroup * group = dynamic_cast< FormGroup* > ( item );

					if( group )
					{
						d->clearIds( group );

						group->postDeletion();
					}
				}
					break;

				case FormObject::TextType :
				{
					FormText * text = dynamic_cast< FormText* > ( item );

					if( text )
					{
						removeDocFromMap( text->document() );

						text->postDeletion();
					}
				}
					break;

				default :
				{
					obj->postDeletion();
				}
					break;
			}
		}

		delete item;
	}

	scene()->update();
}

QRectF
Page::boundingRect() const
{
	if( !d.isNull() )
		return QRectF( 0.0, 0.0, d->m_cfg.size().width(),
			d->m_cfg.size().height() + 30.0 );
	else
		return QRectF();
}

void
Page::paint( QPainter * painter, const QStyleOptionGraphicsItem * option,
	QWidget * widget )
{
	Q_UNUSED( option )
	Q_UNUSED( widget )

	draw( painter, d->m_cfg.size().width(),
		d->m_cfg.size().height(), d->m_cfg.gridStep(),
		d->m_gridMode == ShowGrid );
}

void
Page::draw( QPainter * painter, int width, int height, int gridStep, bool drawGrid )
{
	static const QColor gridColor = Qt::gray;

	painter->setPen( gridColor );

	painter->setRenderHint( QPainter::Antialiasing, false );

	painter->setBrush( QBrush( gridColor ) );

	painter->setBrush( Qt::white );


	painter->setBrush( Qt::white );

	QRect r( 0, 0, width, height );

	painter->drawRect( r );

	if( drawGrid )
	{
		for( int x = gridStep; x < width; x += gridStep )
			painter->drawLine( x, 0, x, height );

		for( int y = gridStep; y < height; y += gridStep )
			painter->drawLine( 0, y, width, y );
	}
}

void
Page::setPosition( const QPointF & pos, bool pushUndoCommand )
{
	Q_UNUSED( pos )
	Q_UNUSED( pushUndoCommand )
}

QPointF
Page::position() const
{
	return pos();
}

QRectF
Page::rectangle() const
{
	return boundingRect();
}

void
Page::setRectangle( const QRectF & rect, bool pushUndoCommand )
{
	if( pushUndoCommand )
		d->m_undoStack->push( new UndoResize( this, objectId(),
			QRectF( 0.0, 0.0, d->m_cfg.size().width(), d->m_cfg.size().height() ),
			rect ) );

	d->m_cfg.size().width() = rect.width();
	d->m_cfg.size().height() = rect.height();

	emit changed();

	scene()->setSceneRect( scene()->itemsBoundingRect() );

	update();

	scene()->update();
}

void
Page::emitChanged()
{
	emit changed();
}

void
Page::renameForm( const QString & name )
{
	const QString old = objectId();

	setObjectId( name );

	d->m_ids.removeOne( old );

	d->m_ids.append( name );

	d->m_cfg.set_tabName( name );
}

void
Page::clearEditModeInTexts()
{
	const auto items = childItems();

	for( const auto & i : items )
	{
		auto * t = dynamic_cast< FormText* > ( i );

		if( t )
			t->clearEditMode();
	}
}

void
Page::slotSetGridStep()
{
	TopGui::instance()->projectWindow()->setGridStep(
		d->m_cfg.gridStep(), false );
}

void
Page::undoCommandInTextAdded()
{
	FormObject * obj = d->m_docs[ sender() ];

	if( obj )
		d->m_undoStack->push( new UndoChangeTextOnForm(
			this, obj->objectId() ) );
}

void
Page::contextMenuEvent( QGraphicsSceneContextMenuEvent * event )
{
	FormRectPlacer * placer = dynamic_cast< FormRectPlacer* > ( d->m_current );

	if( placer )
		delete placer;

	QMenu menu;
	menu.addAction( TopGui::instance()->projectWindow()->showHideGridAction() );
	menu.addAction( d->m_gridStepAction );
	menu.addAction( QIcon( QStringLiteral( ":/Core/img/transform-scale.png" ) ),
		tr( "Resize" ), this, SLOT( resizeForm() ) );

	menu.exec( event->screenPos() );

	event->accept();
}

void
Page::mouseMoveEvent( QGraphicsSceneMouseEvent * mouseEvent )
{
	if( d->m_pressed )
	{
		const QPointF delta = mouseEvent->pos() - d->m_pos;

		d->m_pos = mouseEvent->pos();

		d->m_snap->setSnapPos( d->m_pos );

		switch( PageAction::instance()->mode() )
		{
			case PageAction::DrawLine :
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

			case PageAction::InsertText :
			case PageAction::DrawButton :
			case PageAction::DrawCheckBox :
			case PageAction::DrawRadioButton :
			case PageAction::DrawComboBox :
			case PageAction::DrawSpinBox :
			case PageAction::DrawHSlider :
			case PageAction::DrawVSlider :
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

			case PageAction::DrawRect :
			{
				FormRect * rect = dynamic_cast< FormRect* > ( d->m_current );

				if( rect )
				{
					QRectF r = rect->rectangle();
					r.setBottomRight( mouseEvent->pos() );

					rect->setRectangle( r );
				}

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
Page::mousePressEvent( QGraphicsSceneMouseEvent * mouseEvent )
{
	if( mouseEvent->button() == Qt::LeftButton )
	{
		d->m_pos = mouseEvent->pos();

		d->m_snap->setSnapPos( d->m_pos );

		switch( PageAction::instance()->mode() )
		{
			case PageAction::DrawLine :
			{
				if( d->m_current != d->m_currentPoly )
					d->hideHandlesOfCurrent();

				d->m_pressed = true;

				FormLine * line = new FormLine( this, this );

				bool intersected = false;
				bool intersectedEnds = false;
				FormLine * intersectedLine = 0;

				QPointF p = d->lineStartPoint( mouseEvent->pos(),
					intersected, intersectedEnds, intersectedLine );

				if( !intersected && PageAction::instance()->isSnapEnabled() )
					p = d->m_snap->snapPos();

				line->setLine( p.x(), p.y(), p.x(), p.y() );

				if( intersectedEnds && d->m_currentLines.size() == 1 &&
					PageAction::instance()->testFlag( PageAction::Polyline ) )
						d->m_polyline = true;

				const QString id = d->id();

				line->setObjectId( id );

				d->m_ids.append( id );

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

			case PageAction::InsertText :
			case PageAction::DrawButton :
			case PageAction::DrawCheckBox :
			case PageAction::DrawRadioButton :
			case PageAction::DrawComboBox :
			case PageAction::DrawSpinBox :
			case PageAction::DrawHSlider :
			case PageAction::DrawVSlider :
			{
				d->hideHandlesOfCurrent();

				d->m_pressed = true;

				FormRectPlacer * rect = new FormRectPlacer( this );

				QPointF p = mouseEvent->pos();

				if( PageAction::instance()->isSnapEnabled() )
					p = d->m_snap->snapPos();

				rect->setStartPos( p );

				d->m_current = rect;

				mouseEvent->accept();

				return;
			}
				break;

			case PageAction::DrawRect :
			{
				d->hideHandlesOfCurrent();

				d->m_pressed = true;

				FormRect * rect = new FormRect( this, this );

				d->m_current = rect;

				const QString id = d->id();

				rect->setObjectId( id );

				d->m_ids.append( id );

				QPointF p = mouseEvent->pos();

				if( PageAction::instance()->isSnapEnabled() )
					p = d->m_snap->snapPos();

				rect->setRectangle( QRectF( p, QSizeF( 0.0, 0.0 ) ) );

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

template< class Elem, class Config >
Elem * onReleaseWithRectPlacer( QGraphicsScene * scene, PagePrivate * d,
	QGraphicsSceneMouseEvent * mouseEvent, Page * form )
{
	scene->removeItem( d->m_current );

	FormRectPlacer * rect = dynamic_cast< FormRectPlacer* >
		( d->m_current );

	Elem * elem = 0;

	if( rect )
	{
		QPointF p = mouseEvent->pos();

		if( PageAction::instance()->isSnapEnabled() )
			p = d->m_snap->snapPos();

		QRectF r = rect->rect();
		r = QRectF( r.x(), r.y(), p.x() - r.x(), p.y() - r.y() );
		r = r.normalized();

		elem = new Elem( r, form, form );

		if( elem->defaultSize().width() > 0.0 )
		{
			const auto s = elem->defaultSize();
			const auto r = elem->rectangle();

			auto w = r.width();
			auto h = r.height();

			bool resize = false;

			if( r.width() < s.width() )
			{
				w = s.width();
				resize = true;
			}

			if( r.height() < s.height() )
			{
				h = s.height();
				resize = true;
			}

			if( resize )
				elem->setRectangle( QRectF( r.topLeft().x(), r.topLeft().y(), w, h ), false );
		}

		const QString id = d->id();

		elem->setObjectId( id );

		d->m_ids.append( id );

		d->m_undoStack->push( new UndoCreate< Elem, Config > ( form,
			elem->objectId() ) );
	}

	delete d->m_current;

	d->m_current = elem;

	mouseEvent->accept();

	d->m_pressed = false;

	return elem;
}

void
Page::mouseReleaseEvent( QGraphicsSceneMouseEvent * mouseEvent )
{
	if( mouseEvent->button() == Qt::LeftButton && d->m_pressed )
	{
		d->m_pressed = false;

		d->m_snap->setSnapPos( mouseEvent->pos() );

		switch( PageAction::instance()->mode() )
		{
			case PageAction::DrawLine :
			{
				FormLine * line = dynamic_cast< FormLine* > ( d->m_current );

				if( line )
				{
					if( !d->m_polyline )
						d->m_undoStack->push(
							new UndoCreate< FormLine, Cfg::Line > (
								this, line->objectId() ) );

					bool intersected = false;
					bool intersectedEnds = false;
					FormLine * intersectedLine = 0;

					QPointF p = d->lineStartPoint( mouseEvent->pos(),
						intersected, intersectedEnds, intersectedLine );

					if( !intersected && PageAction::instance()->isSnapEnabled() )
						p = d->m_snap->snapPos();

					const QLineF l = line->line();

					line->setLine( l.p1().x(), l.p1().y(), p.x(), p.y() );

					if( d->m_polyline )
					{
						if( !d->m_currentLines.isEmpty() )
						{
							d->m_currentPoly = new FormPolyline( this, this );

							const QString id =
								d->m_currentLines.first()->objectId();

							d->m_currentPoly->setObjectId( id );

							d->m_currentPoly->appendLine(
								d->m_currentLines.first()->line() );
							d->m_currentPoly->showHandles( true );

							delete d->m_currentLines.first();

							d->m_currentLines.clear();
						}

						d->m_currentPoly->appendLine( line->line() );

						d->m_undoStack->push( new UndoAddLineToPoly( this,
							d->m_currentPoly->objectId(),
							line->line() ) );

						scene()->removeItem( line );

						delete line;

						d->m_current = d->m_currentPoly;

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

			case PageAction::InsertText :
			{
				FormText * text =
					onReleaseWithRectPlacer< FormText, Cfg::Text > (
						scene(), d.data(), mouseEvent, this );

				if( text )
				{
					text->setFocus();

					QTextCursor c = text->textCursor();
					c.select( QTextCursor::Document );
					text->setTextCursor( c );

					d->m_docs.insert( text->document(), text );

					connect( text->document(), &QTextDocument::undoCommandAdded,
						this, &Page::undoCommandInTextAdded );
				}

				emit changed();

				return;
			}
				break;

			case PageAction::DrawRect :
			{
				FormRect * rect = dynamic_cast< FormRect* > ( d->m_current );

				if( rect )
				{
					QPointF p = mouseEvent->pos();

					if( PageAction::instance()->isSnapEnabled() )
						p = d->m_snap->snapPos();

					QRectF r = rect->rectangle();
					r.setBottomRight( p );

					rect->setRectangle( r );

					d->m_undoStack->push( new UndoCreate< FormRect, Cfg::Rect > (
						this, rect->objectId() ) );
				}

				update();

				d->m_pressed = false;

				mouseEvent->accept();

				emit changed();

				return;
			}
				break;

			case PageAction::DrawButton :
			{
				onReleaseWithRectPlacer< FormButton, Cfg::Button > (
					scene(), d.data(), mouseEvent, this );

				emit changed();

				return;
			}
				break;

			case PageAction::DrawCheckBox :
			{
				onReleaseWithRectPlacer< FormCheckBox, Cfg::CheckBox > (
					scene(), d.data(), mouseEvent, this );

				emit changed();

				return;
			}
				break;

			case PageAction::DrawRadioButton :
			{
				onReleaseWithRectPlacer< FormRadioButton, Cfg::CheckBox > (
					scene(), d.data(), mouseEvent, this );

				emit changed();

				return;
			}
				break;

			case PageAction::DrawComboBox :
			{
				onReleaseWithRectPlacer< FormComboBox, Cfg::ComboBox > (
					scene(), d.data(), mouseEvent, this );

				emit changed();

				return;
			}
				break;

			case PageAction::DrawSpinBox :
			{
				onReleaseWithRectPlacer< FormSpinBox, Cfg::SpinBox > (
					scene(), d.data(), mouseEvent, this );

				emit changed();

				return;
			}
				break;

			case PageAction::DrawHSlider :
			{
				onReleaseWithRectPlacer< FormHSlider, Cfg::HSlider > (
					scene(), d.data(), mouseEvent, this );

				emit changed();

				return;
			}
				break;

			case PageAction::DrawVSlider :
			{
				onReleaseWithRectPlacer< FormVSlider, Cfg::VSlider > (
					scene(), d.data(), mouseEvent, this );

				emit changed();

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
Page::hoverEnterEvent( QGraphicsSceneHoverEvent * event )
{
	d->m_snap->setSnapPos( event->pos() );

	event->ignore();
}

void
Page::hoverMoveEvent( QGraphicsSceneHoverEvent * event )
{
	d->m_snap->setSnapPos( event->pos() );

	event->ignore();
}

void
Page::dragEnterEvent( QGraphicsSceneDragDropEvent * event )
{
	d->m_snap->setSnapPos( event->pos() );

	if( event->mimeData()->hasImage() )
		event->acceptProposedAction();
	else
		QGraphicsObject::dragEnterEvent( event );
}

void
Page::dragMoveEvent( QGraphicsSceneDragDropEvent * event )
{
	d->m_snap->setSnapPos( event->pos() );

	if( event->mimeData()->hasImage() )
		event->acceptProposedAction();
	else
		QGraphicsObject::dragMoveEvent( event );
}

void
Page::dropEvent( QGraphicsSceneDragDropEvent * event )
{
	if( event->mimeData()->hasImage() )
	{
		d->hideHandlesOfCurrent();

		FormImage * image = new FormImage( this, this );

		const QString id = d->id();

		image->setObjectId( id );

		d->m_undoStack->push( new UndoCreate< FormImage, Cfg::Image > (
			this, image->objectId() ) );

		d->m_ids.append( id );

		if( PageAction::instance()->mode() == PageAction::Select )
		{
			image->setFlag( QGraphicsItem::ItemIsSelectable, true );

			image->setSelected( true );
		}

		d->m_snap->setSnapPos( event->pos() );

		if( PageAction::instance()->isSnapEnabled() )
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

void
Page::removeDocFromMap( QObject * doc )
{
	d->m_docs.remove( doc );
}

void
Page::updateDocItemInMap( QObject * doc, FormText * text )
{
	d->m_docs[ doc ] = text;
}

void
Page::setCurrentLine( FormLine * line )
{
	d->m_current = line;

	d->m_currentLines.clear();

	if( line )
		d->m_currentLines.append( line );

	d->m_currentPoly = Q_NULLPTR;
}

void
Page::setCurrentPolyLine( FormPolyline * line )
{
	d->m_current = line;

	d->m_currentLines.clear();

	d->m_currentPoly = line;

	d->m_polyline = true;
}

} /* namespace Core */

} /* namespace Prototyper */
