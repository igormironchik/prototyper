
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

#ifndef PROTOTYPER__CORE__DESC_WINDOW_HPP__INCLUDED
#define PROTOTYPER__CORE__DESC_WINDOW_HPP__INCLUDED

// Qt include.
#include <QMainWindow>
#include <QScopedPointer>

QT_BEGIN_NAMESPACE
class QTextDocument;
QT_END_NAMESPACE


namespace Prototyper {

namespace Core {

class Form;


//
// DescWindow
//

class DescWindowPrivate;

//! Window with descriptions of form's items.
class DescWindow
	:	public QMainWindow
{
	Q_OBJECT

signals:
	//! Changed.
	void changed();

public:
	DescWindow( QWidget * parent = 0, Qt::WindowFlags flags = 0 );
	~DescWindow();

public slots:
	//! Set editors.
	void setEditors( const QString & current,
		const QMap< QString, QSharedPointer< QTextDocument > > & docs,
		Form * form );
	//! Rename item.
	void renameItem( Form * form, const QString & old, const QString & name );
	//! Delete item.
	void deleteItem( Form * form, const QString & name );

protected:
	void closeEvent( QCloseEvent * event )
		Q_DECL_OVERRIDE;

private slots:
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
	//! Current index changed.
	void currentIndexChanged( int index );
	//! Cursor position changed.
	void cursorPositionChanged();

private:
	friend class DescWindowPrivate;

	Q_DISABLE_COPY( DescWindow )

	QScopedPointer< DescWindowPrivate > d;
}; // class DescWindow

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__DESC_WINDOW_HPP__INCLUDED
