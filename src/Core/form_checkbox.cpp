
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
#include "form_checkbox.hpp"
#include "utils.hpp"
#include "form_image_handles.hpp"
#include "form_checkbox_properties.hpp"
#include "page.hpp"
#include "form_undo_commands.hpp"
#include "form_actions.hpp"
#include "form_checkbox_private.hpp"
#include "constants.hpp"
#include "ui_form_object_properties.h"
#include "ui_form_text_properties.h"
#include "ui_form_text_style_properties.h"
#include "ui_form_checkbox_properties.h"

// Qt include.
#include <QPainter>
#include <QApplication>
#include <QMenu>
#include <QAction>
#include <QGraphicsSceneContextMenuEvent>
#include <QUndoStack>
#include <QGraphicsScene>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QSpacerItem>


namespace Prototyper {

namespace Core {

//
// FormCheckBoxPrivate
//

FormCheckBoxPrivate::FormCheckBoxPrivate( FormCheckBox * parent,
	const QRectF & rect )
	:	q( parent )
	,	m_rect( QRectF( rect.topLeft().x(), rect.topLeft().y(),
			FormCheckBox::boxHeight(), FormCheckBox::boxHeight() ) )
	,	m_checked( true )
	,	m_handles( nullptr )
	,	m_text( FormCheckBox::tr( "Text" ) )
	,	m_width( rect.width() )
{
}

FormCheckBoxPrivate::~FormCheckBoxPrivate() = default;

void
FormCheckBoxPrivate::init()
{
	m_handles.reset( new FormResizableProxy( q, q->parentItem(), q->page() ) );

	m_handles->setMinSize( q->minimumSize() );

	m_handles->hide();

	m_font = QApplication::font();

	m_font.setPixelSize( MmPx::instance().fromPtY( c_defaultFontSize ) );

	setRect( m_rect );

	q->setObjectPen( QPen( PageAction::instance()->strokeColor() ),
		false );

	q->setObjectBrush( QBrush( PageAction::instance()->fillColor() ), false );
}

void
FormCheckBoxPrivate::setRect( const QRectF & rect )
{
	const QRectF r = rect;

	m_rect = QRectF( 0.0, 0.0, FormCheckBox::boxHeight(), rect.height() );

	m_width = r.width();

	q->setPos( r.topLeft() );

	QRectF hr = q->boundingRect();
	hr.moveTopLeft( r.topLeft() );

	m_handles->setRect( hr );
}

void
FormCheckBoxPrivate::connectProperties()
{
	if( m_props )
	{
		m_objProps->connectProperties();
		m_textProps->connectProperties( this );
		m_textStyleProps->connectProperties( this );
		m_checkProps->connectProperties( this );
	}
}

void
FormCheckBoxPrivate::disconnectProperties()
{
	if( m_props )
	{
		m_objProps->disconnectProperties();
		m_textProps->disconnectProperties();
		m_textStyleProps->disconnectProperties();
		m_checkProps->disconnectProperties();
	}
}


//
// FormCheckBox
//

FormCheckBox::FormCheckBox( const QRectF & rect, Page * page,
	QGraphicsItem * parent )
	:	QGraphicsObject( parent )
	,	FormObject( FormObject::CheckBoxType, page )
	,	d( new FormCheckBoxPrivate( this, rect ) )
{
	d->init();
}

FormCheckBox::FormCheckBox( const QRectF & rect, Page * page,
	FormObject::ObjectType type, QGraphicsItem * parent )
	:	QGraphicsObject( parent )
	,	FormObject( type, page )
	,	d( new FormCheckBoxPrivate( this, rect ) )
{
	d->init();
}

FormCheckBox::~FormCheckBox() = default;

void
FormCheckBox::paint( QPainter * painter, const QStyleOptionGraphicsItem * option,
	QWidget * widget )
{
	Q_UNUSED( option )
	Q_UNUSED( widget )

	painter->setClipRect( boundingRect() );

	draw( painter,
		objectPen(),
		objectBrush(),
		d->m_font,
		QRectF( 0.0, 0.0, d->m_rect.width(), d->m_rect.height() ),
		d->m_width,
		d->m_checked,
		d->m_text,
		boundingRect() );

	if( isSelected() && !group() )
		d->m_handles->show();
	else
		d->m_handles->hide();
}

void
FormCheckBox::draw( QPainter * painter, const QPen & pen, const QBrush & brush, const QFont & font,
	const QRectF & rect, qreal width, bool isChecked, const QString & text,
	const QRectF & boundingRect, int dpi )
{
	Q_UNUSED( width )

	painter->setPen( pen );
	painter->setBrush( brush );

	QRectF r = rect;
	r.setWidth( boxHeight( dpi ) );

	if( r.height() > boxHeight( dpi ) )
	{
		r.setTopLeft( QPointF( r.topLeft().x() + pen.widthF(),
			r.topLeft().y() + ( r.height() - boxHeight( dpi ) ) / c_halfDivider ) );
		r.setHeight( boxHeight( dpi ) );
	}

	painter->drawRect( r );

	const qreal o = r.height() / 5.0;

	if( isChecked )
	{
		painter->drawLine( QLineF( r.topLeft().x() + o,
			r.topLeft().y() + boxHeight( dpi ) / c_halfDivider,
			r.topLeft().x() + boxHeight( dpi ) / c_halfDivider,
			r.bottomLeft().y() - o ) );

		painter->drawLine( QLineF(
			r.topLeft().x() + boxHeight( dpi ) / c_halfDivider,
			r.bottomLeft().y() - o,
			r.topRight().x() - o,
			r.topRight().y() + o ) );
	}

	painter->setFont( font );

	r = boundingRect;
	r.moveLeft( r.x() + boxHeight( dpi ) + o );

	painter->drawText( r, Qt::AlignLeft | Qt::AlignVCenter, text );
}

qreal
FormCheckBox::boxHeight( int dpi )
{
	if( !dpi )
		return MmPx::instance().fromMmY( 3.5 );

	return MmPx::instance().fromMm( 3.5, dpi );
}

void
FormCheckBox::setObjectPen( const QPen & p, bool pushUndoCommand )
{
	FormObject::setObjectPen( p, pushUndoCommand );

	update();
}

void
FormCheckBox::setObjectBrush( const QBrush & b, bool pushUndoCommand )
{
	FormObject::setObjectBrush( b, pushUndoCommand );

	update();
}

Cfg::CheckBox
FormCheckBox::cfg() const
{
	Cfg::CheckBox c;

	c.set_width( MmPx::instance().toMmX( d->m_width ) );

	c.set_isChecked( d->m_checked );
	c.set_objectId( objectId() );
	c.set_pen( Cfg::pen( objectPen() ) );
	c.set_brush( Cfg::brush( objectBrush() ) );

	Cfg::Point p;
	p.set_x( MmPx::instance().toMmX( pos().x() ) );
	p.set_y( MmPx::instance().toMmY( pos().y() ) );

	c.set_pos( p );

	Cfg::Size s;
	s.set_width( MmPx::instance().toMmX( d->m_rect.width() ) );
	s.set_height( MmPx::instance().toMmY( d->m_rect.height() ) );

	c.set_size( s );

	c.set_text( text() );

	c.set_z( zValue() );

	return c;
}

void
FormCheckBox::setCfg( const Cfg::CheckBox & c )
{
	d->m_checked = c.isChecked();

	setObjectId( c.objectId() );
	setObjectPen( Cfg::fromPen( c.pen() ), false );
	setObjectBrush( Cfg::fromBrush( c.brush() ), false );

	d->setRect( QRectF( MmPx::instance().fromMmX( c.pos().x() ),
		MmPx::instance().fromMmY( c.pos().y() ),
		MmPx::instance().fromMmX( c.width() ),
		MmPx::instance().fromMmY( c.size().height() ) ) );

	setText( c.text() );

	setZValue( c.z() );

	update();
}

Cfg::TextStyle
FormCheckBox::text() const
{
	Cfg::TextStyle textStyle = Cfg::textStyleFromFont( d->m_font );
	textStyle.style().push_back( Cfg::c_left );
	textStyle.set_text( d->m_text );

	return textStyle;
}

void
FormCheckBox::setText( const Cfg::TextStyle & c )
{
	if( std::find( c.style().cbegin(), c.style().cend(),
		Cfg::c_normalStyle ) != c.style().cend() )
	{
		d->m_font.setWeight( QFont::Normal );
		d->m_font.setItalic( false );
		d->m_font.setUnderline( false );
	}
	else
	{
		if( std::find( c.style().cbegin(), c.style().cend(),
			Cfg::c_boldStyle ) != c.style().cend() )
				d->m_font.setWeight( QFont::Bold );
		else
			d->m_font.setWeight( QFont::Normal );

		if( std::find( c.style().cbegin(), c.style().cend(),
			Cfg::c_italicStyle ) != c.style().cend() )
				d->m_font.setItalic( true );
		else
			d->m_font.setItalic( false );

		if( std::find( c.style().cbegin(), c.style().cend(),
			Cfg::c_underlineStyle ) != c.style().cend() )
				d->m_font.setUnderline( true );
		else
			d->m_font.setUnderline( false );
	}

	d->m_font.setPixelSize( MmPx::instance().fromPtY( c.fontSize() ) );

	d->m_text = c.text();

	if( d->m_props )
	{
		d->disconnectProperties();
		d->m_textProps->ui()->m_text->setText( d->m_text );
		d->m_textStyleProps->ui()->m_size->setValue( qRound( c.fontSize() ) );
		d->m_textStyleProps->ui()->m_bold->setChecked( d->m_font.weight() == QFont::Bold );
		d->m_textStyleProps->ui()->m_italic->setChecked( d->m_font.italic() );
		d->m_textStyleProps->ui()->m_underline->setChecked( d->m_font.underline() );
		d->connectProperties();
	}

	if( scene() )
		scene()->update();
}

QRectF
FormCheckBox::boundingRect() const
{
	const QFontMetrics fm( d->m_font );

	const qreal dy = ( fm.height() > d->m_rect.height() ?
		( fm.height() - d->m_rect.height() ) / 2.0 : 0.0 );

	return { 0.0, -dy, d->m_width, d->m_rect.height() + dy * c_halfDivider };
}

void
FormCheckBox::setPosition( const QPointF & pos, bool pushUndoCommand )
{
	FormObject::setPosition( pos, pushUndoCommand );

	QRectF r = boundingRect();
	r.moveTopLeft( pos );

	if( d->m_props )
	{
		d->disconnectProperties();
		d->m_objProps->ui()->m_x->setValue( pos.x() );
		d->m_objProps->ui()->m_y->setValue( pos.y() );
		d->connectProperties();
	}


	d->setRect( r );
}

QPointF
FormCheckBox::position() const
{
	return pos();
}

QRectF
FormCheckBox::rectangle() const
{
	QRectF r = boundingRect();
	r.moveTopLeft( position() );

	return r;
}

void
FormCheckBox::setRectangle( const QRectF & rect, bool pushUndoCommand )
{
	FormObject::setRectangle( rect, pushUndoCommand );

	resize( rect );

	if( d->m_props )
	{
		d->disconnectProperties();
		d->m_objProps->ui()->m_width->setValue( rect.width() );
		d->m_objProps->ui()->m_height->setValue( rect.height() );
		d->connectProperties();
	}

	scene()->update();
}

void
FormCheckBox::resize( const QRectF & rect )
{
	setPos( rect.topLeft() );

	d->m_width = rect.width();

	const QFontMetrics fm( d->m_font );

	const qreal dy = ( fm.height() > rect.height() ?
		fm.height() - rect.height() : 0.0 );

	d->m_rect.setHeight( rect.height() + dy );

	QRectF r = boundingRect();
	r.moveTopLeft( rect.topLeft() );

	d->m_handles->setRect( r );

	update();

	page()->update();
}

void
FormCheckBox::moveResizable( const QPointF & delta )
{
	moveBy( delta.x(), delta.y() );
}

QSizeF
FormCheckBox::defaultSize() const
{
	return { MmPx::instance().fromMmX( 15.0 ), boxHeight() };
}

QSizeF
FormCheckBox::minimumSize() const
{
	return QSizeF( boxHeight(), boxHeight() );
}

QWidget *
FormCheckBox::properties( QWidget * parent )
{
	d->m_props = new QWidget( parent );
	d->m_objProps = new ObjectProperties( this, d->m_props );
	d->m_textProps = new TextProperties( d->m_props );
	d->m_textStyleProps = new TextStyleProperties( d->m_props );
	d->m_checkProps = new CheckBoxProperties( d->m_props );

	QVBoxLayout * l = new QVBoxLayout( d->m_props );
	l->setContentsMargins( 0, 0, 0, 0 );
	l->setSpacing( 0 );
	l->addWidget( d->m_objProps );
	l->addWidget( d->m_textProps );
	l->addWidget( d->m_textStyleProps );
	l->addWidget( d->m_checkProps );
	l->addSpacerItem( new QSpacerItem( 0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding ) );

	d->m_objProps->ui()->m_x->setValue( pos().x() );
	d->m_objProps->ui()->m_y->setValue( pos().y() );
	d->m_objProps->ui()->m_width->setValue( d->m_rect.width() );
	d->m_objProps->ui()->m_height->setValue( d->m_rect.height() );

	d->m_textProps->ui()->m_text->setText( d->m_text );
	d->m_textStyleProps->ui()->m_size->setValue( qRound( MmPx::instance().toPtY( d->m_font.pixelSize() ) ) );
	d->m_textStyleProps->ui()->m_bold->setChecked( d->m_font.weight() == QFont::Bold );
	d->m_textStyleProps->ui()->m_italic->setChecked( d->m_font.italic() );
	d->m_textStyleProps->ui()->m_underline->setChecked( d->m_font.underline() );

	d->m_checkProps->ui()->m_checked->setChecked( d->m_checked );

	d->m_objProps->ui()->m_width->setMinimum( FormCheckBox::boxHeight() );
	d->m_objProps->ui()->m_height->setMinimum( FormCheckBox::boxHeight() );

	d->m_checkProps->ui()->m_checkGroup->setTitle( typeName() );

	d->connectProperties();

	return d->m_props.data();
}

void
FormCheckBox::updatePropertiesValues()
{
	if( d->m_props )
	{
		d->disconnectProperties();
		d->m_objProps->ui()->m_x->setValue( position().x() );
		d->m_objProps->ui()->m_y->setValue( position().y() );
		d->m_objProps->ui()->m_width->setValue( rectangle().width() );
		d->m_objProps->ui()->m_height->setValue( rectangle().height() );
		d->connectProperties();
	}
}

QString
FormCheckBox::typeName() const
{
	return tr( "Check Box" );
}

bool
FormCheckBox::isChecked() const
{
	return d->m_checked;
}

void
FormCheckBox::setChecked( bool on )
{
	d->m_checked = on;

	if( d->m_props )
	{
		d->disconnectProperties();
		d->m_checkProps->ui()->m_checked->setChecked( d->m_checked );
		d->connectProperties();
	}

	update();
}

FormObject *
FormCheckBox::clone() const
{
	auto * o = new FormCheckBox( rectangle(), page(), parentItem() );

	o->setCfg( cfg() );

	o->setObjectId( page()->nextId() );

	return o;
}

} /* namespace Core */

} /* namespace Prototyper */
