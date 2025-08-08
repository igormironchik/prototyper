
/*
    SPDX-FileCopyrightText: 2016-2024 Igor Mironchik <igor.mironchik@gmail.com>
    SPDX-License-Identifier: GPL-3.0-or-later
*/

// Prototyper include.
#include "text_editor.hpp"
#include "constants.hpp"
#include "dlg/link_dlg.hpp"
#include "form/utils.hpp"

#include <QDesktopServices>
#include <QMouseEvent>
#include <QTextBlockFormat>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextDocumentFragment>

namespace Prototyper
{

namespace Core
{

//
// TextEditorPrivate
//

class TextEditorPrivate
{
public:
    explicit TextEditorPrivate(TextEditor *parent)
        : q(parent)
    {
    }

    //! Init.
    void init();

    //! Parent.
    TextEditor *q;
    //! Link pressed.
    bool linkPressed = false;
}; // class TextEditorPrivate;

void TextEditorPrivate::init()
{
    q->setUndoRedoEnabled(true);

    {
        QFont f = q->font();
        f.setPixelSize(MmPx::instance().fromPtY(c_defaultFontSize));
        q->setFont(f);
    }

    initDefaultFont(q);

    q->setAutoFormatting(QTextEdit::AutoNone);
    q->setTextInteractionFlags(Qt::TextEditorInteraction | Qt::TextBrowserInteraction);

    TextEditor::connect(q, &QTextEdit::cursorPositionChanged, q, &TextEditor::slotCursorChanged);
    TextEditor::connect(q, &QTextEdit::textChanged, q, &TextEditor::changed);
}

//
// TextEditor
//

TextEditor::TextEditor(QWidget *parent)
    : QTextEdit(parent)
    , d(new TextEditorPrivate(this))
{
    d->init();
}

TextEditor::~TextEditor() = default;

std::vector<Cfg::TextStyle> TextEditor::text() const
{
    return Cfg::text(textCursor(), toPlainText());
}

void TextEditor::setText(const std::vector<Cfg::TextStyle> &blocks)
{
    disconnect(this, &QTextEdit::textChanged, this, Q_NULLPTR);

    auto *doc = new QTextDocument(this);
    Cfg::fillTextDocument(doc, blocks);

    setDocument(doc);

    document()->setTextWidth(width());
    document()->clearUndoRedoStacks();

    initDefaultFont(this);

    connect(this, &QTextEdit::textChanged, this, &TextEditor::changed);
}

void TextEditor::setFontSize(int s)
{
    QTextCursor c = textCursor();

    if (c.hasSelection()) {
        if (c.position() != c.selectionEnd()) {
            c.setPosition(c.selectionEnd());
        }

        QTextCharFormat fmt = c.charFormat();

        QFont f = fmt.font();
        f.setPixelSize(MmPx::instance().fromPtY(s));
        fmt.setFont(f);

        textCursor().setCharFormat(fmt);
    } else {
        QFont f = currentFont();
        f.setPixelSize(MmPx::instance().fromPtY(s));
        setCurrentFont(f);
    }

    emit changed();
}

void TextEditor::bold(bool on)
{
    QTextCursor c = textCursor();

    if (c.hasSelection()) {
        if (c.position() != c.selectionEnd()) {
            c.setPosition(c.selectionEnd());
        }

        QTextCharFormat fmt = c.charFormat();

        fmt.setFontWeight(on ? QFont::Bold : QFont::Normal);

        textCursor().setCharFormat(fmt);
    } else {
        setFontWeight(on ? QFont::Bold : QFont::Normal);
    }

    emit changed();
}

void TextEditor::italic(bool on)
{
    QTextCursor c = textCursor();

    if (c.hasSelection()) {
        if (c.position() != c.selectionEnd()) {
            c.setPosition(c.selectionEnd());
        }

        QTextCharFormat fmt = c.charFormat();

        fmt.setFontItalic(on);

        textCursor().setCharFormat(fmt);
    } else {
        setFontItalic(on);
    }

    emit changed();
}

void TextEditor::underline(bool on)
{
    QTextCursor c = textCursor();

    if (c.hasSelection()) {
        if (c.position() != c.selectionEnd()) {
            c.setPosition(c.selectionEnd());
        }

        QTextCharFormat fmt = c.charFormat();

        fmt.setFontUnderline(on);

        textCursor().setCharFormat(fmt);
    } else {
        setFontUnderline(on);
    }

    emit changed();
}

void TextEditor::changeTextColor()
{
    emit changed();
}

void TextEditor::slotCursorChanged()
{
    emit cursorChanged(textCursor());
}

void TextEditor::clearFormat()
{
    QTextCursor c = textCursor();

    if (c.hasSelection()) {
        QTextCharFormat fmt = c.charFormat();

        fmt.setFontUnderline(false);
        fmt.setFontItalic(false);
        fmt.setFontWeight(QFont::Normal);
        fmt.clearBackground();
        fmt.clearForeground();
        QFont f = fmt.font();
        f.setPixelSize(MmPx::instance().fromPtY(c_defaultFontSize));
        fmt.setFont(f);
        fmt.setAnchor(false);
        fmt.setAnchorHref(QString());
        fmt.setForeground(QBrush(QColor(Qt::black)));

        textCursor().setCharFormat(fmt);
    } else {
        setFontUnderline(false);
        setFontItalic(false);
        setFontWeight(QFont::Normal);
        setTextColor(Qt::black);
        QFont f = currentFont();
        f.setPixelSize(MmPx::instance().fromPtY(c_defaultFontSize));
        setCurrentFont(f);
        QTextCursor cursor = textCursor();
        QTextCharFormat fmt = cursor.charFormat();
        fmt.setAnchor(false);
        fmt.setAnchorHref(QString());
        fmt.setForeground(QBrush(QColor(Qt::black)));
        cursor.setCharFormat(fmt);
        setTextCursor(cursor);
    }

    emit changed();
}

void TextEditor::reset()
{
    clear();

    clearFormat();
}

void TextEditor::alignLeft()
{
    QTextCursor c = textCursor();

    if (c.hasSelection()) {
        QTextBlockFormat b = c.blockFormat();
        b.setAlignment(Qt::AlignLeft);
        c.setBlockFormat(b);
        setTextCursor(c);
    } else {
        setAlignment(Qt::AlignLeft);
    }
}

void TextEditor::alignCenter()
{
    QTextCursor c = textCursor();

    if (c.hasSelection()) {
        QTextBlockFormat b = c.blockFormat();
        b.setAlignment(Qt::AlignCenter);
        c.setBlockFormat(b);
        setTextCursor(c);
    } else {
        setAlignment(Qt::AlignCenter);
    }
}

void TextEditor::alignRight()
{
    QTextCursor c = textCursor();

    if (c.hasSelection()) {
        QTextBlockFormat b = c.blockFormat();
        b.setAlignment(Qt::AlignRight);
        c.setBlockFormat(b);
        setTextCursor(c);
    } else {
        setAlignment(Qt::AlignRight);
    }
}

void TextEditor::insertLink()
{
    LinkDlg dlg(this);

    if (dlg.exec() == QDialog::Accepted) {
        QTextCursor c = textCursor();
        QTextCharFormat fmt = c.charFormat();
        fmt.setAnchor(true);
        fmt.setAnchorHref(dlg.url());
        fmt.setForeground(QBrush(c_linkColor));

        c.setCharFormat(fmt);

        c.insertText(dlg.text());
    }
}

void TextEditor::mousePressEvent(QMouseEvent *e)
{
    QTextEdit::mousePressEvent(e);

    if (e->button() == Qt::LeftButton) {
        if (!textCursor().charFormat().anchorHref().isEmpty()) {
            d->linkPressed = true;
        }
    }
}

void TextEditor::mouseReleaseEvent(QMouseEvent *e)
{
    QTextEdit::mouseReleaseEvent(e);

    if (e->button() == Qt::LeftButton) {
        if (d->linkPressed) {
            d->linkPressed = false;

            const auto f = textCursor().charFormat();

            if (!f.anchorHref().isEmpty() && e->modifiers() == Qt::ControlModifier) {
                QDesktopServices::openUrl(QUrl(f.anchorHref()));
            }
        }
    }
}

} /* namespace Core */

} /* namespace Prototyper */
