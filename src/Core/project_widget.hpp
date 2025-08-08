
/*
    SPDX-FileCopyrightText: 2016-2024 Igor Mironchik <igor.mironchik@gmail.com>
    SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef PROTOTYPER__CORE__PROJECT_WIDGET_HPP__INCLUDED
#define PROTOTYPER__CORE__PROJECT_WIDGET_HPP__INCLUDED

// Qt include.
#include <QWidget>

// C++ include.
#include <memory>

QT_BEGIN_NAMESPACE
class QTabWidget;
class QUndoGroup;
QT_END_NAMESPACE

namespace Prototyper
{

namespace Core
{

namespace Cfg
{

class Project;

} /* namespace Cfg */

class PageView;
class ProjectDescTab;
class ProjectWindow;

//
// ProjectWidget
//

class ProjectWidgetPrivate;

//! Main widget with project.
class ProjectWidget final : public QWidget
{
    Q_OBJECT

signals:
    //! Project has been modified.
    void changed();
    //! Page added.
    void pageAdded(Prototyper::Core::PageView *);
    //! Page deleted.
    void pageDeleted(Prototyper::Core::PageView *);

public:
    explicit ProjectWidget(Cfg::Project &cfg,
                           ProjectWindow *parent = 0,
                           Qt::WindowFlags f = Qt::WindowFlags());
    ~ProjectWidget() override;

    //! \return Pages.
    const QList<PageView *> &pages() const;

    //! Enable/disable selection.
    void enableSelection(bool on = true);

    //! \return Pages' names.
    QStringList pagesNames() const;

    //! \return Project tab name.
    QString projectTabName() const;

    //! \return Tab widget.
    QTabWidget *tabs() const;

    //! \return Project's description tab.
    ProjectDescTab *descriptionTab() const;

    //! Set project.
    void setProject(const Cfg::Project &cfg);

    //! \return Undo group.
    QUndoGroup *undoGroup() const;

    //! Clean undo group.
    void cleanUndoGroup();

    //! \return Is tab renamed?
    bool isTabRenamed() const;
    //! Set tab renamed.
    void setTabRenamed(bool on = true);

    //! \return Is comment changed?
    bool isCommentChanged() const;
    //! Clear comment changed flag.
    void clearCommentChanged();

public slots:
    //! Add page.
    void addPage();
    //! Rename tab.
    void renameTab(const QString &oldName);
    //! Delete page.
    void deletePage(const QString &name);
    //! New project.
    void newProject();
    //! Activate tabe.
    void activateTab(const QString &tabName);

private slots:
    friend class ProjectWidgetPrivate;

    //! Tab changed.
    void tabChanged(int index);

private:
    friend class ProjectWidgetPrivate;

    Q_DISABLE_COPY(ProjectWidget)

    std::unique_ptr<ProjectWidgetPrivate> d;
}; // class ProjectWidget

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__PROJECT_WIDGET_HPP__INCLUDED
