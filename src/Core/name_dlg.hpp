
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

#ifndef PROTOTYPER__CORE__NAME_DLG_HPP__INCLUDED
#define PROTOTYPER__CORE__NAME_DLG_HPP__INCLUDED

// Qt include.
#include <QDialog>
#include <QScopedPointer>


namespace Prototyper {

namespace Core {

//
// NameDlg
//

class NameDlgPrivate;

//! Name dialog.
class NameDlg final
	:	public QDialog
{
	Q_OBJECT

public:
	NameDlg( const QStringList & names,
		const QString & title,
		const QString & oldName,
		QWidget * parent = 0, Qt::WindowFlags f = Qt::WindowFlags() );
	~NameDlg() override;

	//! \return Intered name.
	QString name() const;

private slots:
	//! Text changed.
	void textChanged( const QString & text );

private:
	friend class NameDlgPrivate;

	Q_DISABLE_COPY( NameDlg )

	QScopedPointer< NameDlgPrivate > d;
}; // class NameDlg

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__NAME_DLG_HPP__INCLUDED
