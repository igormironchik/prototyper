
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
#include "form_text.hpp"
#include "form_with_resize_and_move_handles.hpp"
#include "form_resizable.hpp"
#include "form_text_opts.hpp"
#include "utils.hpp"
#include "page.hpp"
#include "form_undo_commands.hpp"
#include "form_actions.hpp"

// Qt include.
#include <QStyleOptionGraphicsItem>
#include <QPainter>
#include <QTextDocument>
#include <QTextCursor>
#include <QFontMetrics>
#include <QFont>
#include <QTextBlockFormat>
#include <QUndoStack>
#include <QKeyEvent>
#include <QEvent>
#include <QGraphicsScene>


namespace Prototyper {

namespace Core {

//
// FormTextPrivate
//

class FormTextPrivate {
public:
	FormTextPrivate( const QRectF & rect, FormText * parent )
		:	q( parent )
		,	m_rect( rect )
		,	m_proxy( 0 )
		,	m_opts( 0 )
		,	m_isFirstPaint( true )
		,	m_showToolBar( false )
	{
	}

	//! Init.
	void init();
	//! Set rect.
	void setRect( const QRectF & rect );

	//! Parent.
	FormText * q;
	//! Rect.
	QRectF m_rect;
	//! Resizable proxy.
	QScopedPointer< FormResizableProxy > m_proxy;
	//! Text options.
	QScopedPointer< FormTextOpts > m_opts;
	//! First paint.
	bool m_isFirstPaint;
	//! Show tool bar.
	bool m_showToolBar;
}; // class FormTextPrivate

void
FormTextPrivate::init()
{
	q->enableEditing( true );

	m_proxy.reset( new FormResizableProxy( q, q->parentItem(), q->form() ) );

	m_opts.reset( new FormTextOpts( q->parentItem() ) );

	m_opts->hide();
	m_opts->setZValue( 999999999999 );

	setRect( m_rect );

	QFont f = q->font();

	f.setPixelSize( MmPx::instance().fromPtY( 10.0 ) );

	q->setFont( f );

	q->document()->setDefaultFont( f );

	QTextCursor c = q->textCursor();

	QTextCharFormat fmt = c.charFormat();
	fmt.setFont( f );

	c.setCharFormat( fmt );

	q->setTextCursor( c );

	q->setPlainText( FormText::tr( "Text" ) );

	m_proxy->setMinSize( q->boundingRect().size() );

	q->setObjectPen( QPen( FormAction::instance()->strokeColor() ),
		false );

	q->setObjectBrush( Qt::transparent );

	FormText::connect( q->document(), &QTextDocument::contentsChanged,
		q, &FormText::p_contentChanged );
	FormText::connect( m_opts.data(), &FormTextOpts::setFontSize,
		q, &FormText::setFontSize );
	FormText::connect( m_opts.data(), &FormTextOpts::bold,
		q, &FormText::bold );
	FormText::connect( m_opts.data(), &FormTextOpts::italic,
		q, &FormText::italic );
	FormText::connect( m_opts.data(), &FormTextOpts::underline,
		q, &FormText::underline );
	FormText::connect( m_opts.data(), &FormTextOpts::textColor,
		q, &FormText::changeTextColor );
	FormText::connect( m_opts.data(), &FormTextOpts::clearFormat,
		q, &FormText::clearFormat );
	FormText::connect( m_opts.data(), &FormTextOpts::alignLeft,
		q, &FormText::alignLeft );
	FormText::connect( m_opts.data(), &FormTextOpts::alignCenter,
		q, &FormText::alignCenter );
	FormText::connect( m_opts.data(), &FormTextOpts::alignRight,
		q, &FormText::alignRight );
}

void
FormTextPrivate::setRect( const QRectF & rect )
{
	m_rect = rect;

	q->setPos( m_rect.topLeft() );

	q->setTextWidth( m_rect.width() );

	QRectF r = q->boundingRect();
	r.moveTo( q->pos() );

	m_proxy->setRect( r );
}


//
// FormText
//

FormText::FormText( const QRectF & rect, Page * form, QGraphicsItem * parent )
	:	QGraphicsTextItem( parent )
	,	FormObject( FormObject::TextType, form, FormObject::ResizeWidth )
	,	d( new FormTextPrivate( rect, this ) )
{
	d->init();
}

FormText::~FormText()
{
}

void
FormText::clearEditMode()
{
	clearSelection();

	d->m_showToolBar = false;

	update();
}

void
FormText::p_contentChanged()
{
	QRectF r = boundingRect();
	r.moveTo( pos() );

	d->m_proxy->setRect( r );
	d->m_proxy->setMinSize( r.size() );
}

Cfg::Text
FormText::cfg() const
{
	Cfg::Text c;

	c.set_objectId( objectId() );

	Cfg::Point p;
	p.set_x( MmPx::instance().toMmX( pos().x() ) );
	p.set_y( MmPx::instance().toMmY( pos().y() ) );

	c.set_pos( p );

	c.set_textWidth( MmPx::instance().toMmX( textWidth() ) );

	c.set_text( Cfg::text( textCursor(), toPlainText() ) );

	c.set_z( zValue() );

	return c;
}

void
FormText::setCfg( const Cfg::Text & c )
{
	setPlainText( QString() );

	QFont f = font();
	QTextCharFormat fmt = textCursor().charFormat();
	QTextBlockFormat b = textCursor().blockFormat();

	foreach( const Cfg::TextStyle & s, c.text() )
	{
		if( std::find( s.style().cbegin(), s.style().cend(),
			Cfg::c_normalStyle ) != s.style().cend() )
		{
			f.setWeight( QFont::Normal );
			f.setItalic( false );
			f.setUnderline( false );

			fmt.setFontWeight( QFont::Normal );
			fmt.setFontItalic( false );
			fmt.setFontUnderline( false );
		}
		else
		{
			if( std::find( s.style().cbegin(), s.style().cend(),
				Cfg::c_boldStyle ) != s.style().cend() )
			{
				f.setWeight( QFont::Bold );
				fmt.setFontWeight( QFont::Bold );
			}
			else
			{
				f.setWeight( QFont::Normal );
				fmt.setFontWeight( QFont::Normal );
			}

			if( std::find( s.style().cbegin(), s.style().cend(),
				Cfg::c_italicStyle ) != s.style().cend() )
			{
				f.setItalic( true );
				fmt.setFontItalic( true );
			}
			else
			{
				f.setItalic( false );
				fmt.setFontItalic( false );
			}

			if( std::find( s.style().cbegin(), s.style().cend(),
				Cfg::c_underlineStyle ) != s.style().cend() )
			{
				f.setUnderline( true );
				fmt.setFontUnderline( true );
			}
			else
			{
				f.setUnderline( false );
				fmt.setFontUnderline( false );
			}
		}

		Cfg::initBlockFormat( b, s );

		f.setPixelSize( MmPx::instance().fromPtY( s.fontSize() ) );

		fmt.setFont( f );

		setFont( f );

		QTextCursor cursor = textCursor();
		cursor.movePosition( QTextCursor::End );
		cursor.setCharFormat( fmt );
		cursor.setBlockFormat( b );
		cursor.insertText( s.text() );
		setTextCursor( cursor );
	}

	document()->clearUndoRedoStacks();

	setObjectId( c.objectId() );

	setTextWidth( MmPx::instance().fromMmX( c.textWidth() ) );

	setPos( QPointF( MmPx::instance().fromMmX( c.pos().x() ),
		MmPx::instance().fromMmY( c.pos().y() ) ) );

	setZValue( c.z() );

	QRectF r = boundingRect();
	r.moveTo( pos() );

	d->m_proxy->setRect( r );
}

void
FormText::enableEditing( bool on )
{
	if( on )
		setTextInteractionFlags( Qt::TextEditorInteraction );
	else
		setTextInteractionFlags( Qt::NoTextInteraction );
}

void
FormText::clearSelection()
{
	QTextCursor c = textCursor();
	c.clearSelection();
	setTextCursor( c );
}

void
FormText::paint( QPainter * painter, const QStyleOptionGraphicsItem * option,
	QWidget * widget )
{
	if( d->m_isFirstPaint )
	{
		p_contentChanged();

		d->m_isFirstPaint = false;
	}

	QGraphicsTextItem::paint( painter, option, widget );

	if( isSelected() && !group() )
	{
		unsetCursor();

		d->m_proxy->show();

		d->m_opts->hide();
	}
	else
	{
		if( !isSelected() && !group() )
			setCursor( Qt::IBeamCursor );

		if( !group() && d->m_showToolBar )
		{
			d->m_opts->setPos( pos() +
				QPointF( 0.0, -d->m_opts->size().height() ) );

			d->m_opts->show();
		}
		else
			d->m_opts->hide();

		d->m_proxy->hide();
	}
}

void
FormText::setPosition( const QPointF & pos, bool pushUndoCommand )
{
	FormObject::setPosition( pos, pushUndoCommand );

	QRectF r = boundingRect();
	r.moveTopLeft( pos );

	d->setRect( r );
}

QPointF
FormText::position() const
{
	return pos();
}

QRectF
FormText::rectangle() const
{
	QRectF r = boundingRect();
	r.moveTopLeft( position() );

	return r;
}

void
FormText::setRectangle( const QRectF & rect, bool pushUndoCommand )
{
	FormObject::setRectangle( rect, pushUndoCommand );

	resize( rect );

	scene()->update();
}

void
FormText::setObjectPen( const QPen & p, bool pushUndoCommand )
{
	FormObject::setObjectPen( p, pushUndoCommand );

	setDefaultTextColor( p.color() );

	update();
}

void
FormText::setFontSize( int s )
{
	QTextCursor c = textCursor();

	if( c.hasSelection() )
	{
		if( c.position() != c.selectionEnd() )
			c.setPosition( c.selectionEnd() );

		QTextCharFormat fmt = c.charFormat();

		QFont f = fmt.font();
		f.setPixelSize( MmPx::instance().fromPtY( s ) );
		fmt.setFont( f );

		textCursor().setCharFormat( fmt );
	}
	else
	{
		QFont f = font();
		f.setPixelSize( MmPx::instance().fromPtY( s ) );
		setFont( f );
	}

	QRectF r = boundingRect();
	r.moveTo( pos() );

	d->m_proxy->setRect( r );
}

void
FormText::bold( bool on )
{
	QTextCursor c = textCursor();

	if( c.hasSelection() )
	{
		if( c.position() != c.selectionEnd() )
			c.setPosition( c.selectionEnd() );

		QTextCharFormat fmt = c.charFormat();

		fmt.setFontWeight( on ? QFont::Bold : QFont::Normal );

		textCursor().setCharFormat( fmt );
	}
	else
	{
		QFont f = font();

		f.setWeight( on ? QFont::Bold : QFont::Normal );

		setFont( f );
	}

	QRectF r = boundingRect();
	r.moveTo( pos() );

	d->m_proxy->setRect( r );
}

void
FormText::italic( bool on )
{
	QTextCursor c = textCursor();

	if( c.hasSelection() )
	{
		if( c.position() != c.selectionEnd() )
			c.setPosition( c.selectionEnd() );

		QTextCharFormat fmt = c.charFormat();

		fmt.setFontItalic( on );

		textCursor().setCharFormat( fmt );
	}
	else
	{
		QFont f = font();

		f.setItalic( on );

		setFont( f );
	}

	QRectF r = boundingRect();
	r.moveTo( pos() );

	d->m_proxy->setRect( r );
}

void
FormText::underline( bool on )
{
	QTextCursor c = textCursor();

	if( c.hasSelection() )
	{
		if( c.position() != c.selectionEnd() )
			c.setPosition( c.selectionEnd() );

		QTextCharFormat fmt = c.charFormat();

		fmt.setFontUnderline( on );

		textCursor().setCharFormat( fmt );
	}
	else
	{
		QFont f = font();

		f.setUnderline( on );

		setFont( f );
	}

	QRectF r = boundingRect();
	r.moveTo( pos() );

	d->m_proxy->setRect( r );
}

void
FormText::changeTextColor()
{
}

void
FormText::p_cursorChanged( const QTextCursor & cursor )
{
	d->m_opts->updateState( cursor );
}

void
FormText::clearFormat()
{
	QTextCursor c = textCursor();

	if( c.hasSelection() )
	{
		QTextCharFormat fmt = c.charFormat();

		fmt.setFontUnderline( false );
		fmt.setFontItalic( false );
		fmt.setFontWeight( QFont::Normal );
		QFont f = fmt.font();
		f.setPixelSize( MmPx::instance().fromPtY( 10.0 ) );
		fmt.setFont( f );

		textCursor().setCharFormat( fmt );
	}
	else
	{
		QFont f = font();
		f.setUnderline( false );
		f.setItalic( false );
		f.setWeight( QFont::Normal );
		f.setPixelSize( MmPx::instance().fromPtY( 10.0 ) );

		setFont( f );
	}

	QRectF r = boundingRect();
	r.moveTo( pos() );

	d->m_proxy->setRect( r );
}

void
FormText::alignLeft()
{
	QTextCursor c = textCursor();
	QTextBlockFormat b = c.blockFormat();
	b.setAlignment( Qt::AlignLeft );
	c.setBlockFormat( b );
	setTextCursor( c );
}

void
FormText::alignCenter()
{
	QTextCursor c = textCursor();
	QTextBlockFormat b = c.blockFormat();
	b.setAlignment( Qt::AlignCenter );
	c.setBlockFormat( b );
	setTextCursor( c );
}

void
FormText::alignRight()
{
	QTextCursor c = textCursor();
	QTextBlockFormat b = c.blockFormat();
	b.setAlignment( Qt::AlignRight );
	c.setBlockFormat( b );
	setTextCursor( c );
}

void
FormText::resize( const QRectF & rect )
{
	d->setRect( rect );

	form()->update();
}

void
FormText::moveResizable( const QPointF & delta )
{
	moveBy( delta.x(), delta.y() );
}

void
FormText::focusInEvent( QFocusEvent * e )
{
	FormAction::instance()->form()->clearEditModeInTexts();

	d->m_showToolBar = true;

	QGraphicsTextItem::focusInEvent( e );
}

void
FormText::keyPressEvent( QKeyEvent * e )
{
	if( e == QKeySequence::Undo || e == QKeySequence::Redo )
		e->ignore();
	else
		QGraphicsTextItem::keyPressEvent( e );
}

bool
FormText::sceneEvent( QEvent * e )
{
	switch( e->type() )
	{
		case QEvent::ShortcutOverride :
		{
			QKeyEvent * ke = static_cast< QKeyEvent* > ( e );

			if( ke == QKeySequence::Redo || ke == QKeySequence::Undo )
			{
				ke->ignore();

				return false;
			}
			else
				return QGraphicsTextItem::sceneEvent( e );
		}
			break;

		default :
			return QGraphicsTextItem::sceneEvent( e );
	}
}

QSizeF
FormText::defaultSize() const
{
	return QSizeF( MmPx::instance().fromMmX( 15.0 ), MmPx::instance().fromMmY( 6.0 ) );
}

void
FormText::mouseReleaseEvent( QGraphicsSceneMouseEvent * event )
{
	QGraphicsTextItem::mouseReleaseEvent( event );

	p_cursorChanged( textCursor() );
}

void
FormText::keyReleaseEvent( QKeyEvent * event )
{
	QGraphicsTextItem::keyReleaseEvent( event );

	p_cursorChanged( textCursor() );
}

} /* namespace Core */

} /* namespace Prototyper */
