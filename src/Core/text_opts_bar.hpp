
/*
    SPDX-FileCopyrightText: 2026 Igor Mironchik <igor.mironchik@gmail.com>
    SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef PROTOTYPER__CORE__TEXT_OPS_HPP__INCLUDED
#define PROTOTYPER__CORE__TEXT_OPS_HPP__INCLUDED

// Qt include.
#include <QToolBar>

// C++ include.
#include <memory>

QT_BEGIN_NAMESPACE
class QTextCursor;
QT_END_NAMESPACE

namespace Prototyper
{

namespace Core
{

//
// TextOptsBar
//

class TextOptsBarPrivate;

//! Text editor's tool bar.
class TextOptsBar final : public QToolBar
{
    Q_OBJECT

signals:
    //! Set font size.
    void setFontSize(int s);
    //! Bold.
    void bold(bool on);
    //! Italic.
    void italic(bool on);
    //! Underline.
    void underline(bool on);
    //! Text color.
    void textColor();
    //! Clear format.
    void clearFormat();
    //! Align left.
    void alignLeft();
    //! Align center.
    void alignCenter();
    //! Align right.
    void alignRight();
    //! Insert link.
    void insertLink();

public:
    //! Icon size.
    enum IconSize {
        //! Small.
        Small = 1,
        //! Large.
        Large = 2,
        //! Without link
        WithoutLink = Small
    }; // enum IconSize

    TextOptsBar(IconSize s = Large,
                QWidget *parent = 0);
    ~TextOptsBar() override;

public slots:
    //! Update state of the buttons.
    void updateState(const QTextCursor &cursor);
    //! Clear format.
    void slotClearFormat();

private:
    friend class TextOptsBarPrivate;

    Q_DISABLE_COPY(TextOptsBar)

    std::unique_ptr<TextOptsBarPrivate> d;
}; // class TextOptsBar

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__TEXT_OPS_HPP__INCLUDED
