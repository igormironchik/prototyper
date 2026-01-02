
/*
    SPDX-FileCopyrightText: 2026 Igor Mironchik <igor.mironchik@gmail.com>
    SPDX-License-Identifier: GPL-3.0-or-later
*/

// Qt include.
#include <QApplication>
#include <QIcon>
#include <QTranslator>

// Prototyper include.
#include <Core/project_window.hpp>
#include <Core/top_gui.hpp>

using namespace Prototyper::Core;

int main(int argc,
         char **argv)
{
    QApplication app(argc, argv);

    QIcon appIcon(QStringLiteral(":/img/Prototyper_256x256.png"));
    appIcon.addFile(QStringLiteral(":/img/Prototyper_128x128.png"));
    appIcon.addFile(QStringLiteral(":/img/Prototyper_64x64.png"));
    appIcon.addFile(QStringLiteral(":/img/Prototyper_48x48.png"));
    appIcon.addFile(QStringLiteral(":/img/Prototyper_32x32.png"));
    appIcon.addFile(QStringLiteral(":/img/Prototyper_22x22.png"));
    appIcon.addFile(QStringLiteral(":/img/Prototyper_16x16.png"));
    QApplication::setWindowIcon(appIcon);

    QTranslator appTranslator;
    if (appTranslator.load("./tr/Prototyper_" + QLocale::system().name())) {
        QApplication::installTranslator(&appTranslator);
    }

    TopGui::instance()->projectWindow()->show();

    return QApplication::exec();
}
