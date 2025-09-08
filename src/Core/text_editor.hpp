
/*
    SPDX-FileCopyrightText: 2025 Igor Mironchik <igor.mironchik@gmail.com>
    SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef PROTOTYPER__CORE__TEXT_EDITOR_HPP__INCLUDED
#define PROTOTYPER__CORE__TEXT_EDITOR_HPP__INCLUDED

// Qt include.
#include <QTextEdit>

// C++ include.
#include <memory>

// Prototyper include.
#include "project_cfg.hpp"

namespace Prototyper
{

namespace Core
{

//
// TextEditor
//

class TextEditorPrivate;

//! Text editor.
class TextEditor final : public QTextEdit
{
    Q_OBJECT

signals:
    //! Cursor changed.
    void cursorChanged(const QTextCursor &);
    //! Project has been modified.
    void changed();

public:
    explicit TextEditor(QWidget *parent = 0);
    ~TextEditor() override;

    //! \return Text.
    std::vector<Cfg::TextStyle> text() const;
    //! Set text.
    void setText(const std::vector<Cfg::TextStyle> &blocks);

public slots:
    //! Set font size.
    void setFontSize(int s);
    //! Bold.
    void bold(bool on);
    //! Italic.
    void italic(bool on);
    //! Underline.
    void underline(bool on);
    //! Change text color.
    void changeTextColor();
    //! Clear format.
    void clearFormat();
    //! Reset state.
    void reset();
    //! Align left.
    void alignLeft();
    //! Align center.
    void alignCenter();
    //! Align right.
    void alignRight();
    //! Insert link.
    void insertLink();

private slots:
    //! Cursor changed.
    void slotCursorChanged();

protected:
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void keyPressEvent(QKeyEvent *e) override;

private:
    friend class TextEditorPrivate;

    Q_DISABLE_COPY(TextEditor)

    std::unique_ptr<TextEditorPrivate> d;
}; // class TextEditor

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__TEXT_EDITOR_HPP__INCLUDED
