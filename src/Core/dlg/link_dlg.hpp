
/*
    SPDX-FileCopyrightText: 2016-2024 Igor Mironchik <igor.mironchik@gmail.com>
    SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef PROTOTYPER__CORE__LINK_DLG_HPP__INCLUDED
#define PROTOTYPER__CORE__LINK_DLG_HPP__INCLUDED

// Qt include.
#include <QDialog>

// C++ include.
#include <memory>

namespace Prototyper
{

namespace Core
{

//
// LinkDlg
//

class LinkDlgPrivate;

//! Link dialog.
class LinkDlg final : public QDialog
{
    Q_OBJECT

public:
    LinkDlg(QWidget *parent = nullptr);
    ~LinkDlg() override;

    //! \return Url.
    QString url() const;
    //! \return Text.
    QString text() const;

private:
    Q_DISABLE_COPY(LinkDlg)

    std::unique_ptr<LinkDlgPrivate> d;
}; // class LinkDlg

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__LINK_DLG_HPP__INCLUDED
