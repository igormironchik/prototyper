
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

#ifndef PROTOTYPER__CORE__PROJECT_WIDGET_HPP__INCLUDED
#define PROTOTYPER__CORE__PROJECT_WIDGET_HPP__INCLUDED

// Qt include.
#include <QWidget>
#include <QScopedPointer>

QT_BEGIN_NAMESPACE
class QTabWidget;
class QUndoGroup;
QT_END_NAMESPACE


namespace Prototyper {

namespace Core {

namespace Cfg {

class Project;

} /* namespace Cfg */

class PageView;
class ProjectDescTab;

//
// ProjectWidget
//

class ProjectWidgetPrivate;

//! Main window with project.
class ProjectWidget
	:	public QWidget
{
	Q_OBJECT

signals:
	//! Project has been modified.
	void changed();
	//! Form added.
	void formAdded( PageView * );
	//! Form deleted.
	void formDeleted( PageView * );

public:
	explicit ProjectWidget( Cfg::Project & cfg,
		QWidget * parent = 0, Qt::WindowFlags f = 0 );
	~ProjectWidget();

	//! \return Pages.
	const QList< PageView* > & pages() const;

	//! Enable/disable selection.
	void enableSelection( bool on = true );

	//! \return Pages' names.
	QStringList pagesNames() const;

	//! \return Project tab name.
	QString projectTabName() const;

	//! \return Tab widget.
	QTabWidget * tabs() const;

	//! \return Project's description tab.
	ProjectDescTab * descriptionTab() const;

	//! Set project.
	void setProject( const Cfg::Project & cfg );

	//! \return Undo group.
	QUndoGroup * undoGroup() const;

	//! Clean undo group.
	void cleanUndoGroup();

	//! \return Is tab renamed?
	bool isTabRenamed() const;
	//! Set tab renamed.
	void setTabRenamed( bool on = true );

public slots:
	//! Add page.
	void addPage();
	//! Rename tab.
	void renameTab( const QString & oldName );
	//! Delete form.
	void deleteForm( const QString & name );
	//! New project.
	void newProject();
	//! Activate tabe.
	void activateTab( const QString & tabName );

private slots:
	friend class ProjectWidgetPrivate;

	//! Tab changed.
	void tabChanged( int index );

private:
	friend class ProjectWidgetPrivate;

	Q_DISABLE_COPY( ProjectWidget )

	QScopedPointer< ProjectWidgetPrivate > d;
}; // class ProjectWidget

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__PROJECT_WIDGET_HPP__INCLUDED
