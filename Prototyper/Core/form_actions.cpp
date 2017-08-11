
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
#include "form_actions.hpp"
#include "form.hpp"

// Qt include.
#include <QApplication>
#include <QColor>


namespace Prototyper {

namespace Core {

//
// FormActionPrivate
//

class FormActionPrivate {
public:
	explicit FormActionPrivate( FormAction * parent )
		:	q( parent )
		,	m_mode( FormAction::Select )
		,	m_form( 0 )
		,	m_fillColor( Qt::transparent )
		,	m_strokeColor( Qt::black )
		,	m_snap( true )
		,	m_flags( 0 )
	{
	}

	//! Parent.
	FormAction * q;
	//! Mode.
	FormAction::Mode m_mode;
	//! Form.
	Form * m_form;
	//! Fill color.
	QColor m_fillColor;
	//! Stroke color.
	QColor m_strokeColor;
	//! Is snap enabled?
	bool m_snap;
	//! Flags.
	FormAction::Flags m_flags;
}; // class FormActionPrivate


//
// FormAction
//

FormAction::FormAction()
	:	d( new FormActionPrivate( this ) )
{
}

FormAction::~FormAction()
{
}

static FormAction * formActionInstance = 0;

void
FormAction::cleanup()
{
	delete formActionInstance;

	formActionInstance = 0;
}

FormAction *
FormAction::instance()
{
	if( !formActionInstance )
	{
		formActionInstance = new FormAction;

		qAddPostRoutine( &FormAction::cleanup );
	}

	return formActionInstance;
}

const FormAction::Flags &
FormAction::flags() const
{
	return d->m_flags;
}

void
FormAction::setFlags( const Flags & f )
{
	d->m_flags = f;
}

void
FormAction::setFlag( Flag f )
{
	d->m_flags |= f;
}

bool
FormAction::testFlag( Flag f ) const
{
	return d->m_flags.testFlag( f );
}

void
FormAction::clearFlag( Flag f )
{
	d->m_flags &= ~Flags( f );
}

FormAction::Mode
FormAction::mode() const
{
	return d->m_mode;
}

void
FormAction::setMode( Mode m )
{
	d->m_mode = m;
}

Form *
FormAction::form() const
{
	return d->m_form;
}

void
FormAction::setForm( Form * f )
{
	d->m_form = f;
}

const QColor &
FormAction::fillColor() const
{
	return d->m_fillColor;
}

void
FormAction::setFillColor( const QColor & c )
{
	d->m_fillColor = c;
}

const QColor &
FormAction::strokeColor() const
{
	return d->m_strokeColor;
}

void
FormAction::setStrokeColor( const QColor & c )
{
	d->m_strokeColor = c;
}

bool
FormAction::isSnapEnabled() const
{
	return d->m_snap;
}

void
FormAction::enableSnap( bool on )
{
	d->m_snap = on;
}

} /* namespace Core */

} /*  namespace Prototyper */
