
/*
    SPDX-FileCopyrightText: 2025 Igor Mironchik <igor.mironchik@gmail.com>
    SPDX-License-Identifier: GPL-3.0-or-later
*/

// Prototyper include.
#include "name_dlg.hpp"
#include "ui_name_dlg.h"

// Qt include.
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QPalette>
#include <QPushButton>

namespace Prototyper
{

namespace Core
{

//
// NameDlgPrivate
//

class NameDlgPrivate
{
public:
    NameDlgPrivate(const QStringList &names,
                   const QString &title,
                   const QString &oldName,
                   NameDlg *parent)
        : q(parent)
        , m_names(names)
        , m_title(title)
        , m_oldName(oldName)
    {
    }

    //! Init.
    void init();

    //! Parent.
    NameDlg *q;
    //! Ui.
    Ui::NameDlg m_ui;
    //! Names.
    const QStringList &m_names;
    //! Normal text color.
    QColor m_color;
    //! Title.
    QString m_title;
    //! Old name.
    QString m_oldName;
}; // class NameDlgPrivate

void NameDlgPrivate::init()
{
    m_ui.setupUi(q);

    q->setWindowTitle(m_title);

    m_color = m_ui.m_edit->palette().color(QPalette::Text);

    m_ui.m_edit->setText(m_oldName);
    m_ui.m_edit->selectAll();

    m_ui.m_btns->button(QDialogButtonBox::Ok)->setEnabled(false);

    NameDlg::connect(m_ui.m_edit, &QLineEdit::textChanged, q, &NameDlg::textChanged);
}

//
// NameDlg
//

NameDlg::NameDlg(const QStringList &names,
                 const QString &title,
                 const QString &oldName,
                 QWidget *parent,
                 Qt::WindowFlags f)
    : QDialog(parent,
              f)
    , d(new NameDlgPrivate(names,
                           title,
                           oldName,
                           this))
{
    d->init();
}

NameDlg::~NameDlg() = default;

QString NameDlg::name() const
{
    return d->m_ui.m_edit->text().simplified();
}

void NameDlg::textChanged(const QString &text)
{
    if (!text.simplified().isEmpty() && !d->m_names.contains(text.simplified())) {
        QPalette p = d->m_ui.m_edit->palette();
        p.setColor(QPalette::Text, d->m_color);
        d->m_ui.m_edit->setPalette(p);

        d->m_ui.m_btns->button(QDialogButtonBox::Ok)->setEnabled(true);
    } else {
        if (text.simplified().isEmpty()) {
            QPalette p = d->m_ui.m_edit->palette();
            p.setColor(QPalette::Text, d->m_color);
            d->m_ui.m_edit->setPalette(p);
        } else {
            QPalette p = d->m_ui.m_edit->palette();
            p.setColor(QPalette::Text, Qt::red);
            d->m_ui.m_edit->setPalette(p);
        }

        d->m_ui.m_btns->button(QDialogButtonBox::Ok)->setEnabled(false);
    }
}

} /* namespace Core */

} /* namespace Prototyper */
