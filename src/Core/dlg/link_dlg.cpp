
/*
    SPDX-FileCopyrightText: 2026 Igor Mironchik <igor.mironchik@gmail.com>
    SPDX-License-Identifier: GPL-3.0-or-later
*/

// Prototyper include.
#include "link_dlg.hpp"
#include "ui_link_dlg.h"

// Qt include.
#include <QLineEdit>

namespace Prototyper
{

namespace Core
{

//
// LinkDlgPrivate
//

class LinkDlgPrivate
{
public:
    LinkDlgPrivate(LinkDlg *parent)
        : q(parent)
    {
    }

    //! Parent.
    LinkDlg *q;
    //! Ui.
    Ui::LinkDlg m_ui;
}; // class LinkDlgPrivate

//
// LinkDlg
//

LinkDlg::LinkDlg(QWidget *parent)
    : QDialog(parent)
    , d(new LinkDlgPrivate(this))
{
    d->m_ui.setupUi(this);
}

LinkDlg::~LinkDlg() = default;

QString LinkDlg::url() const
{
    return d->m_ui.m_url->text();
}

QString LinkDlg::text() const
{
    return d->m_ui.m_text->text();
}

} /* namespace Core */

} /* namespace Prototyper */
