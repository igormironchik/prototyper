
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
#include "form_button.hpp"
#include "utils.hpp"
#include "form_button_properties.hpp"
#include "page.hpp"
#include "form_undo_commands.hpp"
#include "form_actions.hpp"
#include "ui_form_button_properties.h"
#include "constants.hpp"

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
	QScopedPointer< FormResizableProxy > m_proxy;
	//! Text.
	QString m_text;
	//! Font.
	QFont m_font;
	//! Properties.
	QPointer< ButtonProperties > m_props;
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

	q->setObjectBrush( Qt::transparent );
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
		FormButton::connect( m_props->ui()->m_x,
			QOverload< int >::of( &QSpinBox::valueChanged ),
			[this]( int v ) {
				q->setPosition( QPointF( v, q->position().y() ) );
				q->page()->emitChanged();
			} );

		FormButton::connect( m_props->ui()->m_y,
			QOverload< int >::of( &QSpinBox::valueChanged ),
			[this]( int v ) {
				q->setPosition( QPointF( q->position().x(), v ) );
				q->page()->emitChanged();
			} );

		FormButton::connect( m_props->ui()->m_width,
			QOverload< int >::of( &QSpinBox::valueChanged ),
			[this]( int v ) {
				QRectF r = m_rect;
				r.setWidth( v );
				r.moveTopLeft( q->position() );
				q->setRectangle( r, true );
				q->page()->emitChanged();
			} );

		FormButton::connect( m_props->ui()->m_height,
			QOverload< int >::of( &QSpinBox::valueChanged ),
			[this]( int v ) {
				QRectF r = m_rect;
				r.setHeight( v );
				r.moveTopLeft( q->position() );
				q->setRectangle( r, true );
				q->page()->emitChanged();
			} );

		FormButton::connect( m_props->ui()->m_text,
			&QLineEdit::textChanged,
			[this]( const QString & t ) {
				const auto oldText = q->text();
				m_text = t;
				q->page()->emitChanged();

				q->page()->undoStack()->push( new UndoChangeTextWithOpts( q->page(),
					q->objectId(), oldText, q->text() ) );

				q->update();
			} );

		FormButton::connect( m_props->ui()->m_size,
			QOverload< int >::of( &QSpinBox::valueChanged ),
			[this]( int v ) {
				const auto oldText = q->text();
				m_font.setPixelSize( MmPx::instance().fromPtY( v ) );
				q->page()->emitChanged();

				q->page()->undoStack()->push( new UndoChangeTextWithOpts( q->page(),
					q->objectId(), oldText, q->text() ) );

				q->update();
			} );

		FormButton::connect( m_props->ui()->m_bold,
			&QCheckBox::stateChanged,
			[this]( int v ) {
				const auto oldText = q->text();
				m_font.setWeight( ( v == Qt::Checked ? QFont::Bold : QFont::Normal ) );
				q->page()->emitChanged();

				q->page()->undoStack()->push( new UndoChangeTextWithOpts( q->page(),
					q->objectId(), oldText, q->text() ) );

				q->update();
			} );

		FormButton::connect( m_props->ui()->m_italic,
			&QCheckBox::stateChanged,
			[this]( int v ) {
				const auto oldText = q->text();
				m_font.setItalic( ( v == Qt::Checked ) );
				q->page()->emitChanged();

				q->page()->undoStack()->push( new UndoChangeTextWithOpts( q->page(),
					q->objectId(), oldText, q->text() ) );

				q->update();
			} );

		FormButton::connect( m_props->ui()->m_underline,
			&QCheckBox::stateChanged,
			[this]( int v ) {
				const auto oldText = q->text();
				m_font.setUnderline( ( v == Qt::Checked ) );
				q->page()->emitChanged();

				q->page()->undoStack()->push( new UndoChangeTextWithOpts( q->page(),
					q->objectId(), oldText, q->text() ) );

				q->update();
			} );
	}
}

void
FormButtonPrivate::disconnectProperties()
{
	if( m_props )
	{
		FormButton::disconnect( m_props->ui()->m_x,
			QOverload< int >::of( &QSpinBox::valueChanged ), nullptr, nullptr );

		FormButton::disconnect( m_props->ui()->m_y,
			QOverload< int >::of( &QSpinBox::valueChanged ), nullptr, nullptr );

		FormButton::disconnect( m_props->ui()->m_width,
			QOverload< int >::of( &QSpinBox::valueChanged ), nullptr, nullptr );

		FormButton::disconnect( m_props->ui()->m_height,
			QOverload< int >::of( &QSpinBox::valueChanged ), nullptr, nullptr );

		FormButton::disconnect( m_props->ui()->m_text,
			&QLineEdit::textChanged, nullptr, nullptr );

		FormButton::disconnect( m_props->ui()->m_size,
			QOverload< int >::of( &QSpinBox::valueChanged ), nullptr, nullptr );

		FormButton::disconnect( m_props->ui()->m_bold,
			&QCheckBox::stateChanged, nullptr, nullptr );

		FormButton::disconnect( m_props->ui()->m_italic,
			&QCheckBox::stateChanged, nullptr, nullptr );

		FormButton::disconnect( m_props->ui()->m_underline,
			&QCheckBox::stateChanged, nullptr, nullptr );
	}
}


//
// FormButton
//

FormButton::FormButton( const QRectF & rect, Page * form,
	QGraphicsItem * parent )
	:	QGraphicsObject( parent )
	,	FormObject( FormObject::ButtonType, form )
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

	c.set_z( zValue() );

	return c;
}

void
FormButton::setCfg( const Cfg::Button & c )
{
	setObjectId( c.objectId() );
	setObjectPen( Cfg::fromPen( c.pen() ), false );
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
		d->m_props->ui()->m_text->setText( d->m_text );
		d->m_props->ui()->m_size->setValue( qRound( c.fontSize() ) );
		d->m_props->ui()->m_bold->setChecked( d->m_font.weight() == QFont::Bold );
		d->m_props->ui()->m_italic->setChecked( d->m_font.italic() );
		d->m_props->ui()->m_underline->setChecked( d->m_font.underline() );
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
		d->m_props->ui()->m_x->setValue( pos.x() );
		d->m_props->ui()->m_y->setValue( pos.y() );
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
		d->m_props->ui()->m_width->setValue( rect.width() );
		d->m_props->ui()->m_height->setValue( rect.height() );
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
	d->m_props = new ButtonProperties( parent );

	d->m_props->ui()->m_x->setValue( pos().x() );
	d->m_props->ui()->m_y->setValue( pos().y() );
	d->m_props->ui()->m_width->setValue( d->m_rect.width() );
	d->m_props->ui()->m_height->setValue( d->m_rect.height() );
	d->m_props->ui()->m_text->setText( d->m_text );
	d->m_props->ui()->m_size->setValue( qRound( MmPx::instance().toPtY( d->m_font.pixelSize() ) ) );
	d->m_props->ui()->m_bold->setChecked( d->m_font.weight() == QFont::Bold );
	d->m_props->ui()->m_italic->setChecked( d->m_font.italic() );
	d->m_props->ui()->m_underline->setChecked( d->m_font.underline() );

	d->m_props->ui()->m_width->setMinimum( defaultSize().width() );
	d->m_props->ui()->m_height->setMinimum( defaultSize().height() );

	d->connectProperties();

	return d->m_props.data();
}

void
FormButton::updatePropertiesValues()
{
	if( d->m_props )
	{
		d->disconnectProperties();
		d->m_props->ui()->m_x->setValue( position().x() );
		d->m_props->ui()->m_y->setValue( position().y() );
		d->m_props->ui()->m_width->setValue( rectangle().width() );
		d->m_props->ui()->m_height->setValue( rectangle().height() );
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
