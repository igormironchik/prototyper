
/*
	SPDX-FileCopyrightText: 2016-2024 Igor Mironchik <igor.mironchik@gmail.com>
	SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef PROTOTYPER__CORE__PROJECT_WIDGET_TAB_BAR_HPP__INCLUDED
#define PROTOTYPER__CORE__PROJECT_WIDGET_TAB_BAR_HPP__INCLUDED

// Qt include.
#include <QTabBar>

// C++ include.
#include <memory>


namespace Prototyper {

namespace Core {

//
// ProjectTabBar
//

class ProjectTabBarPrivate;

//! Project's tab bar.
class ProjectTabBar final
	:	public QTabBar
{
	Q_OBJECT

signals:
	//! Form rename request.
	void formRenameRequest( const QString & name );
	//! Form delete request.
	void formDeleteRequest( const QString & name );
	//! Add form request.
	void formAddRequest();

public:
	explicit ProjectTabBar( QWidget * parent = 0 );
	~ProjectTabBar() override;

private slots:
	//! Rename tab.
	void renameTab();
	//! Add form.
	void addPage();
	//! Delete form.
	void deleteForm();

protected:
	void contextMenuEvent( QContextMenuEvent * event ) override;

private:
	Q_DISABLE_COPY( ProjectTabBar )

	std::unique_ptr< ProjectTabBarPrivate > d;
}; // class ProjectTabBar

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__PROJECT_WIDGET_TAB_BAR_HPP__INCLUDED
