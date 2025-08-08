
/*
    SPDX-FileCopyrightText: 2016-2024 Igor Mironchik <igor.mironchik@gmail.com>
    SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef PROTOTYPER__CORE__TOP_GUI_HPP__INCLUDED
#define PROTOTYPER__CORE__TOP_GUI_HPP__INCLUDED

// Qt include.
#include <QObject>

// C++ include.
#include <memory>

// Prototyper include.
#include "export.hpp"

namespace Prototyper
{

namespace Core
{

class ProjectWindow;

//
// TopGui
//

class TopGuiPrivate;

//! Singleton for top GUI components.
class PROTOTYPER_CORE_EXPORT TopGui final : public QObject
{
    Q_OBJECT

private:
    TopGui();
    ~TopGui();

    static void cleanup();

public:
    //! \return Instance of top GUI.
    static TopGui *instance();

    //! \return Project window.
    ProjectWindow *projectWindow();

    //! Save cfg.
    void saveCfg(QWidget *parent = 0);

private:
    Q_DISABLE_COPY(TopGui)

    std::unique_ptr<TopGuiPrivate> d;
}; // class TopGui

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__TOP_GUI_HPP__INCLUDED
