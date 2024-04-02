
/*
	SPDX-FileCopyrightText: 2016-2024 Igor Mironchik <igor.mironchik@gmail.com>
	SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef PROTOTYPER__CORE__NAME_DLG_HPP__INCLUDED
#define PROTOTYPER__CORE__NAME_DLG_HPP__INCLUDED

// Qt include.
#include <QDialog>

// C++ include.
#include <memory>


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

	std::unique_ptr< NameDlgPrivate > d;
}; // class NameDlg

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__NAME_DLG_HPP__INCLUDED
