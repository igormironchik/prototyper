
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

#ifndef PROTOTYPER__CORE__FORM_TEXT_OPTS_HPP__INCLUDED
#define PROTOTYPER__CORE__FORM_TEXT_OPTS_HPP__INCLUDED

// Qt include.
#include <QGraphicsProxyWidget>
#include <QScopedPointer>


namespace Prototyper {

namespace Core {

//
// FormTextOpts
//

class FormTextOptsPrivate;

//! Toolbar with text options on the form.
class FormTextOpts
	:	public QGraphicsProxyWidget
{
	Q_OBJECT

signals:
	//! Less font size.
	void lessFontSize();
	//! More font size.
	void moreFontSize();
	//! Bold.
	void bold( bool on );
	//! Italic.
	void italic( bool on );
	//! Underline.
	void underline( bool on );
	//! Text color.
	void textColor();
	//! Clear format.
	void clearFormat();
	//! Align left.
	void alignLeft();
	//! Align center.
	void alignCenter();
	//! Align right.
	void alignRight();

public:
	explicit FormTextOpts( QGraphicsItem * parent );
	~FormTextOpts();

public slots:
	//! Update state of the buttons.
	void updateState( const QTextCursor & cursor );

private:
	Q_DISABLE_COPY( FormTextOpts )

	QScopedPointer< FormTextOptsPrivate > d;
}; // class FormTextOpts

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__FORM_TEXT_OPTS_HPP__INCLUDED
