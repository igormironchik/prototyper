
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

#ifndef PROTOTYPER__CORE__NEW_FORM_DLG_HPP__INCLUDED
#define PROTOTYPER__CORE__NEW_FORM_DLG_HPP__INCLUDED

// Qt include.
#include <QDialog>
#include <QScopedPointer>


namespace Prototyper {

namespace Core {

//
// NewFormDlg
//

class NewFormDlgPrivate;

//! Name dialog.
class NewFormDlg
	:	public QDialog
{
	Q_OBJECT

public:
	NewFormDlg( const QStringList & names,
		const QString & title,
		QWidget * parent = 0, Qt::WindowFlags f = 0 );
	~NewFormDlg();

	//! \return Intered name.
	QString name() const;

	//! \return Size.
	QSize size() const;

private slots:
	//! Text changed.
	void textChanged( const QString & text );

private:
	friend class NewFormDlgPrivate;

	Q_DISABLE_COPY( NewFormDlg )

	QScopedPointer< NewFormDlgPrivate > d;
}; // class NewFormDlg

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__NEW_FORM_DLG_HPP__INCLUDED
