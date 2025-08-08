
/*
    SPDX-FileCopyrightText: 2016-2024 Igor Mironchik <igor.mironchik@gmail.com>
    SPDX-License-Identifier: GPL-3.0-or-later
*/

// Prototyper include.
#include "project_widget_tab_bar.hpp"

// Qt include.
#include <QContextMenuEvent>
#include <QMenu>

namespace Prototyper
{

namespace Core
{

//
// ProjectTabBarPrivate
//

class ProjectTabBarPrivate
{
public:
    explicit ProjectTabBarPrivate(ProjectTabBar *parent)
        : q(parent)
        , m_menuTabIndex(0)
    {
    }

    //! Init.
    void init();

    //! Parent.
    ProjectTabBar *q;
    //! Menu tab index.
    int m_menuTabIndex;
}; // class ProjectTabBarPrivate

void ProjectTabBarPrivate::init()
{
}

//
// ProjectTabBar
//

ProjectTabBar::ProjectTabBar(QWidget *parent)
    : QTabBar(parent)
    , d(new ProjectTabBarPrivate(this))
{
    d->init();
}

ProjectTabBar::~ProjectTabBar() = default;

void ProjectTabBar::renameTab()
{
    emit formRenameRequest(tabText(d->m_menuTabIndex));
}

void ProjectTabBar::addPage()
{
    emit formAddRequest();
}

void ProjectTabBar::deleteForm()
{
    emit formDeleteRequest(tabText(d->m_menuTabIndex));
}

void ProjectTabBar::contextMenuEvent(QContextMenuEvent *event)
{
    d->m_menuTabIndex = tabAt(event->pos());

    QMenu menu;

    if (d->m_menuTabIndex >= 0) {
        menu.addAction(QIcon(QStringLiteral(":/Core/img/edit-rename.png")),
                       tr("Rename"),
                       this,
                       &ProjectTabBar::renameTab);
    }

    menu.addAction(QIcon(QStringLiteral(":/Core/img/list-add.png")), tr("Add Page"), this, &ProjectTabBar::addPage);

    if (d->m_menuTabIndex != 0) {
        menu.addAction(QIcon(QStringLiteral(":/Core/img/edit-delete.png")),
                       tr("Delete Form"),
                       this,
                       &ProjectTabBar::deleteForm);
    }

    menu.exec(event->globalPos());

    event->accept();
}

} /* namespace Core */

} /* namespace Prototyper */
