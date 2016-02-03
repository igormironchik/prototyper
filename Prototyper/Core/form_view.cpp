
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
#include "form_view.hpp"
#include "form_scene.hpp"
#include "form.hpp"


namespace Prototyper {

namespace Core {

//
// FormViewPrivate
//

class FormViewPrivate {
public:
	FormViewPrivate( Cfg::Form & cfg, FormView * parent )
		:	q( parent )
		,	m_scene( 0 )
		,	m_form( 0 )
		,	m_cfg( cfg )
	{
	}

	//! Init.
	void init();

	//! Parent.
	FormView * q;
	//! Scene.
	FormScene * m_scene;
	//! Form.
	Form * m_form;
	//! Cfg.
	Cfg::Form & m_cfg;
}; // class FormViewPrivate

void
FormViewPrivate::init()
{
	q->setFrameStyle( QFrame::NoFrame );

	m_scene = new FormScene( q );

	q->setScene( m_scene );

	m_form = new Form( m_cfg );

	m_scene->addItem( m_form );
}


//
// FormView
//

FormView::FormView( Cfg::Form & cfg, QWidget * parent )
	:	QGraphicsView( parent )
	,	d( new FormViewPrivate( cfg, this ) )
{
	d->init();
}

FormView::~FormView()
{
}

FormScene *
FormView::formScene() const
{
	return d->m_scene;
}

Form *
FormView::form() const
{
	return d->m_form;
}

} /* namespace Core */

} /* namespace Prototyper */
