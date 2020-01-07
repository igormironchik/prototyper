
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
#include "form_spinbox.hpp"
#include "utils.hpp"
#include "page.hpp"
#include "form_spinbox_properties.hpp"
#include "form_undo_commands.hpp"
#include "form_actions.hpp"
#include "ui_form_spinbox_properties.h"

// Qt include.
#include <QPainter>
#include <QPainterPath>
#include <QApplication>
#include <QGraphicsSceneContextMenuEvent>
#include <QMenu>
#include <QAction>
#include <QUndoStack>
#include <QGraphicsScene>


namespace Prototyper {

namespace Core {

//
// FormSpinBoxPrivate
//

class FormSpinBoxPrivate {
public:
	FormSpinBoxPrivate( FormSpinBox * parent, const QRectF & rect )
		:	q( parent )
		,	m_rect( QRectF( rect.x(), rect.y(), rect.width(), 25.0 ) )
		,	m_proxy( 0 )
		,	m_text( FormSpinBox::tr( "1" ) )
		,	m_properties( nullptr )
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
	FormSpinBox * q;
	//! Rect.
	QRectF m_rect;
	//! Resizable proxy.
	QScopedPointer< FormResizableProxy > m_proxy;
	//! Font.
	QFont m_font;
	//! Text.
	QString m_text;
	//! Properties.
	QPointer< SpinBoxProperties > m_properties;
}; // class FormSpinBoxPrivate

void
FormSpinBoxPrivate::init()
{
	m_proxy.reset( new FormResizableProxy( q, q->parentItem(), q->page() ) );

	setRect( m_rect );

	m_proxy->setMinSize( q->defaultSize() );

	m_font = QApplication::font();

	m_font.setPixelSize( MmPx::instance().fromPtY( 10.0 ) );

	q->setObjectPen( QPen( FormAction::instance()->strokeColor() ),
		false );

	q->setObjectBrush( Qt::transparent );
}

void
FormSpinBoxPrivate::setRect( const QRectF & rect )
{
	m_rect = rect;

	q->setPos( m_rect.topLeft() );

	m_proxy->setRect( m_rect );

	m_rect.moveTopLeft( QPointF( 0.0, 0.0 ) );
}

void
FormSpinBoxPrivate::connectProperties()
{
	if( m_properties )
	{
		FormSpinBox::connect( m_properties->ui()->m_x,
			QOverload< int >::of( &QSpinBox::valueChanged ),
			[this]( int v ) {
				q->setPosition( QPointF( v, q->position().y() ) );
				q->page()->emitChanged();
			} );

		FormSpinBox::connect( m_properties->ui()->m_y,
			QOverload< int >::of( &QSpinBox::valueChanged ),
			[this]( int v ) {
				q->setPosition( QPointF( q->position().x(), v ) );
				q->page()->emitChanged();
			} );

		FormSpinBox::connect( m_properties->ui()->m_width,
			QOverload< int >::of( &QSpinBox::valueChanged ),
			[this]( int v ) {
				QRectF r = m_rect;
				r.setWidth( v );
				r.moveTopLeft( q->position() );
				q->setRectangle( r, true );
				q->page()->emitChanged();
			} );

		FormSpinBox::connect( m_properties->ui()->m_height,
			QOverload< int >::of( &QSpinBox::valueChanged ),
			[this]( int v ) {
				QRectF r = m_rect;
				r.setHeight( v );
				r.moveTopLeft( q->position() );
				q->setRectangle( r, true );
				q->page()->emitChanged();
			} );

		FormSpinBox::connect( m_properties->ui()->m_value,
			QOverload< int >::of( &QSpinBox::valueChanged ),
			[this]( int v ) {
				const auto oldText = q->text();
				m_text = QString::number( v );
				q->page()->emitChanged();

				q->page()->undoStack()->push( new UndoChangeTextWithOpts( q->page(),
					q->objectId(), oldText, q->text() ) );

				q->update();
			} );

		FormSpinBox::connect( m_properties->ui()->m_size,
			QOverload< int >::of( &QSpinBox::valueChanged ),
			[this]( int v ) {
				const auto oldText = q->text();
				m_font.setPixelSize( MmPx::instance().fromPtY( v ) );
				q->page()->emitChanged();

				q->page()->undoStack()->push( new UndoChangeTextWithOpts( q->page(),
					q->objectId(), oldText, q->text() ) );

				q->update();
			} );

		FormSpinBox::connect( m_properties->ui()->m_bold,
			&QCheckBox::stateChanged,
			[this]( int v ) {
				const auto oldText = q->text();
				m_font.setWeight( ( v == Qt::Checked ? QFont::Bold : QFont::Normal ) );
				q->page()->emitChanged();

				q->page()->undoStack()->push( new UndoChangeTextWithOpts( q->page(),
					q->objectId(), oldText, q->text() ) );

				q->update();
			} );

		FormSpinBox::connect( m_properties->ui()->m_italic,
			&QCheckBox::stateChanged,
			[this]( int v ) {
				const auto oldText = q->text();
				m_font.setItalic( ( v == Qt::Checked ? true : false ) );
				q->page()->emitChanged();

				q->page()->undoStack()->push( new UndoChangeTextWithOpts( q->page(),
					q->objectId(), oldText, q->text() ) );

				q->update();
			} );

		FormSpinBox::connect( m_properties->ui()->m_underline,
			&QCheckBox::stateChanged,
			[this]( int v ) {
				const auto oldText = q->text();
				m_font.setUnderline( ( v == Qt::Checked ? true : false ) );
				q->page()->emitChanged();

				q->page()->undoStack()->push( new UndoChangeTextWithOpts( q->page(),
					q->objectId(), oldText, q->text() ) );

				q->update();
			} );
	}
}

void
FormSpinBoxPrivate::disconnectProperties()
{
	if( m_properties )
	{
		FormSpinBox::disconnect( m_properties->ui()->m_x,
			QOverload< int >::of( &QSpinBox::valueChanged ), 0, 0 );

		FormSpinBox::disconnect( m_properties->ui()->m_y,
			QOverload< int >::of( &QSpinBox::valueChanged ), 0, 0 );

		FormSpinBox::disconnect( m_properties->ui()->m_width,
			QOverload< int >::of( &QSpinBox::valueChanged ), 0, 0 );

		FormSpinBox::disconnect( m_properties->ui()->m_height,
			QOverload< int >::of( &QSpinBox::valueChanged ), 0, 0 );

		FormSpinBox::disconnect( m_properties->ui()->m_value,
			QOverload< int >::of( &QSpinBox::valueChanged ), 0, 0 );

		FormSpinBox::disconnect( m_properties->ui()->m_size,
			QOverload< int >::of( &QSpinBox::valueChanged ), 0, 0 );

		FormSpinBox::disconnect( m_properties->ui()->m_bold,
			&QCheckBox::stateChanged, 0, 0 );

		FormSpinBox::disconnect( m_properties->ui()->m_italic,
			&QCheckBox::stateChanged, 0, 0 );

		FormSpinBox::disconnect( m_properties->ui()->m_underline,
			&QCheckBox::stateChanged, 0, 0 );
	}
}


//
// FormSpinBox
//

FormSpinBox::FormSpinBox( const QRectF & rect, Page * form,
	QGraphicsItem * parent )
	:	QGraphicsObject( parent )
	,	FormObject( FormObject::SpinBoxType, form )
	,	d( new FormSpinBoxPrivate( this, rect ) )
{
	d->init();
}

FormSpinBox::~FormSpinBox()
{
}

void
FormSpinBox::updatePropertiesValues()
{
	if( d->m_properties )
	{
		d->disconnectProperties();
		d->m_properties->ui()->m_x->setValue( position().x() );
		d->m_properties->ui()->m_y->setValue( position().y() );
		d->m_properties->ui()->m_width->setValue( rectangle().width() );
		d->m_properties->ui()->m_height->setValue( rectangle().height() );
		d->connectProperties();
	}
}

void
FormSpinBox::paint( QPainter * painter, const QStyleOptionGraphicsItem * option,
	QWidget * widget )
{
	Q_UNUSED( widget )
	Q_UNUSED( option )

	draw( painter, d->m_rect, objectPen(), d->m_font, d->m_text );

	if( isSelected() && !group() )
		d->m_proxy->show();
	else
		d->m_proxy->hide();
}

void
FormSpinBox::draw( QPainter * painter, const QRectF & rect,
	const QPen & pen, const QFont & font, const QString & text, int dpi )
{
	painter->setPen( pen );

	QRectF r = rect;

	if( r.height() > boxHeight( dpi ) )
	{
		r.setHeight( boxHeight( dpi ) );
		r.moveTopLeft( QPointF( rect.topLeft().x(), rect.topLeft().y() +
			( rect.height() - boxHeight( dpi ) ) / 2.0 ) );
	}

	const qreal ro = r.height() / 10.0;

	painter->drawRoundedRect( r, ro, ro );

	const qreal h = r.height();
	const qreal w = h * 0.75;
	const qreal o = w / 5.0;
	const qreal leftX = r.x() + r.width() - w;

	painter->drawLine( QLineF( leftX, r.y(), leftX, r.y() + h ) );

	QPainterPath top;
	top.moveTo( leftX + o, r.y() + r.height() / 2.0 - o / 2.0 );
	top.lineTo( leftX + w - o, r.y() + r.height() / 2.0 - o / 2.0 );
	top.lineTo( leftX + w / 2.0, r.y() + o );
	top.lineTo( leftX + o, r.y() + r.height() / 2.0 - o / 2.0 );

	painter->setBrush( QBrush( pen.color() ) );

	painter->drawPath( top );

	QPainterPath bottom;
	bottom.moveTo( leftX + o, r.y() + r.height() / 2.0 + o / 2.0 );
	bottom.lineTo( leftX + w - o, r.y() + r.height() / 2.0 + o / 2.0 );
	bottom.lineTo( leftX + w / 2.0, r.y() + h - o );
	bottom.lineTo( leftX + o, r.y() + r.height() / 2.0 + o / 2.0 );

	painter->drawPath( bottom );

	QRectF textR = r;
	textR.setRight( leftX - o );

	painter->setFont( font );

	painter->drawText( textR, Qt::AlignRight | Qt::AlignVCenter,
		text );
}

qreal
FormSpinBox::boxHeight( int dpi )
{
	if( !dpi )
		return MmPx::instance().fromMmY( 4.0 );
	else
		return MmPx::instance().fromMm( 4.0, dpi );
}

void
FormSpinBox::setObjectPen( const QPen & p, bool pushUndoCommand )
{
	FormObject::setObjectPen( p, pushUndoCommand );

	update();
}

Cfg::SpinBox
FormSpinBox::cfg() const
{
	Cfg::SpinBox c;

	c.set_objectId( objectId() );

	Cfg::Point p;
	p.set_x( MmPx::instance().toMmX( pos().x() ) );
	p.set_y( MmPx::instance().toMmY( pos().y() ) );

	c.set_pos( p );

	Cfg::Size s;
	s.set_width( MmPx::instance().toMmX( d->m_rect.width() ) );
	s.set_height( MmPx::instance().toMmY( d->m_rect.height() ) );

	c.set_size( s );

	c.set_pen( Cfg::pen( objectPen() ) );

	c.set_text( text() );

	c.set_z( zValue() );

	return c;
}

void
FormSpinBox::setCfg( const Cfg::SpinBox & c )
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
FormSpinBox::text() const
{
	Cfg::TextStyle textStyle = Cfg::textStyleFromFont( d->m_font );
	textStyle.style().push_back( Cfg::c_right );
	textStyle.set_text( d->m_text );

	return textStyle;
}

void
FormSpinBox::setText( const Cfg::TextStyle & c )
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

	if( d->m_properties )
	{
		d->disconnectProperties();
		d->m_properties->ui()->m_value->setValue( d->m_text.toInt() );
		d->m_properties->ui()->m_size->setValue( qRound( c.fontSize() ) );
		d->m_properties->ui()->m_bold->setChecked( d->m_font.weight() == QFont::Bold );
		d->m_properties->ui()->m_italic->setChecked( d->m_font.italic() );
		d->m_properties->ui()->m_underline->setChecked( d->m_font.underline() );
		d->connectProperties();
	}

	update();
}

QRectF
FormSpinBox::boundingRect() const
{
	return d->m_rect;
}

void
FormSpinBox::setPosition( const QPointF & pos, bool pushUndoCommand )
{
	if( pushUndoCommand )
		page()->undoStack()->push( new UndoMove( page(), objectId(), pos - position() ) );

	QRectF r = boundingRect();
	r.moveTopLeft( pos );

	if( d->m_properties )
	{
		d->disconnectProperties();
		d->m_properties->ui()->m_x->setValue( pos.x() );
		d->m_properties->ui()->m_y->setValue( pos.y() );
		d->connectProperties();
	}

	d->setRect( r );
}

QPointF
FormSpinBox::position() const
{
	return pos();
}

QRectF
FormSpinBox::rectangle() const
{
	QRectF r = boundingRect();
	r.moveTopLeft( position() );

	return r;
}

void
FormSpinBox::setRectangle( const QRectF & rect,
	bool pushUndoCommand )
{
	if( pushUndoCommand )
		page()->undoStack()->push( new UndoResize( page(), objectId(),
			rectangle(), rect ) );

	if( d->m_properties )
	{
		d->disconnectProperties();
		d->m_properties->ui()->m_width->setValue( rect.width() );
		d->m_properties->ui()->m_height->setValue( rect.height() );
		d->connectProperties();
	}

	resize( rect );

	scene()->update();
}

void
FormSpinBox::resize( const QRectF & rect )
{
	d->setRect( rect );

	page()->update();
}

void
FormSpinBox::moveResizable( const QPointF & delta )
{
	moveBy( delta.x(), delta.y() );
}

QSizeF
FormSpinBox::defaultSize() const
{
	return QSizeF( MmPx::instance().fromMmX( 15.0 ), boxHeight() );
}

QWidget *
FormSpinBox::properties( QWidget * parent )
{
	d->m_properties = new SpinBoxProperties( parent );

	d->m_properties->ui()->m_x->setValue( pos().x() );
	d->m_properties->ui()->m_y->setValue( pos().y() );
	d->m_properties->ui()->m_width->setValue( d->m_rect.width() );
	d->m_properties->ui()->m_height->setValue( d->m_rect.height() );
	d->m_properties->ui()->m_value->setValue( d->m_text.toInt() );
	d->m_properties->ui()->m_size->setValue( qRound( MmPx::instance().toPtY( d->m_font.pixelSize() ) ) );
	d->m_properties->ui()->m_bold->setChecked( d->m_font.weight() == QFont::Bold );
	d->m_properties->ui()->m_italic->setChecked( d->m_font.italic() );
	d->m_properties->ui()->m_underline->setChecked( d->m_font.underline() );

	d->m_properties->ui()->m_width->setMinimum( defaultSize().width() );
	d->m_properties->ui()->m_height->setMinimum( defaultSize().height() );

	d->connectProperties();

	return d->m_properties.data();
}

} /* namespace Core */

} /* namespace Prototyper */
