
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
#include "project_description_tab.hpp"
#include "text_editor.hpp"
#include "text_opts_bar.hpp"

// Qt include.
#include <QVBoxLayout>


namespace Prototyper {

namespace Core {

//
// ProjectDescTabPrivate
//

class ProjectDescTabPrivate {
public:
	explicit ProjectDescTabPrivate( ProjectDescTab * parent )
		:	q( parent )
		,	m_bar( nullptr )
		,	m_editor( nullptr )
	{
	}

	//! Init.
	void init();

	//! Parent.
	ProjectDescTab * q;
	//! Bar.
	TextOptsBar * m_bar;
	//! Editor.
	TextEditor * m_editor;
}; // class ProjectDescTabPrivate

void
ProjectDescTabPrivate::init()
{
	auto * layout = new QVBoxLayout( q );
	layout->setContentsMargins( 0, 0, 0, 0 );
	layout->setSpacing( 0 );

	m_bar = new TextOptsBar( TextOptsBar::Large, q );
	m_bar->setObjectName( QStringLiteral( "m_textOptsBar" ) );

	m_editor = new TextEditor( q );
	m_editor->setFrameStyle( QFrame::NoFrame );
	layout->addWidget( m_editor );

	ProjectDescTab::connect( m_editor, &TextEditor::cursorChanged,
		m_bar, &TextOptsBar::updateState );
	ProjectDescTab::connect( m_bar, &TextOptsBar::setFontSize,
		m_editor, &TextEditor::setFontSize );
	ProjectDescTab::connect( m_bar, &TextOptsBar::bold,
		m_editor, &TextEditor::bold );
	ProjectDescTab::connect( m_bar, &TextOptsBar::italic,
		m_editor, &TextEditor::italic );
	ProjectDescTab::connect( m_bar, &TextOptsBar::underline,
		m_editor, &TextEditor::underline );
	ProjectDescTab::connect( m_bar, &TextOptsBar::textColor,
		m_editor, &TextEditor::changeTextColor );
	ProjectDescTab::connect( m_bar, &TextOptsBar::clearFormat,
		m_editor, &TextEditor::clearFormat );
	ProjectDescTab::connect( m_bar, &TextOptsBar::alignLeft,
		m_editor, &TextEditor::alignLeft );
	ProjectDescTab::connect( m_bar, &TextOptsBar::alignCenter,
		m_editor, &TextEditor::alignCenter );
	ProjectDescTab::connect( m_bar, &TextOptsBar::alignRight,
		m_editor, &TextEditor::alignRight );
	ProjectDescTab::connect( m_bar, &TextOptsBar::insertLink,
		m_editor, &TextEditor::insertLink );
}


//
// ProjectDescTab
//

ProjectDescTab::ProjectDescTab( QWidget * parent, Qt::WindowFlags f )
	:	QWidget( parent, f )
	,	d( new ProjectDescTabPrivate( this ) )
{
	d->init();
}

ProjectDescTab::~ProjectDescTab() = default;

QToolBar *
ProjectDescTab::toolBar() const
{
	return d->m_bar;
}

TextEditor *
ProjectDescTab::editor() const
{
	return d->m_editor;
}

} /* namespace Core */

} /* namespace Prototyper */
