
/*
    SPDX-FileCopyrightText: 2026 Igor Mironchik <igor.mironchik@gmail.com>
    SPDX-License-Identifier: GPL-3.0-or-later
*/

// Prototyper include.
#include "top_gui.hpp"
#include "project_widget.hpp"
#include "project_window.hpp"
#include "session_cfg.hpp"
#include "windows_cfg.hpp"

// Qt include.
#include <QApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QStandardPaths>
#include <QTextStream>

namespace Prototyper
{

namespace Core
{

static const QString c_appCfgFileName = QStringLiteral("/Prototyper.cfg");
static const QString c_appSessionCfgFileName = QStringLiteral("/Session.cfg");
static const int c_stateVersion = 2;

//
// TopGuiPrivate
//

class TopGuiPrivate
{
public:
    explicit TopGuiPrivate(TopGui *parent)
        : q(parent)
        , m_projectWindow(nullptr)
    {
    }

    ~TopGuiPrivate()
    {
        delete m_projectWindow;
    }

    //! Init.
    void init();
    //! Place windows by default.
    void placeByDefault();

    //! Parent.
    TopGui *q;
    //! Project window.
    ProjectWindow *m_projectWindow;
    //! App's windows cfg.
    QString m_appCfgFileName;
    //! App's session cfg.
    QString m_appSessionCfgFileName;
}; // class TopGuiPrivate

void TopGuiPrivate::init()
{
    m_projectWindow = new ProjectWindow;

    static const QString cfgFolder = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);

    m_appCfgFileName = cfgFolder + c_appCfgFileName;
    m_appSessionCfgFileName = cfgFolder + c_appSessionCfgFileName;

    if (!QFileInfo::exists(m_appCfgFileName)) {
        placeByDefault();
    } else {
        QFile file(m_appCfgFileName);

        if (file.open(QIODevice::ReadOnly)) {
            try {
                Cfg::tag_WindowsCfg<cfgfile::qstring_trait_t> tag;

                QTextStream stream(&file);

                cfgfile::read_cfgfile(tag, stream, m_appCfgFileName);

                file.close();

                Cfg::WindowsCfg cfg = tag.get_cfg();

                m_projectWindow->move(cfg.projectWindow().x(), cfg.projectWindow().y());
                m_projectWindow->resize(cfg.projectWindow().width(), cfg.projectWindow().height());

                if (cfg.projectWindow().isMaximized()) {
                    m_projectWindow->showMaximized();
                }

                if (!cfg.state().isEmpty()) {
                    m_projectWindow->restoreState(QByteArray::fromBase64(cfg.state().toLatin1()), c_stateVersion);
                }
            } catch (const cfgfile::exception_t<cfgfile::qstring_trait_t> &) {
                file.close();

                placeByDefault();
            }
        } else {
            placeByDefault();
        }
    }

    QString projectFileName;
    QString author;

    QFile file(m_appSessionCfgFileName);

    if (file.open(QIODevice::ReadOnly)) {
        try {
            Cfg::tag_Session<cfgfile::qstring_trait_t> tag;

            QTextStream stream(&file);

            cfgfile::read_cfgfile(tag, stream, m_appSessionCfgFileName);

            file.close();

            projectFileName = tag.get_cfg().project();
            author = tag.get_cfg().author();
        } catch (const cfgfile::exception_t<cfgfile::qstring_trait_t> &) {
            file.close();
        }
    }

    if (!author.isEmpty()) {
        m_projectWindow->setAuthor(author);
    }

    if (!projectFileName.isEmpty() && QFileInfo::exists(projectFileName)) {
        m_projectWindow->readProject(projectFileName);
    }
}

void TopGuiPrivate::placeByDefault()
{
    m_projectWindow->showMaximized();
}

//
// TopGui
//

TopGui::TopGui()
    : d(new TopGuiPrivate(this))
{
}

TopGui::~TopGui() = default;

static TopGui *topGuiInstance = nullptr;

void TopGui::cleanup()
{
    if (topGuiInstance) {
        delete topGuiInstance;

        topGuiInstance = nullptr;
    }
}

TopGui *TopGui::instance()
{
    if (!topGuiInstance) {
        topGuiInstance = new TopGui;

        topGuiInstance->d->init();

        qAddPostRoutine(&TopGui::cleanup);
    }

    return topGuiInstance;
}

ProjectWindow *TopGui::projectWindow()
{
    return d->m_projectWindow;
}

void TopGui::saveCfg(QWidget *parent)
{
    QFileInfo info(d->m_appCfgFileName);

    QDir dir(info.path());

    if (!dir.exists()) {
        if (!dir.mkpath(info.path())) {
            if (parent) {
                QMessageBox::warning(parent,
                                     tr("Unable to create folder..."),
                                     tr("Unable to create folder for the configuration files. "
                                        "Path: \"%1\".")
                                         .arg(info.path()));
            }

            return;
        }
    }

    {
        QFile file(d->m_appCfgFileName);

        if (file.open(QIODevice::WriteOnly)) {
            try {
                Cfg::WindowsCfg cfg;

                Cfg::Window proj;
                proj.set_x(d->m_projectWindow->x());
                proj.set_y(d->m_projectWindow->y());
                proj.set_width(d->m_projectWindow->width());
                proj.set_height(d->m_projectWindow->height());
                proj.set_isShown(!d->m_projectWindow->isMinimized());
                proj.set_isMaximized(d->m_projectWindow->isMaximized());

                cfg.set_projectWindow(proj);

                cfg.set_state(QString(d->m_projectWindow->saveState(c_stateVersion).toBase64()));

                Cfg::tag_WindowsCfg<cfgfile::qstring_trait_t> tag(cfg);

                QTextStream stream(&file);

                cfgfile::write_cfgfile(tag, stream);

                file.close();
            } catch (const cfgfile::exception_t<cfgfile::qstring_trait_t> &x) {
                file.close();

                if (parent) {
                    QMessageBox::warning(parent,
                                         tr("Unable to Save Configuration..."),
                                         tr("Unable to save configuration.\n"
                                            "%1")
                                             .arg(x.desc()));
                }
            }
        } else if (parent) {
            QMessageBox::warning(parent,
                                 tr("Unable to Save Configuration..."),
                                 tr("Unable to save configuration.\n"
                                    "Unable to open file."));
        }
    }

    QFile file(d->m_appSessionCfgFileName);

    if (file.open(QIODevice::WriteOnly)) {
        try {
            Cfg::Session s;
            s.set_project(d->m_projectWindow->projectFileName());
            s.set_author(d->m_projectWindow->author());

            Cfg::tag_Session<cfgfile::qstring_trait_t> tag(s);

            QTextStream stream(&file);

            cfgfile::write_cfgfile(tag, stream);

            file.close();
        } catch (const cfgfile::exception_t<cfgfile::qstring_trait_t> &x) {
            file.close();

            if (parent) {
                QMessageBox::warning(parent,
                                     tr("Unable to Save Session..."),
                                     tr("Unable to save session.\n"
                                        "%1")
                                         .arg(x.desc()));
            }
        }
    } else if (parent) {
        QMessageBox::warning(parent,
                             tr("Unable to Save Session..."),
                             tr("Unable to save session.\n"
                                "Unable to open file."));
    }
}

} /* namespace Core */

} /* namespace Prototyper */
