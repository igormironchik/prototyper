
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

#ifndef PROTOTYPER__CORE__TABS_LIST_HPP__INCLUDED
#define PROTOTYPER__CORE__TABS_LIST_HPP__INCLUDED

// Qt include.
#include <QDockWidget>
#include <QScopedPointer>
#include <QListView>

QT_BEGIN_NAMESPACE
class QStringListModel;
QT_END_NAMESPACE


namespace Prototyper {

namespace Core {

//
// TabsView
//

class TabsViewPrivate;

//! Tabs list view.
class TabsView
	:	public QListView
{
	Q_OBJECT

signals:
	//! Enter button pressed.
	void enterPressed( const QModelIndex & );
	//! Activate tab.
	void activateTab( const QModelIndex & );

public:
	TabsView( QWidget * parent );
	~TabsView();

protected:
	 void keyPressEvent( QKeyEvent * event ) Q_DECL_OVERRIDE;
	 void contextMenuEvent( QContextMenuEvent * event ) Q_DECL_OVERRIDE;

private slots:
	 //! Activate tab.
	 void p_activateTab();

private:
	 friend class TabsViewPrivate;

	 Q_DISABLE_COPY( TabsView )

	 QScopedPointer< TabsViewPrivate > d;
}; // class TabsView


//
// TabsList
//

class TabsListPrivate;

//! Form hierarchy.
class TabsList
	:	public QDockWidget
{
	Q_OBJECT

public:
	TabsList( QWidget * parent = 0, Qt::WindowFlags f = 0 );
	~TabsList();

	//! \return Model.
	QStringListModel * model() const;

private slots:
	void p_doubleClicked( const QModelIndex & index );

private:
	friend class TabsListPrivate;

	Q_DISABLE_COPY( TabsList )

	QScopedPointer< TabsListPrivate > d;
}; // class TabsList

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__TABS_LIST_HPP__INCLUDED
