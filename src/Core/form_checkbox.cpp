
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
#include "form_checkbox.hpp"
#include "utils.hpp"
#include "form_image_handles.hpp"
#include "form_checkbox_properties.hpp"
#include "page.hpp"
#include "form_undo_commands.hpp"

// Qt include.
#include <QPainter>
#include <QApplication>
#include <QMenu>
#include <QAction>
#include <QGraphicsSceneContextMenuEvent>
#include <QUndoStack>
#include <QGraphicsScene>


namespace Prototyper {

namespace Core {

//
// FormCheckBoxPrivate
//

FormCheckBoxPrivate::FormCheckBoxPrivate( FormCheckBox * parent,
	const QRectF & rect, qreal defaultSize )
	:	q( parent )
	,	m_rect( QRectF( rect.topLeft().x(), rect.topLeft().y(),
			rect.width(), defaultSize ) )
	,	m_checked( true )
	,	m_handles( 0 )
	,	m_text( FormCheckBox::tr( "Text" ) )
	,	m_width( rect.width() )
{
}

FormCheckBoxPrivate::~FormCheckBoxPrivate()
{
}

void
FormCheckBoxPrivate::init()
{
	m_handles.reset( new FormResizableProxy( q, q->parentItem(), q->form() ) );

	m_handles->setMinSize( q->defaultSize() );

	m_handles->hide();

	m_font = QApplication::font();

	m_font.setPointSize( 14.0 );

	setRect( m_rect );
}

void
FormCheckBoxPrivate::setRect( const QRectF & rect )
{
	const QRectF r = rect;

	m_rect = QRectF( 0.0, 0.0, r.height(), r.height() );

	m_width = r.width();

	q->setPos( r.topLeft() );

	QRectF hr = q->boundingRect();
	hr.moveTopLeft( r.topLeft() );

	m_handles->setRect( hr );
}


//
// FormCheckBox
//

FormCheckBox::FormCheckBox( const QRectF & rect, Page * form,
	QGraphicsItem * parent )
	:	QGraphicsObject( parent )
	,	FormObject( FormObject::CheckBoxType, form )
	,	d( new FormCheckBoxPrivate( this, rect ) )
{
	d->init();
}

FormCheckBox::FormCheckBox( const QRectF & rect, Page * form, qreal defaultSize,
	FormObject::ObjectType type, QGraphicsItem * parent )
	:	QGraphicsObject( parent )
	,	FormObject( type, form )
	,	d( new FormCheckBoxPrivate( this, rect, defaultSize ) )
{
	d->init();
}

FormCheckBox::~FormCheckBox()
{
}

void
FormCheckBox::paint( QPainter * painter, const QStyleOptionGraphicsItem * option,
	QWidget * widget )
{
	Q_UNUSED( option )
	Q_UNUSED( widget )

	painter->setClipRect( boundingRect() );

	draw( painter,
		objectPen(),
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
FormCheckBox::draw( QPainter * painter, const QPen & pen, const QFont & font,
	const QRectF & rect, qreal width, bool isChecked, const QString & text,
	const QRectF & boundingRect )
{
	Q_UNUSED( width )

	painter->setPen( pen );

	painter->drawRect( rect );

	if( isChecked )
	{
		painter->drawLine( QLineF( rect.topLeft().x() + 4.0,
			rect.topLeft().y() + rect.height() / 2.0,
			rect.topLeft().x() + rect.width() / 2.0,
			rect.bottomLeft().y() - 4.0 ) );

		painter->drawLine( QLineF(
			rect.topLeft().x() + rect.width() / 2.0,
			rect.bottomLeft().y() - 4.0,
			rect.topRight().x() - 4.0,
			rect.topRight().y() + 4.0 ) );
	}

	painter->setFont( font );

	QRectF r = boundingRect;
	r.moveLeft( rect.x() + rect.height() + 4.0 );

	painter->drawText( r, Qt::AlignLeft | Qt::AlignVCenter, text );
}

void
FormCheckBox::setObjectPen( const QPen & p, bool pushUndoCommand )
{
	FormObject::setObjectPen( p, pushUndoCommand );

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

	Cfg::Point p;
	p.set_x( MmPx::instance().toMmX( pos().x() ) );
	p.set_y( MmPx::instance().toMmY( pos().y() ) );

	c.set_pos( p );

	Cfg::Size s;
	s.set_width( MmPx::instance().toMmX( d->m_rect.width() ) );
	s.set_height( MmPx::instance().toMmY( d->m_rect.height() ) );

	c.set_size( s );

	c.set_text( text() );

	return c;
}

void
FormCheckBox::setCfg( const Cfg::CheckBox & c )
{
	d->m_checked = c.isChecked();

	setObjectId( c.objectId() );
	setObjectPen( Cfg::fromPen( c.pen() ), false );

	d->setRect( QRectF( MmPx::instance().fromMmX( c.pos().x() ),
		MmPx::instance().fromMmY( c.pos().y() ),
		MmPx::instance().fromMmX( c.width() ),
		MmPx::instance().fromMmY( c.size().height() ) ) );

	setText( c.text() );

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

	d->m_font.setPointSize( c.fontSize() );

	d->m_text = c.text();

	if( scene() )
		scene()->update();
}

QRectF
FormCheckBox::boundingRect() const
{
	const QFontMetrics fm( d->m_font );

	const qreal dy = ( fm.height() > d->m_rect.height() ?
		( fm.height() - d->m_rect.height() ) / 2.0 : 0.0 );

	return QRectF( 0.0, -dy, d->m_width, d->m_rect.height() + dy * 2.0 );
}

void
FormCheckBox::setPosition( const QPointF & pos, bool pushUndoCommand )
{
	if( pushUndoCommand )
		form()->undoStack()->push( new UndoMove< FormCheckBox > ( form(),
			objectId(), pos - position() ) );

	QRectF r = boundingRect();
	r.moveTopLeft( pos );

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
	Q_UNUSED( pushUndoCommand )

	resize( rect );

	scene()->update();
}

void
FormCheckBox::resize( const QRectF & rect )
{
	setPos( rect.topLeft() );

	d->m_width = rect.width();

	QRectF r = boundingRect();
	r.moveTopLeft( rect.topLeft() );

	d->m_handles->setRect( r );

	update();

	form()->update();
}

void
FormCheckBox::moveResizable( const QPointF & delta )
{
	moveBy( delta.x(), delta.y() );
}

void
FormCheckBox::contextMenuEvent( QGraphicsSceneContextMenuEvent * event )
{
	QMenu menu;

	menu.addAction( QIcon( ":/Core/img/configure.png" ),
		tr( "Properties" ), this, &FormCheckBox::properties );

	menu.exec( event->screenPos() );
}

void
FormCheckBox::properties()
{
	CheckBoxProperties dlg;

	dlg.setCfg( cfg() );

	if( dlg.exec() == QDialog::Accepted )
	{
		Cfg::CheckBox c = dlg.cfg();

		form()->undoStack()->push( new UndoChangeTextWithOpts( form(),
			objectId(), text(), c.text() ) );

		setText( c.text() );

		d->m_checked = c.isChecked();

		update();
	}
}

QSizeF
FormCheckBox::defaultSize() const
{
	return QSizeF( MmPx::instance().fromMmX( 20.0 ), MmPx::instance().fromMmY( 3.5 ) );
}

} /* namespace Core */

} /* namespace Prototyper */
