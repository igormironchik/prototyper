
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

#ifndef PROTOTYPER__CORE__TEXT_EDITOR_HPP__INCLUDED
#define PROTOTYPER__CORE__TEXT_EDITOR_HPP__INCLUDED

// Qt include.
#include <QTextEdit>
#include <QScopedPointer>


namespace Prototyper {

namespace Core {

//
// TextEditor
//

class TextEditorPrivate;

//! Text editor.
class TextEditor
	:	public QTextEdit
{
	Q_OBJECT

signals:
	//! Cursor changed.
	void cursorChanged( const QTextCursor & );

public:
	TextEditor( QWidget * parent = 0 );
	~TextEditor();

public slots:
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
	//! Change text color.
	void changeTextColor();
	//! Clear format.
	void clearFormat();

private slots:
	//! Cursor changed.
	void slotCursorChanged();

private:
	friend class TextEditorPrivate;

	Q_DISABLE_COPY( TextEditor )

	QScopedPointer< TextEditorPrivate > d;
}; // class TextEditor

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__TEXT_EDITOR_HPP__INCLUDED
