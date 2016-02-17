
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

#ifndef PROTOTYPER__CORE__LINK_DLG_HPP__INCLUDED
#define PROTOTYPER__CORE__LINK_DLG_HPP__INCLUDED

// Qt include.
#include <QDialog>
#include <QScopedPointer>


namespace Prototyper {

namespace Core {

//
// LinkDlg
//

class LinkDlgPrivate;

//! Name dialog.
class LinkDlg
	:	public QDialog
{
	Q_OBJECT

public:
	explicit LinkDlg( const QStringList & names,
		QWidget * parent = 0, Qt::WindowFlags f = 0 );
	~LinkDlg();

	//! \return Selected form.
	QString form() const;

private slots:
	//! Item clicked.
	void clicked( const QModelIndex & index );

private:
	friend class LinkDlgPrivate;

	Q_DISABLE_COPY( LinkDlg )

	QScopedPointer< LinkDlgPrivate > d;
}; // class LinkDlg

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__LINK_DLG_HPP__INCLUDED
