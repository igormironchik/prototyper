
/*
	SPDX-FileCopyrightText: 2016-2024 Igor Mironchik <igor.mironchik@gmail.com>
	SPDX-License-Identifier: GPL-3.0-or-later
*/

// Prototyper include.
#include "button.hpp"
#include "utils.hpp"
#include "page.hpp"
#include "undo_commands.hpp"
#include "actions.hpp"
#include "../constants.hpp"
#include "properties/object_properties.hpp"
#include "properties/text_properties.hpp"
#include "properties/text_style_properties.hpp"
#include "properties/ui_object_properties.h"

// Qt include.
#include <QFontMetrics>
#include <QFont>
#include <QPainter>
#include <QApplication>
#include <QGraphicsSceneContextMenuEvent>
#include <QMenu>
#include <QAction>
#include <QUndoStack>
#include <QGraphicsScene>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QSpacerItem>


namespace Prototyper {

namespace Core {

//
// FormButtonPrivate
//

class FormButtonPrivate {
public:
	FormButtonPrivate( FormButton * parent, const QRectF & rect )
		:	q( parent )
		,	m_rect( rect )
		,	m_proxy( nullptr )
		,	m_text( QLatin1String( "OK" ) )
		,	m_props( nullptr )
	{
	}

	//! Init.
	void init();
	//! Set rect.
	void setRect( const QRectF & rect );
	//! Connect properties signals.
	void connectProperties();
	//! Disconnect properties signals.
	void disconnectProperties();

	//! Parent.
	FormButton * q;
	//! Rect.
	QRectF m_rect;
	//! Resizable proxy.
	std::unique_ptr< FormResizableProxy > m_proxy;
	//! Text.
	QString m_text;
	//! Font.
	QFont m_font;
	//! Properties.
	QPointer< QWidget > m_props;
	//! Object properties.
	QPointer< ObjectProperties > m_objProps;
	//! Text properties.
	QPointer< TextProperties > m_textProps;
	//! Text style properties.
	QPointer< TextStyleProperties > m_textStyleProps;
}; // class FormButtonPrivate

void
FormButtonPrivate::init()
{
	m_proxy.reset( new FormResizableProxy( q, q->parentItem(), q->page() ) );

	setRect( m_rect );

	m_font = QApplication::font();

	m_font.setPixelSize( MmPx::instance().fromPtY( c_defaultFontSize ) );

	QFontMetrics m( m_font );
	m_proxy->setMinSize(
		QSizeF( m.boundingRect( QLatin1Char( 'a' ) ).size() ) );

	q->setObjectPen( QPen( PageAction::instance()->strokeColor() ),
		false );

	q->setObjectBrush( QBrush( PageAction::instance()->fillColor() ), false );
}

void
FormButtonPrivate::setRect( const QRectF & rect )
{
	m_rect = rect;

	q->setPos( m_rect.topLeft() );

	m_proxy->setRect( m_rect );

	m_rect.moveTopLeft( QPointF( 0.0, 0.0 ) );
}

void
FormButtonPrivate::connectProperties()
{
	if( m_props )
	{
		m_objProps->connectProperties();
		m_textProps->connectProperties( this );
		m_textStyleProps->connectProperties( this );
	}
}

void
FormButtonPrivate::disconnectProperties()
{
	if( m_props )
	{
		m_objProps->disconnectProperties();
		m_textProps->disconnectProperties();
		m_textStyleProps->disconnectProperties();
	}
}


//
// FormButton
//

FormButton::FormButton( const QRectF & rect, Page * page,
	QGraphicsItem * parent )
	:	QGraphicsObject( parent )
	,	FormObject( FormObject::ButtonType, page )
	,	d( new FormButtonPrivate( this, rect ) )
{
	d->init();
}

FormButton::~FormButton() = default;

void
FormButton::paint( QPainter * painter, const QStyleOptionGraphicsItem * option,
	QWidget * widget )
{
	Q_UNUSED( widget )
	Q_UNUSED( option )

	painter->setPen( objectPen() );
	painter->setFont( d->m_font );
	painter->setBrush( objectBrush() );

	painter->drawRect( d->m_rect );
	painter->drawText( d->m_rect, Qt::AlignCenter, d->m_text );

	if( isSelected() && !group() )
		d->m_proxy->show();
	else
		d->m_proxy->hide();
}

void
FormButton::setObjectPen( const QPen & p, bool pushUndoCommand )
{
	FormObject::setObjectPen( p, pushUndoCommand );

	update();
}

void
FormButton::setObjectBrush( const QBrush & b, bool pushUndoCommand )
{
	FormObject::setObjectBrush( b, pushUndoCommand );

	update();
}

Cfg::Button
FormButton::cfg() const
{
	Cfg::Button c;

	c.set_objectId( objectId() );

	Cfg::Point p;
	p.set_x( MmPx::instance().toMmX( pos().x() ) );
	p.set_y( MmPx::instance().toMmY( pos().y() ) );

	c.set_pos( p );

	Cfg::Size s;
	s.set_width( MmPx::instance().toMmX( d->m_rect.width() ) );
	s.set_height( MmPx::instance().toMmY( d->m_rect.height() ) );

	c.set_size( s );

	c.set_text( text() );

	c.set_pen( Cfg::pen( objectPen() ) );
	c.set_brush( Cfg::brush( objectBrush() ) );

	c.set_z( zValue() );

	return c;
}

void
FormButton::setCfg( const Cfg::Button & c )
{
	setObjectId( c.objectId() );
	setObjectPen( Cfg::fromPen( c.pen() ), false );
	setObjectBrush( Cfg::fromBrush( c.brush() ), false );
	d->setRect( QRectF( MmPx::instance().fromMmX( c.pos().x() ),
		MmPx::instance().fromMmY( c.pos().y() ),
		MmPx::instance().fromMmX( c.size().width() ),
		MmPx::instance().fromMmY( c.size().height() ) ) );

	setText( c.text() );

	setZValue( c.z() );

	update();
}

Cfg::TextStyle
FormButton::text() const
{
	Cfg::TextStyle textStyle = Cfg::textStyleFromFont( d->m_font );
	textStyle.style().push_back( Cfg::c_center );
	textStyle.set_text( d->m_text );

	return textStyle;
}

void
FormButton::setText( const Cfg::TextStyle & c )
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

	update();
}

QRectF
FormButton::boundingRect() const
{
	return d->m_rect;
}

void
FormButton::setPosition( const QPointF & pos, bool pushUndoCommand )
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
FormButton::position() const
{
	return pos();
}

QRectF
FormButton::rectangle() const
{
	QRectF r = boundingRect();
	r.moveTopLeft( position() );

	return r;
}

void
FormButton::setRectangle( const QRectF & rect, bool pushUndoCommand )
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
FormButton::resize( const QRectF & rect )
{
	d->setRect( rect );

	page()->update();
}

void
FormButton::moveResizable( const QPointF & delta )
{
	moveBy( delta.x(), delta.y() );
}

QSizeF
FormButton::defaultSize() const
{
	return QSizeF( MmPx::instance().fromMmX( 10.0 ), MmPx::instance().fromMmY( 5.0 ) );
}

QWidget *
FormButton::properties( QWidget * parent )
{
	d->m_props = new QWidget( parent );
	d->m_objProps = new ObjectProperties( this, d->m_props );
	d->m_textProps = new TextProperties( d->m_props );
	d->m_textStyleProps = new TextStyleProperties( d->m_props );

	QVBoxLayout * l = new QVBoxLayout( d->m_props );
	l->setContentsMargins( 0, 0, 0, 0 );
	l->setSpacing( 0 );
	l->addWidget( d->m_objProps );
	l->addWidget( d->m_textProps );
	l->addWidget( d->m_textStyleProps );
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

	d->m_objProps->ui()->m_width->setMinimum( defaultSize().width() );
	d->m_objProps->ui()->m_height->setMinimum( defaultSize().height() );

	d->connectProperties();

	return d->m_props.data();
}

void
FormButton::updatePropertiesValues()
{
	if( d->m_props )
	{
		d->disconnectProperties();
		d->m_objProps->ui()->m_x->setValue( qRound( position().x() ) );
		d->m_objProps->ui()->m_y->setValue( qRound( position().y() ) );
		d->m_objProps->ui()->m_width->setValue( qRound( rectangle().width() ) );
		d->m_objProps->ui()->m_height->setValue( qRound( rectangle().height() ) );
		d->connectProperties();
	}
}

FormObject *
FormButton::clone() const
{
	auto * o = new FormButton( rectangle(), page(), parentItem() );

	o->setCfg( cfg() );

	o->setObjectId( page()->nextId() );

	return o;
}

} /* namespace Core */

} /* namespace Prototyper */
