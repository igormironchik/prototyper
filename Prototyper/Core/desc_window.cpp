
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
#include "desc_window.hpp"
#include "text_opts_bar.hpp"
#include "text_editor.hpp"

// Qt include.
#include <QCloseEvent>
#include <QToolBox>
#include <QTextDocument>
#include <QList>

#include <QDebug>
#include <QApplication>


namespace Prototyper {

namespace Core {

//
// DescWindowPrivate
//

class DescWindowPrivate {
public:
	DescWindowPrivate( DescWindow * parent )
		:	q( parent )
		,	m_box( 0 )
		,	m_bar( 0 )
		,	m_form( 0 )
	{
	}

	//! Init.
	void init();
	//! \return Current text editor.
	TextEditor * currentEditor() const;
	//! \return Is editor has focus.
	bool isEditorHasFocus() const;
	//! Clear.
	void clear();

	//! Parent.
	DescWindow * q;
	//! Toolbox.
	QToolBox * m_box;
	//! Toolbar.
	TextOptsBar * m_bar;
	//! Editors.
	QList< TextEditor* > m_editors;
	//! Form.
	Form * m_form;
}; // class DescWindowPrivate

void
DescWindowPrivate::init()
{
	q->setWindowTitle( DescWindow::tr( "Descriptions" ) );

	m_bar = new TextOptsBar( TextOptsBar::Large, q );

	q->addToolBar( Qt::TopToolBarArea, m_bar );

	DescWindow::connect( m_bar, &TextOptsBar::lessFontSize,
		q, &DescWindow::lessFontSize );
	DescWindow::connect( m_bar, &TextOptsBar::moreFontSize,
		q, &DescWindow::moreFontSize );
	DescWindow::connect( m_bar, &TextOptsBar::bold,
		q, &DescWindow::bold );
	DescWindow::connect( m_bar, &TextOptsBar::italic,
		q, &DescWindow::italic );
	DescWindow::connect( m_bar, &TextOptsBar::underline,
		q, &DescWindow::underline );
	DescWindow::connect( m_bar, &TextOptsBar::textColor,
		q, &DescWindow::textColor );
	DescWindow::connect( m_bar, &TextOptsBar::clearFormat,
		q, &DescWindow::clearFormat );
}

TextEditor *
DescWindowPrivate::currentEditor() const
{
	return static_cast< TextEditor* > ( m_box->currentWidget() );
}

bool
DescWindowPrivate::isEditorHasFocus() const
{
	return ( m_box->currentWidget() != 0 );
}

void
DescWindowPrivate::clear()
{
	for( int i = 0; i < m_editors.size(); ++i )
		DescWindow::disconnect( m_editors.at( i ), 0, 0, 0 );

	m_editors.clear();

	m_form = 0;
}


//
// DescWindow
//

DescWindow::DescWindow( QWidget * parent, Qt::WindowFlags flags )
	:	QMainWindow( parent, flags )
	,	d( new DescWindowPrivate( this ) )
{
	d->init();
}

DescWindow::~DescWindow()
{
}

void
DescWindow::setEditors( const QString & current,
	const QMap< QString, QSharedPointer< QTextDocument > > & docs, Form * form )
{
	d->clear();

	if( d->m_box )
	{
		disconnect( d->m_box, 0, 0, 0 );

		setCentralWidget( 0 );

		delete d->m_box;
	}

	d->m_box = new QToolBox( this );

	setCentralWidget( d->m_box );

	DescWindow::connect( d->m_box, &QToolBox::currentChanged,
		this, &DescWindow::currentIndexChanged );

	auto it = docs.constBegin();
	auto last = docs.constEnd();

	int i = 0;
	int index = -1;

	for( ; it != last; ++it, ++i )
	{
		if( it.key() == current )
			index = i;

		d->m_editors.append( new TextEditor( d->m_box ) );

		d->m_editors.last()->setDocument( it.value().data() );

		d->m_editors.last()->setFontPointSize( 10.0 );

		QTextCursor c = d->m_editors.last()->textCursor();

		QTextCharFormat fmt = c.charFormat();
		fmt.setFontPointSize( 10.0 );

		c.setCharFormat( fmt );

		d->m_editors.last()->setTextCursor( c );

		connect( d->m_editors.last(), &TextEditor::cursorPositionChanged,
			this, &DescWindow::cursorPositionChanged );

		connect( d->m_editors.last(), &TextEditor::changed,
			this, &DescWindow::changed );

		d->m_box->addItem( d->m_editors.last(), it.key() );
	}

	if( index >= 0 )
		d->m_box->setCurrentIndex( index );

	d->m_form = form;
}

void
DescWindow::renameItem( Form * form, const QString & old,
	const QString & name )
{
	if( d->m_form == form )
	{
		for( int i = 0; i < d->m_box->count(); ++i )
		{
			if( d->m_box->itemText( i ) == old )
			{
				d->m_box->setItemText( i, name );

				return;
			}
		}
	}
}

void
DescWindow::closeEvent( QCloseEvent * event )
{
	event->ignore();

	hide();
}

void
DescWindow::lessFontSize()
{
	if( d->isEditorHasFocus() )
		d->currentEditor()->lessFontSize();
}

void
DescWindow::moreFontSize()
{
	if( d->isEditorHasFocus() )
		d->currentEditor()->moreFontSize();
}

void
DescWindow::bold( bool on )
{
	if( d->isEditorHasFocus() )
		d->currentEditor()->bold( on );
}

void
DescWindow::italic( bool on )
{
	if( d->isEditorHasFocus() )
		d->currentEditor()->italic( on );
}

void
DescWindow::underline( bool on )
{
	if( d->isEditorHasFocus() )
		d->currentEditor()->underline( on );
}

void
DescWindow::textColor()
{
	if( d->isEditorHasFocus() )
		d->currentEditor()->textColor();
}

void
DescWindow::clearFormat()
{
	if( d->isEditorHasFocus() )
		d->currentEditor()->clearFormat();
}

void
DescWindow::currentIndexChanged( int index )
{
	if( index >= 0 )
		d->m_bar->updateState( d->currentEditor()->textCursor() );
}

void
DescWindow::cursorPositionChanged()
{
	d->m_bar->updateState( d->currentEditor()->textCursor() );
}

} /* namespace Core */

} /* namespace Prototyper */
