
/*!
	\file

	\author Igor Mironchik (igor.mironchik at gmail dot com).

	Copyright (c) 2016-2021 Igor Mironchik

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
#include "form_group.hpp"
#include "form_move_handle.hpp"
#include "form_line.hpp"
#include "form_polyline.hpp"
#include "form_text.hpp"
#include "form_image.hpp"
#include "form_rectangle.hpp"
#include "form_button.hpp"
#include "form_combobox.hpp"
#include "form_radio_button.hpp"
#include "form_checkbox.hpp"
#include "form_hslider.hpp"
#include "form_vslider.hpp"
#include "form_spinbox.hpp"
#include "page.hpp"
#include "form_undo_commands.hpp"
#include "utils.hpp"
#include "constants.hpp"

// Qt include.
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsScene>
#include <QEvent>
#include <QGraphicsSceneMouseEvent>
#include <QUndoStack>

// C++ include.
#include <algorithm>


namespace Prototyper {

namespace Core {

//
// FormGroupPrivate
//

class FormGroupPrivate {
public:
	explicit FormGroupPrivate( FormGroup * parent )
		:	q( parent )
		,	m_center( nullptr )
		,	m_topLeft( nullptr )
		,	m_topRight( nullptr )
		,	m_bottomRight( nullptr )
		,	m_bottomLeft( nullptr )
		,	m_handleMoved( false )
	{
	}

	//! Init.
	void init();

	//! Parent.
	FormGroup * q;
	//! Center handle.
	std::unique_ptr< FormMoveHandle > m_center;
	//! Top-left handle.
	std::unique_ptr< FormMoveHandle > m_topLeft;
	//! Top-right handle.
	std::unique_ptr< FormMoveHandle > m_topRight;
	//! Bottom-right handle.
	std::unique_ptr< FormMoveHandle > m_bottomRight;
	//! Bottom-left handle.
	std::unique_ptr< FormMoveHandle > m_bottomLeft;
	//! Child.
	QList< FormObject* > m_child;
	//! Old pos.
	QPointF m_oldPos;
	//! Is handle moved?
	bool m_handleMoved;
}; // class FormGroupPrivate

void
FormGroupPrivate::init()
{
	m_center.reset( new FormMoveHandle( c_halfHandleSize,
		QPointF( c_halfHandleSize, c_halfHandleSize ), q,
		q->parentItem(), q->page(), Qt::SizeAllCursor ) );
	m_center->setZValue( c_mostTopZValue );
	m_center->hide();

	m_topLeft.reset( new FormMoveHandle( c_halfHandleSize,
		QPointF( c_halfHandleSize * c_halfDivider, c_halfHandleSize * c_halfDivider ), q,
		q->parentItem(), q->page(), Qt::SizeAllCursor ) );
	m_topLeft->setZValue( c_mostTopZValue );
	m_topLeft->hide();

	m_topRight.reset( new FormMoveHandle( c_halfHandleSize,
		QPointF( 0.0, c_halfHandleSize * c_halfDivider ), q,
		q->parentItem(), q->page(), Qt::SizeAllCursor ) );
	m_topRight->setZValue( c_mostTopZValue );
	m_topRight->hide();

	m_bottomRight.reset( new FormMoveHandle( c_halfHandleSize, QPointF( 0.0, 0.0 ), q,
		q->parentItem(), q->page(), Qt::SizeAllCursor ) );
	m_bottomRight->setZValue( c_mostTopZValue );
	m_bottomRight->hide();

	m_bottomLeft.reset( new FormMoveHandle( c_halfHandleSize,
		QPointF( c_halfHandleSize * c_halfDivider, 0.0 ), q,
		q->parentItem(), q->page(), Qt::SizeAllCursor ) );
	m_bottomLeft->setZValue( c_mostTopZValue );
	m_bottomLeft->hide();
}


//
// FormGroup
//

FormGroup::FormGroup( Page * page, QGraphicsItem * parent )
	:	QGraphicsItemGroup( parent )
	,	FormObject( FormObject::GroupType, page, 0 )
	,	d( new FormGroupPrivate( this ) )
{
	d->init();
}

FormGroup::~FormGroup() = default;

Cfg::Group
FormGroup::cfg() const
{
	Cfg::Group c;

	foreach( QGraphicsItem * item, childItems() )
	{
		auto * obj = dynamic_cast< FormObject* > ( item );

		if( obj )
		{
			switch( obj->objectType() )
			{
				case FormObject::LineType :
				{
					auto * line = dynamic_cast< FormLine* > ( item );

					if( line )
						c.line().push_back( line->cfg() );
				}
					break;

				case FormObject::PolylineType :
				{
					auto * poly = dynamic_cast< FormPolyline* > ( item );

					if( poly )
						c.polyline().push_back( poly->cfg() );
				}
					break;

				case FormObject::TextType :
				{
					auto * text = dynamic_cast< FormText* > ( item );

					if( text )
						c.text().push_back( text->cfg() );
				}
					break;

				case FormObject::ImageType :
				{
					auto * image = dynamic_cast< FormImage* > ( item );

					if( image )
						c.image().push_back( image->cfg() );
				}
					break;

				case FormObject::RectType :
				{
					auto * rect = dynamic_cast< FormRect* > ( item );

					if( rect )
						c.rect().push_back( rect->cfg() );
				}
					break;

				case FormObject::GroupType :
				{
					auto * group = dynamic_cast< FormGroup* > ( item );

					if( group )
						c.group().push_back( group->cfg() );
				}
					break;

				case FormObject::ButtonType :
				{
					auto * e = dynamic_cast< FormButton* > ( item );

					if( e )
						c.button().push_back( e->cfg() );
				}
					break;

				case FormObject::ComboBoxType :
				{
					auto * e = dynamic_cast< FormComboBox* > ( item );

					if( e )
						c.combobox().push_back( e->cfg() );
				}
					break;

				case FormObject::RadioButtonType :
				{
					auto * e = dynamic_cast< FormRadioButton* > ( item );

					if( e )
						c.radiobutton().push_back( e->cfg() );
				}
					break;

				case FormObject::CheckBoxType :
				{
					auto * e = dynamic_cast< FormCheckBox* > ( item );

					if( e )
						c.checkbox().push_back( e->cfg() );
				}
					break;

				case FormObject::HSliderType :
				{
					auto * e = dynamic_cast< FormHSlider* > ( item );

					if( e )
						c.hslider().push_back( e->cfg() );
				}
					break;

				case FormObject::VSliderType :
				{
					auto * e = dynamic_cast< FormVSlider* > ( item );

					if( e )
						c.vslider().push_back( e->cfg() );
				}
					break;

				case FormObject::SpinBoxType :
				{
					auto * e = dynamic_cast< FormSpinBox* > ( item );

					if( e )
						c.spinbox().push_back( e->cfg() );
				}
					break;

				default :
					break;
			}
		}
	}

	c.set_objectId( objectId() );

	Cfg::Point p;
	p.set_x( MmPx::instance().toMmX( pos().x() ) );
	p.set_y( MmPx::instance().toMmY( pos().y() ) );

	c.set_pos( p );

	c.set_z( zValue() );

	return c;
}

void
FormGroup::setCfg( const Cfg::Group & c )
{
	for( const Cfg::Line & cfg : c.line() )
	{
		auto * line = new FormLine( page(), page() );

		line->setCfg( cfg );

		addToGroup( line );
	}

	for( const Cfg::Polyline & cfg : c.polyline() )
	{
		auto * poly = new FormPolyline( page(), page() );

		poly->setCfg( cfg );

		addToGroup( poly );
	}

	for( const Cfg::Text & cfg : c.text() )
	{
		const QRectF r( 0.0, 0.0, cfg.textWidth(), 0.0 );

		auto * text = new FormText( r, page(), page() );

		text->setCfg( cfg );

		addToGroup( text );
	}

	for( const Cfg::Image & cfg : c.image() )
	{
		auto * image = new FormImage( page(), page() );

		image->setCfg( cfg );

		addToGroup( image );
	}

	for( const Cfg::Rect & cfg : c.rect() )
	{
		auto * rect = new FormRect( page(), page() );

		rect->setCfg( cfg );

		addToGroup( rect );
	}

	for( const Cfg::Group & cfg : c.group() )
	{
		auto * group = new FormGroup( page(), page() );

		group->setCfg( cfg );

		addToGroup( group );
	}

	for( const Cfg::Button & cfg : c.button() )
		createElemWithRect< FormButton > ( cfg );

	for( const Cfg::ComboBox & cfg : c.combobox() )
		createElemWithRect< FormComboBox > ( cfg );

	for( const Cfg::CheckBox & cfg : c.radiobutton() )
		createElemWithRect< FormRadioButton > ( cfg );

	for( const Cfg::CheckBox & cfg : c.checkbox() )
		createElemWithRect< FormCheckBox > ( cfg );

	for( const Cfg::HSlider & cfg : c.hslider() )
		createElemWithRect< FormHSlider > ( cfg );

	for( const Cfg::VSlider & cfg : c.vslider() )
		createElemWithRect< FormVSlider > ( cfg );

	for( const Cfg::SpinBox & cfg : c.spinbox() )
		createElemWithRect< FormSpinBox > ( cfg );

	setObjectId( c.objectId() );

	setZValue( c.z() );

	setPos( QPointF( MmPx::instance().fromMmX( c.pos().x() ),
		MmPx::instance().fromMmY( c.pos().y() ) ) );
}

QList< QGraphicsItem* >
FormGroup::children() const
{
	QList< QGraphicsItem* > items = childItems();

	items.erase( std::remove_if( items.begin(), items.end(),
			[] ( QGraphicsItem * item ) {
				return ( dynamic_cast< FormObject* > ( item ) == nullptr ); } ),
		items.end() );

	return items;
}

QRectF
FormGroup::boundingRect() const
{
	return QGraphicsItemGroup::boundingRect()
		.adjusted( -c_halfHandleSize * c_halfDivider, -c_halfHandleSize * c_halfDivider,
			c_halfHandleSize * c_halfDivider, c_halfHandleSize * c_halfDivider );
}

void
FormGroup::paint( QPainter * painter, const QStyleOptionGraphicsItem * option,
	QWidget * widget )
{
	Q_UNUSED( widget )

	if( isSelected() && !group() )
	{
		painter->setPen( QPen( Qt::gray, 1.0, Qt::DashLine ) );

		painter->setBrush( Qt::NoBrush );

		painter->drawRect( option->rect );

		const QPointF p( option->rect.x() + option->rect.width() / 2.0 -
				 d->m_center->halfOfSize(),
			 option->rect.y() + option->rect.height() / 2.0 -
				 d->m_center->halfOfSize() );

		d->m_center->setPos( mapToParent( p ) );

		d->m_center->show();

		const QPointF p1( option->rect.x(),
			 option->rect.y() );

		d->m_topLeft->setPos( mapToParent( p1 ) );

		d->m_topLeft->show();

		const QPointF p2( option->rect.x() + option->rect.width() -
				 d->m_topRight->halfOfSize() * 2.0,
			 option->rect.y() );

		d->m_topRight->setPos( mapToParent( p2 ) );

		d->m_topRight->show();

		const QPointF p3( option->rect.x() + option->rect.width() -
				 d->m_bottomRight->halfOfSize() * 2.0,
			 option->rect.y() + option->rect.height() -
				 d->m_bottomRight->halfOfSize() * 2.0 );

		d->m_bottomRight->setPos( mapToParent( p3 ) );

		d->m_bottomRight->show();

		const QPointF p4( option->rect.x(),
			 option->rect.y() + option->rect.height() -
				 d->m_bottomLeft->halfOfSize() * 2.0 );

		d->m_bottomLeft->setPos( mapToParent( p4 ) );

		d->m_bottomLeft->show();
	}
	else
	{
		d->m_center->hide();
		d->m_topLeft->hide();
		d->m_topRight->hide();
		d->m_bottomRight->hide();
		d->m_bottomLeft->hide();
	}
}

void
FormGroup::setPosition( const QPointF & pos, bool pushUndoCommand )
{
	FormObject::setPosition( pos, pushUndoCommand );

	setPos( pos - QGraphicsItemGroup::boundingRect().topLeft() );

	update();
}

QPointF
FormGroup::position() const
{
	return pos() + QGraphicsItemGroup::boundingRect().topLeft();
}

QRectF
FormGroup::rectangle() const
{
	QRectF r = boundingRect();
	r.moveTopLeft( position() );

	return r;
}

void
FormGroup::setRectangle( const QRectF & rect, bool pushUndoCommand )
{
	FormObject::setRectangle( rect, pushUndoCommand );
}

void
FormGroup::handleMoved( const QPointF & delta, FormMoveHandle * handle )
{
	Q_UNUSED( handle )

	if( !d->m_handleMoved )
	{
		d->m_oldPos = position();

		d->m_handleMoved = true;
	}

	moveBy( delta.x(), delta.y() );
}

void
FormGroup::handleReleased( FormMoveHandle * handle )
{
	Q_UNUSED( handle )

	d->m_handleMoved = false;

	page()->undoStack()->push( new UndoMove(
		page(), objectId(),
		position() - d->m_oldPos ) );

	updatePropertiesValues();

	page()->emitChanged();
}

template< class Elem, class Config >
inline
void
FormGroup::createElemWithRect( const Config & cfg )
{
	Elem * e = new Elem( QRectF(), page(), page() );

	e->setCfg( cfg );

	addToGroup( e );
}

FormObject *
FormGroup::clone() const
{
	auto * o = new FormGroup( page(), parentItem() );

	o->setCfg( cfg() );

	o->setObjectId( page()->nextId() );

	const auto ch = o->children();

	for( const auto & i : ch )
		dynamic_cast< FormObject* > ( i )->setObjectId( page()->nextId() );

	return o;
}

} /* namespace Core */

} /* namespace Prototyper */
