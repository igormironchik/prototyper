
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
#include "form.hpp"
#include "form_undo_commands.hpp"

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
	FormGroupPrivate( FormGroup * parent )
		:	q( parent )
		,	m_center( 0 )
		,	m_topLeft( 0 )
		,	m_topRight( 0 )
		,	m_bottomRight( 0 )
		,	m_bottomLeft( 0 )
	{
	}

	~FormGroupPrivate()
	{
	}

	//! Init.
	void init();

	//! Parent.
	FormGroup * q;
	//! Center handle.
	QScopedPointer< FormMoveHandle > m_center;
	//! Top-left handle.
	QScopedPointer< FormMoveHandle > m_topLeft;
	//! Top-right handle.
	QScopedPointer< FormMoveHandle > m_topRight;
	//! Bottom-right handle.
	QScopedPointer< FormMoveHandle > m_bottomRight;
	//! Bottom-left handle.
	QScopedPointer< FormMoveHandle > m_bottomLeft;
	//! Child.
	QList< FormObject* > m_child;
	//! Old pos.
	QPointF m_oldPos;
}; // class FormGroupPrivate

void
FormGroupPrivate::init()
{
	m_center.reset( new FormMoveHandle( 3.0, QPointF( 3.0, 3.0 ), q,
		q->parentItem(), q->form(), Qt::SizeAllCursor ) );
	m_center->setZValue( 999 );
	m_center->hide();

	m_topLeft.reset( new FormMoveHandle( 3.0, QPointF( 6.0, 6.0 ), q,
		q->parentItem(), q->form(), Qt::SizeAllCursor ) );
	m_topLeft->setZValue( 999 );
	m_topLeft->hide();

	m_topRight.reset( new FormMoveHandle( 3.0, QPointF( 0.0, 6.0 ), q,
		q->parentItem(), q->form(), Qt::SizeAllCursor ) );
	m_topRight->setZValue( 999 );
	m_topRight->hide();

	m_bottomRight.reset( new FormMoveHandle( 3.0, QPointF( 0.0, 0.0 ), q,
		q->parentItem(), q->form(), Qt::SizeAllCursor ) );
	m_bottomRight->setZValue( 999 );
	m_bottomRight->hide();

	m_bottomLeft.reset( new FormMoveHandle( 3.0, QPointF( 6.0, 0.0 ), q,
		q->parentItem(), q->form(), Qt::SizeAllCursor ) );
	m_bottomLeft->setZValue( 999 );
	m_bottomLeft->hide();
}


//
// FormGroup
//

FormGroup::FormGroup( Form * form, QGraphicsItem * parent )
	:	QGraphicsItemGroup( parent )
	,	FormObject( FormObject::GroupType, form )
	,	d( new FormGroupPrivate( this ) )
{
	d->init();
}

FormGroup::~FormGroup()
{
}

Cfg::Group
FormGroup::cfg() const
{
	Cfg::Group c;

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
					FormButton * e = dynamic_cast< FormButton* > ( item );

					if( e )
						c.button().append( e->cfg() );
				}
					break;

				case FormObject::ComboBoxType :
				{
					FormComboBox * e = dynamic_cast< FormComboBox* > ( item );

					if( e )
						c.combobox().append( e->cfg() );
				}
					break;

				case FormObject::RadioButtonType :
				{
					FormRadioButton * e = dynamic_cast< FormRadioButton* > ( item );

					if( e )
						c.radiobutton().append( e->cfg() );
				}
					break;

				case FormObject::CheckBoxType :
				{
					FormCheckBox * e = dynamic_cast< FormCheckBox* > ( item );

					if( e )
						c.checkbox().append( e->cfg() );
				}
					break;

				case FormObject::HSliderType :
				{
					FormHSlider * e = dynamic_cast< FormHSlider* > ( item );

					if( e )
						c.hslider().append( e->cfg() );
				}
					break;

				case FormObject::VSliderType :
				{
					FormVSlider * e = dynamic_cast< FormVSlider* > ( item );

					if( e )
						c.vslider().append( e->cfg() );
				}
					break;

				case FormObject::SpinBoxType :
				{
					FormSpinBox * e = dynamic_cast< FormSpinBox* > ( item );

					if( e )
						c.spinbox().append( e->cfg() );
				}
					break;

				default :
					break;
			}
		}
	}

	c.setObjectId( objectId() );

	Cfg::Point p;
	p.setX( pos().x() );
	p.setY( pos().y() );

	c.setPos( p );

	c.setLink( link() );

	return c;
}

void
FormGroup::setCfg( const Cfg::Group & c )
{
	foreach( const Cfg::Line & cfg, c.line() )
	{
		FormLine * line = new FormLine( form(), form() );

		line->setCfg( cfg );

		addToGroup( line );
	}

	foreach( const Cfg::Polyline & cfg, c.polyline() )
	{
		FormPolyline * poly = new FormPolyline( form(), form() );

		poly->setCfg( cfg );

		addToGroup( poly );
	}

	foreach( const Cfg::Text & cfg, c.text() )
	{
		const QRectF r( 0.0, 0.0, cfg.textWidth(), 0.0 );

		FormText * text = new FormText( r, form(), form() );

		text->setCfg( cfg );

		addToGroup( text );
	}

	foreach( const Cfg::Image & cfg, c.image() )
	{
		FormImage * image = new FormImage( form(), form() );

		image->setCfg( cfg );

		addToGroup( image );
	}

	foreach( const Cfg::Rect & cfg, c.rect() )
	{
		FormRect * rect = new FormRect( form(), form() );

		rect->setCfg( cfg );

		addToGroup( rect );
	}

	foreach( const Cfg::Group & cfg, c.group() )
	{
		FormGroup * group = new FormGroup( form(), form() );

		group->setCfg( cfg );

		addToGroup( group );
	}

	foreach( const Cfg::Button & cfg, c.button() )
		createElemWithRect< FormButton > ( cfg );

	foreach( const Cfg::ComboBox & cfg, c.combobox() )
		createElemWithRect< FormComboBox > ( cfg );

	foreach( const Cfg::CheckBox & cfg, c.radiobutton() )
		createElemWithRect< FormRadioButton > ( cfg );

	foreach( const Cfg::CheckBox & cfg, c.checkbox() )
		createElemWithRect< FormCheckBox > ( cfg );

	foreach( const Cfg::HSlider & cfg, c.hslider() )
		createElemWithRect< FormHSlider > ( cfg );

	foreach( const Cfg::VSlider & cfg, c.vslider() )
		createElemWithRect< FormVSlider > ( cfg );

	foreach( const Cfg::SpinBox & cfg, c.spinbox() )
		createElemWithRect< FormSpinBox > ( cfg );

	setObjectId( c.objectId() );

	setPos( QPointF( c.pos().x(), c.pos().y() ) );

	setLink( c.link() );
}

QList< QGraphicsItem* >
FormGroup::children() const
{
	QList< QGraphicsItem* > items = childItems();

	std::remove_if( items.begin(), items.end(), [] ( QGraphicsItem * item )
		{ return ( dynamic_cast< FormObject* > ( item ) == 0 ); } );

	return items;
}

QRectF
FormGroup::boundingRect() const
{
	return QGraphicsItemGroup::boundingRect()
		.adjusted( -6.0, -6.0, 6.0, 6.0 );
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
FormGroup::setPosition( const QPointF & pos )
{
	form()->undoStack()->push( new UndoMove< FormGroup > ( form(),
		objectId(), pos - position() ) );

	setPos( pos );

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
FormGroup::setRectangle( const QRectF & rect )
{
	Q_UNUSED( rect )
}

void
FormGroup::handleMoved( const QPointF & delta, FormMoveHandle * handle )
{
	Q_UNUSED( handle )

	d->m_oldPos = position();

	moveBy( delta.x(), delta.y() );
}

void
FormGroup::handleReleased( FormMoveHandle * handle )
{
	Q_UNUSED( handle )

	form()->undoStack()->push( new UndoMove< FormObject > (
		form(), objectId(),
		position() - d->m_oldPos ) );

	form()->emitChanged();
}

template< class Elem, class Config >
inline
void
FormGroup::createElemWithRect( const Config & cfg )
{
	Elem * e = new Elem( QRectF(), form(), form() );

	e->setCfg( cfg );

	addToGroup( e );
}

} /* namespace Core */

} /* namespace Prototyper */
