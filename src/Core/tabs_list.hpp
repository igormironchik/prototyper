
/*
    SPDX-FileCopyrightText: 2025 Igor Mironchik <igor.mironchik@gmail.com>
    SPDX-License-Identifier: GPL-3.0-or-later
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

namespace Prototyper
{

namespace Core
{

//
// TabsView
//

class TabsViewPrivate;

//! Tabs list view.
class TabsView final : public QListView
{
    Q_OBJECT

signals:
    //! Enter button pressed.
    void enterPressed(const QModelIndex &);
    //! Activate tab.
    void activateTab(const QModelIndex &);

public:
    explicit TabsView(QWidget *parent);
    ~TabsView() override;

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;

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

    Q_DISABLE_COPY(TabsView)

    std::unique_ptr<TabsViewPrivate> d;
}; // class TabsView

//
// TabsList
//

class TabsListPrivate;

//! Tabs list dock widget.
class TabsList final : public QDockWidget
{
    Q_OBJECT

public:
    TabsList(QWidget *parent = 0,
             Qt::WindowFlags f = Qt::WindowFlags());
    ~TabsList() override;

    //! \return Model.
    QStringListModel *model() const;

private slots:
    void p_activateTab(const QModelIndex &index);

private:
    friend class TabsListPrivate;

    Q_DISABLE_COPY(TabsList)

    std::unique_ptr<TabsListPrivate> d;
}; // class TabsList

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__TABS_LIST_HPP__INCLUDED
