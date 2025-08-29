
/*
    SPDX-FileCopyrightText: 2025 Igor Mironchik <igor.mironchik@gmail.com>
    SPDX-License-Identifier: GPL-3.0-or-later
*/

// Prototyper include.
#include "tabs_list.hpp"
#include "project_widget.hpp"
#include "project_window.hpp"
#include "top_gui.hpp"

// Qt include.
#include <QAction>
#include <QKeyEvent>
#include <QMenu>
#include <QModelIndex>
#include <QStringListModel>

namespace Prototyper
{

namespace Core
{

//
// TabsModel
//

//! Model for the tabs list.
class TabsModel : public QStringListModel
{
public:
    explicit TabsModel(QObject *parent)
        : QStringListModel(parent)
    {
    }

    Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE
    {
        if (index.isValid()) {
            return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
        }

        return Qt::NoItemFlags;
    }

private:
    Q_DISABLE_COPY(TabsModel)
}; // class TabsModel

//
// TabsViewPrivate
//

class TabsViewPrivate
{
public:
    explicit TabsViewPrivate(TabsView *parent)
        : q(parent)
        , m_activate(nullptr)
    {
    }

    //! Init.
    void init();

    //! Parent.
    TabsView *q;
    //! Activate tab action.
    QAction *m_activate;
    //! Index under cursor.
    QModelIndex m_index;
}; // class TabsViewPrivate

void TabsViewPrivate::init()
{
    m_activate = new QAction(TabsView::tr("Activate Tab"), q);

    TabsView::connect(m_activate, &QAction::triggered, q, &TabsView::p_activateTab);
}

//
// TabsView
//

TabsView::TabsView(QWidget *parent)
    : QListView(parent)
    , d(new TabsViewPrivate(this))
{
    d->init();
}

TabsView::~TabsView() = default;

void TabsView::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        emit enterPressed(currentIndex());
    }

    QListView::keyPressEvent(event);
}

void TabsView::contextMenuEvent(QContextMenuEvent *event)
{
    d->m_index = indexAt(event->pos());

    QMenu menu;

    if (d->m_index.isValid()) {
        menu.addAction(d->m_activate);

        if (d->m_index.row() >= 0)
            menu.addAction(QIcon(QStringLiteral(":/Core/img/edit-rename.png")),
                           tr("Rename"),
                           this,
                           &TabsView::p_renameTab);

        menu.addAction(QIcon(QStringLiteral(":/Core/img/list-add.png")), tr("Add Page"), this, &TabsView::p_addForm);

        if (d->m_index.row() != 0) {
            menu.addAction(QIcon(QStringLiteral(":/Core/img/edit-delete.png")),
                           tr("Delete Form"),
                           this,
                           &TabsView::p_deleteForm);
        }
    } else {
        menu.addAction(QIcon(QStringLiteral(":/Core/img/list-add.png")), tr("Add Page"), this, &TabsView::p_addForm);
    }

    menu.exec(event->globalPos());

    event->accept();
}

void TabsView::p_activateTab()
{
    emit activateTab(d->m_index);
}

void TabsView::p_renameTab()
{
    TopGui::instance()->projectWindow()->projectWidget()->renameTab(d->m_index.data().toString());
}

void TabsView::p_addForm()
{
    TopGui::instance()->projectWindow()->projectWidget()->addPage();
}

void TabsView::p_deleteForm()
{
    TopGui::instance()->projectWindow()->projectWidget()->deletePage(d->m_index.data().toString());
}

//
// TabsListPrivate
//

class TabsListPrivate
{
public:
    explicit TabsListPrivate(TabsList *parent)
        : q(parent)
        , m_view(nullptr)
        , m_model(nullptr)
    {
    }

    //! Init.
    void init();

    //! Parent.
    TabsList *q;
    //! View.
    TabsView *m_view;
    //! Model.
    TabsModel *m_model;
}; // class TabsListPrivate

void TabsListPrivate::init()
{
    m_view = new TabsView(q);

    m_model = new TabsModel(q);

    m_view->setModel(m_model);

    q->setWindowTitle(TabsList::tr("Pages"));

    q->setWidget(m_view);

    TabsList::connect(m_view, &QListView::doubleClicked, q, &TabsList::p_activateTab);
    TabsList::connect(m_view, &TabsView::enterPressed, q, &TabsList::p_activateTab);
    TabsList::connect(m_view, &TabsView::activateTab, q, &TabsList::p_activateTab);
}

//
// TabsList
//

TabsList::TabsList(QWidget *parent,
                   Qt::WindowFlags f)
    : QDockWidget(parent,
                  f)
    , d(new TabsListPrivate(this))
{
    d->init();
}

TabsList::~TabsList() = default;

QStringListModel *TabsList::model() const
{
    return d->m_model;
}

void TabsList::p_activateTab(const QModelIndex &index)
{
    if (index.isValid()) {
        TopGui::instance()->projectWindow()->projectWidget()->activateTab(index.data().toString());
    }
}

} /* namespace Core */

} /* namespace Prototyper */
