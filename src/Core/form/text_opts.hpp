
/*
    SPDX-FileCopyrightText: 2016-2024 Igor Mironchik <igor.mironchik@gmail.com>
    SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef PROTOTYPER__CORE__FORM_TEXT_OPTS_HPP__INCLUDED
#define PROTOTYPER__CORE__FORM_TEXT_OPTS_HPP__INCLUDED

// Qt include.
#include <QGraphicsProxyWidget>

// C++ include.
#include <memory>

namespace Prototyper
{

namespace Core
{

//
// FormTextOpts
//

class FormTextOptsPrivate;

//! Toolbar with text options on the form.
class FormTextOpts final : public QGraphicsProxyWidget
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

public:
    explicit FormTextOpts(QGraphicsItem *parent);
    ~FormTextOpts() override;

public slots:
    //! Update state of the buttons.
    void updateState(const QTextCursor &cursor);

private:
    Q_DISABLE_COPY(FormTextOpts)

    std::unique_ptr<FormTextOptsPrivate> d;
}; // class FormTextOpts

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__FORM_TEXT_OPTS_HPP__INCLUDED
