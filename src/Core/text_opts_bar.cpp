
/*
    SPDX-FileCopyrightText: 2026 Igor Mironchik <igor.mironchik@gmail.com>
    SPDX-License-Identifier: GPL-3.0-or-later
*/

// Prototyper include.
#include "text_opts_bar.hpp"
#include "constants.hpp"
#include "form/utils.hpp"

// Qt include.
#include <QAction>
#include <QActionGroup>
#include <QComboBox>
#include <QIntValidator>
#include <QTextCharFormat>
#include <QTextCursor>

namespace Prototyper
{

namespace Core
{

//
// TextOptsBarPrivate
//

class TextOptsBarPrivate
{
public:
    TextOptsBarPrivate(TextOptsBar::IconSize s,
                       TextOptsBar *parent)
        : q(parent)
        , m_fontBold(nullptr)
        , m_fontItalic(nullptr)
        , m_fontUnderline(nullptr)
        , m_alignLeft(nullptr)
        , m_alignCenter(nullptr)
        , m_alignRight(nullptr)
        , m_link(nullptr)
        , m_iconSize(s)
        , m_fontSize(nullptr)
    {
    }

    //! Init.
    void init();

    //! Parent.
    TextOptsBar *q;
    //! Bold action.
    QAction *m_fontBold;
    //! Italic action.
    QAction *m_fontItalic;
    //! Underline action.
    QAction *m_fontUnderline;
    //! Align left.
    QAction *m_alignLeft;
    //! Align center.
    QAction *m_alignCenter;
    //! Align right.
    QAction *m_alignRight;
    //! Insert link.
    QAction *m_link;
    //! Icon size.
    TextOptsBar::IconSize m_iconSize;
    //! Font size.
    QComboBox *m_fontSize;
}; // class TextOptsBarPrivate;

void TextOptsBarPrivate::init()
{
    m_fontSize = new QComboBox(q);
    m_fontSize->addItems(QStringList()
                         << QString::number(8)
                         << QString::number(9)
                         << QString::number(10)
                         << QString::number(11)
                         << QString::number(12)
                         << QString::number(14)
                         << QString::number(16)
                         << QString::number(18)
                         << QString::number(20)
                         << QString::number(22)
                         << QString::number(24)
                         << QString::number(26)
                         << QString::number(28)
                         << QString::number(36)
                         << QString::number(48)
                         << QString::number(72));
    auto *v = new QIntValidator(1, 72 * 5, m_fontSize);
    m_fontSize->setValidator(v);
    m_fontSize->setEditable(true);
    const QString fontSizeTip = TextOptsBar::tr("Font Size");
    m_fontSize->setToolTip(fontSizeTip);

    TextOptsBar::connect(m_fontSize, &QComboBox::currentTextChanged, q, [this](const QString &txt) {
        emit q->setFontSize(txt.toInt());
    });

    q->addWidget(m_fontSize);

    q->addSeparator();

    const QString fontBoldTip = TextOptsBar::tr("Bold");
    m_fontBold = q->addAction((m_iconSize == TextOptsBar::Large
                                   ? QIcon(QStringLiteral(":/Core/img/format-text-bold.png"))
                                   : QIcon(QStringLiteral(":/Core/img/format-text-bold-small.png"))),
                              fontBoldTip);
    m_fontBold->setShortcut(TextOptsBar::tr("Ctrl+B"));
    m_fontBold->setToolTip(fontBoldTip);
    m_fontBold->setStatusTip(fontBoldTip);
    m_fontBold->setCheckable(true);

    const QString fontItalicTip = TextOptsBar::tr("Italic");
    m_fontItalic = q->addAction((m_iconSize == TextOptsBar::Large
                                     ? QIcon(QStringLiteral(":/Core/img/format-text-italic.png"))
                                     : QIcon(QStringLiteral(":/Core/img/format-text-italic-small.png"))),
                                fontItalicTip);
    m_fontItalic->setShortcut(TextOptsBar::tr("Ctrl+I"));
    m_fontItalic->setToolTip(fontItalicTip);
    m_fontItalic->setStatusTip(fontItalicTip);
    m_fontItalic->setCheckable(true);

    const QString fontUnderlineTip = TextOptsBar::tr("Underline");
    m_fontUnderline = q->addAction((m_iconSize == TextOptsBar::Large
                                        ? QIcon(QStringLiteral(":/Core/img/format-text-underline.png"))
                                        : QIcon(QStringLiteral(":/Core/img/format-text-underline-small.png"))),
                                   fontUnderlineTip);
    m_fontUnderline->setShortcut(TextOptsBar::tr("Ctrl+U"));
    m_fontUnderline->setToolTip(fontUnderlineTip);
    m_fontUnderline->setStatusTip(fontUnderlineTip);
    m_fontUnderline->setCheckable(true);

    auto *alignGroup = new QActionGroup(q);

    q->addSeparator();

    const QString alignLeftTip = TextOptsBar::tr("Align Left");
    m_alignLeft = q->addAction((m_iconSize == TextOptsBar::Large
                                    ? QIcon(QStringLiteral(":/Core/img/align-horizontal-left.png"))
                                    : QIcon(QStringLiteral(":/Core/img/align-horizontal-left-small.png"))),
                               alignLeftTip);
    m_alignLeft->setToolTip(alignLeftTip);
    m_alignLeft->setStatusTip(alignLeftTip);
    m_alignLeft->setCheckable(true);
    alignGroup->addAction(m_alignLeft);

    const QString alignCenterTip = TextOptsBar::tr("Align Center");
    m_alignCenter = q->addAction((m_iconSize == TextOptsBar::Large
                                      ? QIcon(QStringLiteral(":/Core/img/align-horizontal-center.png"))
                                      : QIcon(QStringLiteral(":/Core/img/align-horizontal-center-small.png"))),
                                 alignCenterTip);
    m_alignCenter->setToolTip(alignCenterTip);
    m_alignCenter->setStatusTip(alignCenterTip);
    m_alignCenter->setCheckable(true);
    alignGroup->addAction(m_alignCenter);

    const QString alignRightTip = TextOptsBar::tr("Align Right");
    m_alignRight = q->addAction((m_iconSize == TextOptsBar::Large
                                     ? QIcon(QStringLiteral(":/Core/img/align-horizontal-right.png"))
                                     : QIcon(QStringLiteral(":/Core/img/align-horizontal-right-small.png"))),
                                alignRightTip);
    m_alignRight->setToolTip(alignRightTip);
    m_alignRight->setStatusTip(alignRightTip);
    m_alignRight->setCheckable(true);
    alignGroup->addAction(m_alignRight);

    if (m_iconSize == TextOptsBar::Large) {
        q->addSeparator();

        const QString linkTip = TextOptsBar::tr("Insert Link");
        m_link = q->addAction(QIcon(QStringLiteral(":/Core/img/insert-link.png")), linkTip);
        m_link->setToolTip(linkTip);
        m_link->setStatusTip(linkTip);
        m_link->setCheckable(false);
    }

    q->addSeparator();

    const QString clearFormatTip = TextOptsBar::tr("Clear Format");
    QAction *clearFormat =
        q->addAction((m_iconSize == TextOptsBar::Large ? QIcon(QStringLiteral(":/Core/img/edit-clear.png"))
                                                       : QIcon(QStringLiteral(":/Core/img/edit-clear-small.png"))),
                     clearFormatTip);
    clearFormat->setToolTip(clearFormatTip);
    clearFormat->setStatusTip(clearFormatTip);

    TextOptsBar::connect(m_fontBold, &QAction::triggered, q, &TextOptsBar::bold);
    TextOptsBar::connect(m_fontItalic, &QAction::triggered, q, &TextOptsBar::italic);
    TextOptsBar::connect(m_fontUnderline, &QAction::triggered, q, &TextOptsBar::underline);
    TextOptsBar::connect(clearFormat, &QAction::triggered, q, &TextOptsBar::slotClearFormat);
    TextOptsBar::connect(m_alignLeft, &QAction::triggered, q, &TextOptsBar::alignLeft);
    TextOptsBar::connect(m_alignCenter, &QAction::triggered, q, &TextOptsBar::alignCenter);
    TextOptsBar::connect(m_alignRight, &QAction::triggered, q, &TextOptsBar::alignRight);

    if (m_link) {
        TextOptsBar::connect(m_link, &QAction::triggered, q, &TextOptsBar::insertLink);
    }
}

//
// TextOptsBar
//

TextOptsBar::TextOptsBar(IconSize s,
                         QWidget *parent)
    : QToolBar(parent)
    , d(new TextOptsBarPrivate(s,
                               this))
{
    d->init();
}

TextOptsBar::~TextOptsBar() = default;

void TextOptsBar::updateState(const QTextCursor &cursor)
{
    QTextCharFormat fmt = cursor.charFormat();

    if (cursor.hasSelection()) {
        QTextCursor c = cursor;

        if (c.position() != c.selectionEnd()) {
            c.setPosition(c.selectionEnd());
        }

        fmt = c.charFormat();
    }

    d->m_fontBold->setChecked(fmt.fontWeight() == QFont::Bold);
    d->m_fontItalic->setChecked(fmt.fontItalic());
    d->m_fontUnderline->setChecked(fmt.fontUnderline());

    disconnect(d->m_fontSize, &QComboBox::currentTextChanged, nullptr, nullptr);

    d->m_fontSize->setCurrentText(QString::number(qRound(MmPx::instance().toPtY(fmt.font().pixelSize()))));

    connect(d->m_fontSize, &QComboBox::currentTextChanged, this, [this](const QString &txt) {
        emit setFontSize(txt.toInt());
    });

    QTextBlockFormat b = cursor.blockFormat();

    switch (b.alignment()) {
    case Qt::AlignLeft: {
        d->m_alignLeft->setChecked(true);
    } break;

    case Qt::AlignCenter: {
        d->m_alignCenter->setChecked(true);
    } break;

    case Qt::AlignRight: {
        d->m_alignRight->setChecked(true);
    } break;

    default:
        break;
    }
}

void TextOptsBar::slotClearFormat()
{
    d->m_fontBold->setChecked(false);
    d->m_fontItalic->setChecked(false);
    d->m_fontUnderline->setChecked(false);
    d->m_fontSize->setCurrentText(QString::number(qRound(c_defaultFontSize)));

    d->m_alignLeft->setChecked(true);

    emit clearFormat();
}

} /* namespace Core */

} /* namespace Prototyper */
