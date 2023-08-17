
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
#include "text_opts.hpp"
#include "../text_opts_bar.hpp"


namespace Prototyper {

namespace Core {

//
// FormTextOptsPrivate
//

class FormTextOptsPrivate {
public:
	explicit FormTextOptsPrivate( FormTextOpts * parent )
		:	q( parent )
		,	m_bar( nullptr )
	{
	}

	//! Init.
	void init();

	//! Parent.
	FormTextOpts * q;
	//! Toolbar.
	TextOptsBar * m_bar;
}; // class FormTextOptsPrivate

void
FormTextOptsPrivate::init()
{
	m_bar = new TextOptsBar( TextOptsBar::Small );

	q->setWidget( m_bar );

	FormTextOpts::connect( m_bar, &TextOptsBar::setFontSize,
		q, &FormTextOpts::setFontSize );
	FormTextOpts::connect( m_bar, &TextOptsBar::bold,
		q, &FormTextOpts::bold );
	FormTextOpts::connect( m_bar, &TextOptsBar::italic,
		q, &FormTextOpts::italic );
	FormTextOpts::connect( m_bar, &TextOptsBar::underline,
		q, &FormTextOpts::underline );
	FormTextOpts::connect( m_bar, &TextOptsBar::textColor,
		q, &FormTextOpts::textColor );
	FormTextOpts::connect( m_bar, &TextOptsBar::clearFormat,
		q, &FormTextOpts::clearFormat );
	FormTextOpts::connect( m_bar, &TextOptsBar::alignLeft,
		q, &FormTextOpts::alignLeft );
	FormTextOpts::connect( m_bar, &TextOptsBar::alignCenter,
		q, &FormTextOpts::alignCenter );
	FormTextOpts::connect( m_bar, &TextOptsBar::alignRight,
		q, &FormTextOpts::alignRight );
}


//
// FormTextOpts
//

FormTextOpts::FormTextOpts( QGraphicsItem * parent )
	:	QGraphicsProxyWidget( parent )
	,	d( new FormTextOptsPrivate( this ) )
{
	d->init();
}

FormTextOpts::~FormTextOpts() = default;

void
FormTextOpts::updateState( const QTextCursor & cursor )
{
	d->m_bar->updateState( cursor );
}

} /* namespace Core */

} /* namespace Prototyper */
