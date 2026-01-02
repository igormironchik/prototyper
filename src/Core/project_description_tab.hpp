
/*
    SPDX-FileCopyrightText: 2026 Igor Mironchik <igor.mironchik@gmail.com>
    SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef PROTOTYPER__CORE__PROJECT_DESCRIPTION_TAB_HPP__INCLUDED
#define PROTOTYPER__CORE__PROJECT_DESCRIPTION_TAB_HPP__INCLUDED

// Qt include.
#include <QWidget>

// C++ include.
#include <memory>

QT_BEGIN_NAMESPACE
class QToolBar;
QT_END_NAMESPACE

namespace Prototyper
{

namespace Core
{

class TextEditor;

//
// ProjectDescTab
//

class ProjectDescTabPrivate;

//! Tab with text description of the project.
class ProjectDescTab final : public QWidget
{
    Q_OBJECT

public:
    ProjectDescTab(QWidget *parent = 0,
                   Qt::WindowFlags f = Qt::WindowFlags());
    ~ProjectDescTab() override;

    //! \return Tool bar.
    QToolBar *toolBar() const;
    //! \return Text editor.
    TextEditor *editor() const;

private:
    Q_DISABLE_COPY(ProjectDescTab)

    std::unique_ptr<ProjectDescTabPrivate> d;
}; // class ProjectDescTab

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__PROJECT_DESCRIPTION_TAB_HPP__INCLUDED
