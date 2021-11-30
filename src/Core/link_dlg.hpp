
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

#ifndef PROTOTYPER__CORE__LINK_DLG_HPP__INCLUDED
#define PROTOTYPER__CORE__LINK_DLG_HPP__INCLUDED

// Qt include.
#include <QDialog>

// C++ include.
#include <memory>


namespace Prototyper {

namespace Core {

//
// LinkDlg
//

class LinkDlgPrivate;

//! Link dialog.
class LinkDlg final
	:	public QDialog
{
	Q_OBJECT

public:
	LinkDlg( QWidget * parent = nullptr );
	~LinkDlg() override;

	//! \return Url.
	QString url() const;
	//! \return Text.
	QString text() const;

private:
	Q_DISABLE_COPY( LinkDlg )

	std::unique_ptr< LinkDlgPrivate > d;
}; // class LinkDlg

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__LINK_DLG_HPP__INCLUDED
