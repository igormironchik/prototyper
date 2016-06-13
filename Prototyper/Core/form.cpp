
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
#include "form_size_dlg.hpp"
#include "desc_window.hpp"
#include "utils.hpp"
#include "form_rectangle.hpp"
#include "form_button.hpp"
#include "form_button.hpp"
#include "form_checkbox.hpp"
#include "form_radio_button.hpp"
#include "form_combobox.hpp"
#include "form_spinbox.hpp"
#include "form_hslider.hpp"
#include "form_vslider.hpp"

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
#include <QTextDocument>
#include <QSharedPointer>

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
		,	m_btns( FormProperties::MinimizeButton |
				FormProperties::MaximizeButton |
				FormProperties::CloseButton )
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
	//! Create text.
	FormText * createText( const Cfg::Text & cfg );
	//! Create group.
	FormGroup * createGroup( const Cfg::Group & cfg );
	//! Create element.
	template< class Elem, class Config >
	Elem * createElem( const Config & cfg );
	//! Create element with rect.
	template< class Elem, class Config >
	Elem * createElemWithRect( const Config & cfg, const QRectF & rect );
	//! Clear IDs.
	void clearIds( FormGroup * group );
	//! Add IDs.
	void addIds( FormGroup * group );
	//! Create description.
	void createDescription( const QString & id );
	//! Set text.
	void setText( const QSharedPointer< QTextDocument > & doc,
		const QList< Cfg::TextStyle > & text );
	//! Update link.
	void updateLink( const QList< QGraphicsItem* > & childItems,
		const QString & oldName, const QString & name );
	//! Hide handles of current item.
	void hideHandlesOfCurrent();
	//! Remove descriptions of the object.
	void removeDescriptions( FormObject * obj );

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
	//! Descriptions.
	QMap< QString, QSharedPointer< QTextDocument > > m_desc;
	//! Buttons.
	FormProperties::Buttons m_btns;
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

				FormText * text = dynamic_cast< FormText* > ( item );

				if( text )
					text->enableEditing( false );
			}
		}

		m_model->removeObject( tmp, q );

		q->scene()->removeItem( tmp );

		if( m_desc.contains( tmp->objectId() ) )
		{
			TopGui::instance()->projectWindow()->descWindow()->deleteItem(
				q, tmp->objectId() );

			m_desc.remove( tmp->objectId() );
		}

		m_model->endRemoveObject();

		tmp->postDeletion();

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

	m_btns = FormProperties::Buttons();

	if( m_cfg.windowButtons().contains( Cfg::c_minimize ) )
		m_btns |= FormProperties::MinimizeButton;

	if( m_cfg.windowButtons().contains( Cfg::c_maximize ) )
		m_btns |= FormProperties::MaximizeButton;

	if( m_cfg.windowButtons().contains( Cfg::c_close ) )
		m_btns |= FormProperties::CloseButton;

	TopGui::instance()->projectWindow()->formHierarchy()->view()->update(
		m_model->index( q ) );

	foreach( const Cfg::Line & c, m_cfg.line() )
	{
		FormLine * line = createElem< FormLine > ( c );

		m_model->addObject( line, q );
	}

	foreach( const Cfg::Polyline & c, m_cfg.polyline() )
	{
		FormPolyline * line = createElem< FormPolyline > ( c );

		m_model->addObject( line, q );
	}

	foreach( const Cfg::Text & c, m_cfg.text() )
	{
		FormText * text = createText( c );

		m_model->addObject( text, q );
	}

	foreach( const Cfg::Image & c, m_cfg.image() )
	{
		FormImage * image = createElem< FormImage > ( c );

		m_model->addObject( image, q );
	}

	foreach( const Cfg::Rect & c, m_cfg.rect() )
	{
		FormRect * rect = createElem< FormRect > ( c );

		m_model->addObject( rect, q );
	}

	foreach( const Cfg::Group & c, m_cfg.group() )
	{
		FormGroup * group = createGroup( c );

		m_model->addObject( group, q );
	}

	foreach( const Cfg::Description & c, m_cfg.desc() )
	{
		createDescription( c.id() );

		setText( m_desc[ c.id() ], c.text() );
	}

	foreach( const Cfg::Button & c, m_cfg.button() )
	{
		FormButton * e = createElemWithRect< FormButton > ( c, QRectF() );

		m_model->addObject( e, q );
	}

	foreach( const Cfg::ComboBox & c, m_cfg.combobox() )
	{
		FormComboBox * e = createElemWithRect< FormComboBox > ( c, QRectF() );

		m_model->addObject( e, q );
	}

	foreach( const Cfg::CheckBox & c, m_cfg.radiobutton() )
	{
		FormRadioButton * e = createElemWithRect< FormRadioButton > ( c, QRectF() );

		m_model->addObject( e, q );
	}

	foreach( const Cfg::CheckBox & c, m_cfg.checkbox() )
	{
		FormCheckBox * e = createElemWithRect< FormCheckBox > ( c, QRectF() );

		m_model->addObject( e, q );
	}

	foreach( const Cfg::HSlider & c, m_cfg.hslider() )
	{
		FormHSlider * e = createElemWithRect< FormHSlider > ( c, QRectF() );

		m_model->addObject( e, q );
	}

	foreach( const Cfg::VSlider & c, m_cfg.vslider() )
	{
		FormVSlider * e = createElemWithRect< FormVSlider > ( c, QRectF() );

		m_model->addObject( e, q );
	}

	foreach( const Cfg::SpinBox & c, m_cfg.spinbox() )
	{
		FormSpinBox * e = createElemWithRect< FormSpinBox > ( c, QRectF() );

		m_model->addObject( e, q );
	}

	q->setLink( m_cfg.link() );

	q->update();
}

void
FormPrivate::clear()
{
	m_ids.clear();

	m_desc.clear();

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

template< class Elem, class Config >
inline
Elem *
FormPrivate::createElem( const Config & cfg )
{
	Elem * e = new Elem( q, q );

	e->setCfg( cfg );

	m_ids.append( e->objectId() );

	return e;
}

template< class Elem, class Config >
inline
Elem *
FormPrivate::createElemWithRect( const Config & cfg, const QRectF & rect )
{
	Elem * e = new Elem( rect, q, q );

	e->setCfg( cfg );

	m_ids.append( e->objectId() );

	return e;
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

void
FormPrivate::createDescription( const QString & id )
{
	QSharedPointer< QTextDocument > doc( new QTextDocument );

	QFont f = QApplication::font();
	f.setPointSize( 10.0 );

	doc->setDefaultFont( f );

	QTextCursor c( doc.data() );

	QTextCharFormat fmt = c.charFormat();
	fmt.setFontPointSize( 10.0 );

	c.setCharFormat( fmt );

	m_desc[ id ] = doc;
}

void
FormPrivate::setText( const QSharedPointer< QTextDocument > & doc,
	const QList< Cfg::TextStyle > & text )
{
	doc->clear();

	Cfg::fillTextDocument( doc.data(), text );
}

void
FormPrivate::updateLink( const QList< QGraphicsItem* > & childItems,
	const QString & oldName, const QString & name )
{
	foreach( QGraphicsItem * item, childItems )
	{
		FormObject * obj = dynamic_cast< FormObject* > ( item );

		if( obj )
		{
			if( obj->link() == oldName )
			{
				obj->setLink( name );

				m_model->update( obj );
			}

			if( obj->objectType() == FormObject::GroupType )
				updateLink( item->childItems(), oldName, name );
		}
	}
}

void
FormPrivate::hideHandlesOfCurrent()
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

void
FormPrivate::removeDescriptions( FormObject * obj )
{
	if( m_desc.contains( obj->objectId() ) )
	{
		TopGui::instance()->projectWindow()->descWindow()->deleteItem(
			q, obj->objectId() );

		m_desc.remove( obj->objectId() );
	}

	if( obj->objectType() == FormObject::GroupType )
	{
		FormGroup * group = dynamic_cast< FormGroup* > ( obj );

		if( group )
		{
			foreach( QGraphicsItem * item, group->childItems() )
			{
				FormObject * tmp = dynamic_cast< FormObject* > ( item );

				if( tmp )
					removeDescriptions( tmp );
			}
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

	if( d->m_btns.testFlag( FormProperties::MinimizeButton ) )
		c.windowButtons().append( Cfg::c_minimize );

	if( d->m_btns.testFlag( FormProperties::MaximizeButton ) )
		c.windowButtons().append( Cfg::c_maximize );

	if( d->m_btns.testFlag( FormProperties::CloseButton ) )
		c.windowButtons().append( Cfg::c_close );

	c.line().clear();
	c.polyline().clear();
	c.text().clear();
	c.image().clear();
	c.group().clear();
	c.desc().clear();
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

				case FormObject::RectType :
				{
					FormRect * rect = dynamic_cast< FormRect* > ( item );

					if( rect )
						c.rect().append( rect->cfg() );
				}
					break;

				case FormObject::GroupType :
				{
					FormGroup * group = dynamic_cast< FormGroup* > ( item );

					if( group )
						c.group().append( group->cfg() );
				}
					break;

				case FormObject::ButtonType :
				{
					FormButton * btn = dynamic_cast< FormButton* > ( item );

					if( btn )
						c.button().append( btn->cfg() );
				}
					break;

				case FormObject::CheckBoxType :
				{
					FormCheckBox * chk = dynamic_cast< FormCheckBox* > ( item );

					if( chk )
						c.checkbox().append( chk->cfg() );
				}
					break;

				case FormObject::RadioButtonType :
				{
					FormRadioButton * r = dynamic_cast< FormRadioButton* > ( item );

					if( r )
						c.radiobutton().append( r->cfg() );
				}
					break;

				case FormObject::ComboBoxType :
				{
					FormComboBox * cb = dynamic_cast< FormComboBox* > ( item );

					if( cb )
						c.combobox().append( cb->cfg() );
				}
					break;

				case FormObject::SpinBoxType :
				{
					FormSpinBox * sb = dynamic_cast< FormSpinBox* > ( item );

					if( sb )
						c.spinbox().append( sb->cfg() );
				}
					break;

				case FormObject::HSliderType :
				{
					FormHSlider * hs = dynamic_cast< FormHSlider* > ( item );

					if( hs )
						c.hslider().append( hs->cfg() );
				}
					break;

				case FormObject::VSliderType :
				{
					FormVSlider * vs = dynamic_cast< FormVSlider* > ( item );

					if( vs )
						c.vslider().append( vs->cfg() );
				}
					break;

				default :
					break;
			}
		}
	}

	c.setTabName( objectId() );

	auto last = d->m_desc.constEnd();

	for( auto it = d->m_desc.constBegin(); it != last; ++it )
	{
		Cfg::Description desc;
		desc.setId( it.key() );
		desc.setText( Cfg::text( QTextCursor( it.value().data() ),
			it.value()->toPlainText() ) );

		c.desc().append( desc );
	}

	c.setLink( link() );

	return c;
}

void
Form::setCfg( const Cfg::Form & c )
{
	d->m_cfg = c;

	d->updateFromCfg();

	TopGui::instance()->projectWindow()->formHierarchy()->view()->
		resizeColumnToContents( 1 );
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
	d->hideHandlesOfCurrent();

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

const QStringList &
Form::ids() const
{
	return d->m_ids;
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
		if( item == d->m_current )
			d->m_current = 0;

		FormObject * obj = dynamic_cast< FormObject* > ( item );

		if( obj )
		{
			d->m_model->removeObject( obj, this );

			d->removeDescriptions( obj );
		}

		scene()->removeItem( item );

		if( obj )
		{
			d->m_model->endRemoveObject();

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

				default :
				{
					obj->postDeletion();
				}
					break;
			}
		}

		delete item;
	}
}

void
Form::updateLink( const QString & oldName, const QString & name )
{
	if( link() == oldName )
	{
		setLink( name );

		d->m_model->update( this );
	}

	d->updateLink( childItems(), oldName, name );
}

QRectF
Form::boundingRect() const
{
	if( !d.isNull() )
	{
		const qreal wh = ( d->m_btns.testFlag( FormProperties::MinimizeButton ) ?
			25.0 : ( d->m_btns.testFlag( FormProperties::MaximizeButton ) ?
				25.0 : ( d->m_btns.testFlag( FormProperties::CloseButton ) ?
					25.0 : 0.0 ) ) );

		return QRectF( 0, 0, d->m_cfg.size().width(),
			d->m_cfg.size().height() + wh );
	}
	else
		return QRectF();
}

void
Form::paint( QPainter * painter, const QStyleOptionGraphicsItem * option,
	QWidget * widget )
{
	Q_UNUSED( option )
	Q_UNUSED( widget )

	draw( painter, d->m_cfg.size().width(),
		d->m_cfg.size().height(), d->m_btns, d->m_cfg.gridStep(),
		d->m_gridMode == ShowGrid );
}

void
Form::draw( QPainter * painter, int width, int height,
	FormProperties::Buttons btns, int gridStep, bool drawGrid )
{
	const int wh = ( btns.testFlag( FormProperties::MinimizeButton ) ?
		30 : ( btns.testFlag( FormProperties::MaximizeButton ) ?
			30 : ( btns.testFlag( FormProperties::CloseButton ) ?
				30 : 0 ) ) );

	static const QColor gridColor = Qt::gray;

	painter->setPen( gridColor );

	painter->setRenderHint( QPainter::Antialiasing, false );

	if( wh )
	{
		painter->setBrush( QBrush( gridColor ) );

		painter->drawRect( QRect( 0, 0, width, wh ) );

		painter->setBrush( Qt::white );

		const int w = 20;
		const int o = 5;

		const int trd = qRound( (qreal) w / 3.5 );
		const int half = qRound( (qreal) w / 2.0 );

		int i = 1;

		if( btns.testFlag( FormProperties::CloseButton ) )
		{
			const QRect b( width - o * i - w * i, o, w, w );

			painter->drawEllipse( b );

			painter->drawLine( b.x() + trd, b.y() + trd,
				b.x() + w - trd, b.y() + w - trd );

			painter->drawLine( b.x() + trd, b.y() + w - trd,
				b.x() + w - trd, b.y() + trd );

			++i;
		}

		if( btns.testFlag( FormProperties::MaximizeButton ) )
		{
			const QRect b( width - o * i - w * i, o, w, w );

			painter->save();

			painter->setPen( Qt::white );

			painter->drawLine( b.x() + half, b.y() + trd,
				b.x() + w - trd, b.y() + half );

			painter->drawLine( b.x() + w - trd, b.y() + half,
				b.x() + half, b.y() + w - trd );

			painter->drawLine( b.x() + half, b.y() + w - trd,
				b.x() + trd, b.y() + half );

			painter->drawLine( b.x() + trd, b.y() + half,
				b.x() + half, b.y() + trd );

			painter->restore();

			++i;
		}

		if( btns.testFlag( FormProperties::MinimizeButton ) )
		{
			const QRect b( width - o * i - w * i, o, w, w );

			painter->save();

			painter->setPen( Qt::white );

			painter->drawLine( b.x() + trd, b.y() + half,
				b.x() + half, b.y() + w - trd );

			painter->drawLine( b.x() + half, b.y() + w - trd,
				b.x() + w - trd, b.y() + half );

			painter->restore();
		}
	}

	painter->setBrush( Qt::white );

	QRect r( 0, wh, width, height );

	painter->drawRect( r );

	if( drawGrid )
	{
		for( int x = gridStep; x < width; x += gridStep )
			painter->drawLine( x, wh, x, height + wh );

		for( int y = gridStep + wh; y < height + wh; y += gridStep )
			painter->drawLine( 0, y, width, y );
	}
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

				if( d->m_desc.contains( old ) )
				{
					QSharedPointer< QTextDocument > doc =
						d->m_desc[ obj->objectId() ];

					d->m_desc.remove( obj->objectId() );

					d->m_desc[ dlg.name() ] = doc;
				}

				TopGui::instance()->projectWindow()->descWindow()->renameItem(
					this, obj->objectId(), dlg.name() );

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
Form::renameForm( const QString & name )
{
	const QString old = objectId();

	if( d->m_desc.contains( old ) )
	{
		QSharedPointer< QTextDocument > doc =
			d->m_desc[ old ];

		d->m_desc.remove( old );

		d->m_desc[ name ] = doc;
	}

	TopGui::instance()->projectWindow()->descWindow()->renameItem(
		this, old, name );

	setObjectId( name );

	d->m_ids.removeOne( old );

	d->m_ids.append( name );
}

void
Form::editDescription( const QString & id )
{
	if( !d->m_desc.contains( id ) )
		d->createDescription( id );

	TopGui::instance()->projectWindow()->descWindow()->setEditors(
		id, d->m_desc, this );

	TopGui::instance()->projectWindow()->descWindow()->show();
}

void
Form::slotSetGridStep()
{
	TopGui::instance()->projectWindow()->setGridStep(
		d->m_cfg.gridStep(), false );
}

void
Form::resizeForm()
{
	FormSizeDlg dlg( d->m_cfg.size().width(),
		d->m_cfg.size().height(),
		TopGui::instance()->projectWindow() );

	if( dlg.exec() == QDialog::Accepted )
	{
		d->m_cfg.size().width() = dlg.size().width();
		d->m_cfg.size().height() = dlg.size().height();

		emit changed();

		update();

		scene()->update();
	}
}

void
Form::properties()
{
	FormProperties dlg;

	dlg.setButtons( d->m_btns );

	if( dlg.exec() == QDialog::Accepted )
	{
		d->m_btns = dlg.buttons();

		emit changed();

		update();
	}
}

void
Form::contextMenuEvent( QGraphicsSceneContextMenuEvent * event )
{
	FormRectPlacer * placer = dynamic_cast< FormRectPlacer* > ( d->m_current );

	if( placer )
		delete placer;

	QMenu menu;
	menu.addAction( TopGui::instance()->projectWindow()->showHideGridAction() );
	menu.addAction( d->m_gridStepAction );
	menu.addAction( QIcon( ":/Core/img/transform-scale.png" ),
		tr( "Resize" ), this, SLOT( resizeForm() ) );

	menu.addSeparator();

	menu.addAction( QIcon( ":/Core/img/configure.png" ),
		tr( "Properties" ), this, &Form::properties );

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
			case FormAction::DrawButton :
			case FormAction::DrawCheckBox :
			case FormAction::DrawRadioButton :
			case FormAction::DrawComboBox :
			case FormAction::DrawSpinBox :
			case FormAction::DrawHSlider :
			case FormAction::DrawVSlider :
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

			case FormAction::DrawRect :
			{
				FormRect * rect = dynamic_cast< FormRect* > ( d->m_current );

				if( rect )
				{
					QRectF r = rect->rect();
					r.setBottomRight( mouseEvent->pos() );

					rect->setRect( r );
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
				if( d->m_current != d->m_currentPoly )
					d->hideHandlesOfCurrent();

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
			case FormAction::DrawButton :
			case FormAction::DrawCheckBox :
			case FormAction::DrawRadioButton :
			case FormAction::DrawComboBox :
			case FormAction::DrawSpinBox :
			case FormAction::DrawHSlider :
			case FormAction::DrawVSlider :
			{
				d->hideHandlesOfCurrent();

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

			case FormAction::DrawRect :
			{
				d->hideHandlesOfCurrent();

				d->m_pressed = true;

				FormRect * rect = new FormRect( this, this );

				d->m_current = rect;

				const QString id = d->id();

				rect->setObjectId( id );

				d->m_ids.append( id );

				d->m_model->addObject( rect, this );

				QPointF p = mouseEvent->pos();

				if( FormAction::instance()->isSnapEnabled() )
					p = d->m_snap->snapPos();

				rect->setRect( QRectF( p, QSizeF( 0.0, 0.0 ) ) );

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

template< class Elem >
Elem * onReleaseWithRectPlacer( QGraphicsScene * scene, FormPrivate * d,
	QGraphicsSceneMouseEvent * mouseEvent, Form * form )
{
	scene->removeItem( d->m_current );

	FormRectPlacer * rect = dynamic_cast< FormRectPlacer* >
		( d->m_current );

	Elem * elem = 0;

	if( rect )
	{
		QPointF p = mouseEvent->pos();

		if( FormAction::instance()->isSnapEnabled() )
			p = d->m_snap->snapPos();

		QRectF r = rect->rect();
		r.setBottomRight( p );

		elem = new Elem( r, form, form );

		const QString id = d->id();

		elem->setObjectId( id );

		d->m_ids.append( id );

		d->m_model->addObject( elem, form );
	}

	delete d->m_current;

	d->m_current = elem;

	mouseEvent->accept();

	d->m_pressed = false;

	return elem;
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

			case FormAction::InsertText :
			{
				FormText * text =
					onReleaseWithRectPlacer< FormText >(
						scene(), d.data(), mouseEvent, this );

				if( text )
				{
					text->setFocus();

					QTextCursor c = text->textCursor();
					c.select( QTextCursor::Document );
					text->setTextCursor( c );
				}

				emit changed();

				return;
			}
				break;

			case FormAction::DrawRect :
			{
				FormRect * rect = dynamic_cast< FormRect* > ( d->m_current );

				if( rect )
				{
					QPointF p = mouseEvent->pos();

					if( FormAction::instance()->isSnapEnabled() )
						p = d->m_snap->snapPos();

					QRectF r = rect->rect();
					r.setBottomRight( p );

					rect->setRect( r );
				}

				update();

				d->m_pressed = false;

				mouseEvent->accept();

				emit changed();

				return;
			}
				break;

			case FormAction::DrawButton :
			{
				onReleaseWithRectPlacer< FormButton > (
					scene(), d.data(), mouseEvent, this );

				emit changed();

				return;
			}
				break;

			case FormAction::DrawCheckBox :
			{
				onReleaseWithRectPlacer< FormCheckBox > (
					scene(), d.data(), mouseEvent, this );

				emit changed();

				return;
			}
				break;

			case FormAction::DrawRadioButton :
			{
				onReleaseWithRectPlacer< FormRadioButton > (
					scene(), d.data(), mouseEvent, this );

				emit changed();

				return;
			}
				break;

			case FormAction::DrawComboBox :
			{
				onReleaseWithRectPlacer< FormComboBox > (
					scene(), d.data(), mouseEvent, this );

				emit changed();

				return;
			}
				break;

			case FormAction::DrawSpinBox :
			{
				onReleaseWithRectPlacer< FormSpinBox > (
					scene(), d.data(), mouseEvent, this );

				emit changed();

				return;
			}
				break;

			case FormAction::DrawHSlider :
			{
				onReleaseWithRectPlacer< FormHSlider > (
					scene(), d.data(), mouseEvent, this );

				emit changed();

				return;
			}
				break;

			case FormAction::DrawVSlider :
			{
				onReleaseWithRectPlacer< FormVSlider > (
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
		d->hideHandlesOfCurrent();

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
