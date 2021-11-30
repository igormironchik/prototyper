
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

#ifndef PROTOTYPER__CORE__TABS_LIST_HPP__INCLUDED
#define PROTOTYPER__CORE__TABS_LIST_HPP__INCLUDED

// Qt include.
#include <QDockWidget>
#include <QListView>

// C++ include.
#include <memory>

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
class TabsView final
	:	public QListView
{
	Q_OBJECT

signals:
	//! Enter button pressed.
	void enterPressed( const QModelIndex & );
	//! Activate tab.
	void activateTab( const QModelIndex & );

public:
	explicit TabsView( QWidget * parent );
	~TabsView() override;

protected:
	 void keyPressEvent( QKeyEvent * event ) override;
	 void contextMenuEvent( QContextMenuEvent * event ) override;

private slots:
	 //! Activate tab.
	 void p_activateTab();
	 //! Rename tab.
	 void p_renameTab();
	 //! Add form.
	 void p_addForm();
	 //! Delete form.
	 void p_deleteForm();

private:
	 friend class TabsViewPrivate;

	 Q_DISABLE_COPY( TabsView )

	 std::unique_ptr< TabsViewPrivate > d;
}; // class TabsView


//
// TabsList
//

class TabsListPrivate;

//! Tabs list dock widget.
class TabsList final
	:	public QDockWidget
{
	Q_OBJECT

public:
	TabsList( QWidget * parent = 0, Qt::WindowFlags f = Qt::WindowFlags() );
	~TabsList() override;

	//! \return Model.
	QStringListModel * model() const;

private slots:
	void p_activateTab( const QModelIndex & index );

private:
	friend class TabsListPrivate;

	Q_DISABLE_COPY( TabsList )

	std::unique_ptr< TabsListPrivate > d;
}; // class TabsList

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__TABS_LIST_HPP__INCLUDED
