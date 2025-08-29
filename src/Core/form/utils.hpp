
/*
    SPDX-FileCopyrightText: 2025 Igor Mironchik <igor.mironchik@gmail.com>
    SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef PROTOTYPER__CORE__UTILS_HPP__INCLUDED
#define PROTOTYPER__CORE__UTILS_HPP__INCLUDED

// Qt include.
#include <QFont>
#include <QTextCursor>
#include <QtGlobal>

// Prototyper include.
#include "../constants.hpp"
#include "project_cfg.hpp"

QT_BEGIN_NAMESPACE
class QTextCharFormat;
class QTextCursor;
class QTextDocument;
class QTextBlockFormat;
class QGraphicsItem;
QT_END_NAMESPACE

namespace Prototyper
{

namespace Core
{

//
// minMaxZ
//

//! \return Min and max Z index.
QPair<qreal,
      qreal>
minMaxZ(const QList<QGraphicsItem *> &items);

//
// MmPx
//

class MmPx final
{
public:
    static const MmPx &instance();

    qreal toMmX(qreal px) const;
    qreal fromMmX(qreal mm) const;

    qreal toMmY(qreal px) const;
    qreal fromMmY(qreal mm) const;

    QSizeF a4() const;

    qreal fromMm(qreal mm,
                 qreal dpi) const;

    int fromPtY(qreal pt) const;
    qreal toPtY(int px) const;

    int fromPt(qreal pt,
               qreal dpi) const;

    qreal yDpi() const;

private:
    MmPx();

    qreal m_xdots;
    qreal m_ydots;
}; // class MmPx

namespace Cfg
{

static const QString c_boldStyle = QLatin1String("bold");
static const QString c_italicStyle = QLatin1String("italic");
static const QString c_underlineStyle = QLatin1String("underline");
static const QString c_normalStyle = QLatin1String("normal");
static const QString c_left = QLatin1String("left");
static const QString c_right = QLatin1String("right");
static const QString c_center = QLatin1String("center");
static const QString c_link = QLatin1String("link");

static const QString c_minimize = QLatin1String("minimize");
static const QString c_maximize = QLatin1String("maximize");
static const QString c_close = QLatin1String("close");

//
// textStyle
//

//! \return Text style for the configuration.
std::vector<QString> textStyle(const QTextCharFormat &f,
                               const QTextBlockFormat &b);

//
// text
//

//! \return Text for the configuration.
std::vector<Cfg::TextStyle> text(QTextCursor c,
                                 const QString &data);

//
// pen
//

Cfg::Pen pen(const QPen &p);

//
// fromPen
//

QPen fromPen(const Cfg::Pen &p);
QPen fromPen(const Cfg::Pen &p,
             qreal dpi);

//
// brush
//

Cfg::Brush brush(const QBrush &b);

//
// fromBrush
//

QBrush fromBrush(const Cfg::Brush &b);

//
// initBlockFormat
//

void initBlockFormat(QTextBlockFormat &b,
                     const Cfg::TextStyle &style);

//
// fillTextDocument
//

void fillTextDocument(QTextDocument *doc,
                      const std::vector<Cfg::TextStyle> &text,
                      qreal dpi = MmPx::instance().yDpi(),
                      qreal scale = 1.0);

//
// textStyleFromFont
//

TextStyle textStyleFromFont(const QFont &f);

} /* namespace Cfg */

//
// applyTextFormat
//

template<typename T>
void applyTextFormat(const Cfg::TextStyle &s,
                     T *editor)
{
    if (std::find(s.style().cbegin(), s.style().cend(), Cfg::c_normalStyle) != s.style().cend()) {
        editor->setFontWeight(QFont::Normal);
        editor->setFontItalic(false);
        editor->setFontUnderline(false);
    } else {
        if (std::find(s.style().cbegin(), s.style().cend(), Cfg::c_boldStyle) != s.style().cend()) {
            editor->setFontWeight(QFont::Bold);
        } else {
            editor->setFontWeight(QFont::Normal);
        }

        if (std::find(s.style().cbegin(), s.style().cend(), Cfg::c_italicStyle) != s.style().cend()) {
            editor->setFontItalic(true);
        } else {
            editor->setFontItalic(false);
        }

        if (std::find(s.style().cbegin(), s.style().cend(), Cfg::c_underlineStyle) != s.style().cend()) {
            editor->setFontUnderline(true);
        } else {
            editor->setFontUnderline(false);
        }
    }

    if (std::find(s.style().cbegin(), s.style().cend(), Cfg::c_left) != s.style().cend()) {
        editor->setAlignment(Qt::AlignLeft);
    } else if (std::find(s.style().cbegin(), s.style().cend(), Cfg::c_right) != s.style().cend()) {
        editor->setAlignment(Qt::AlignRight);
    } else if (std::find(s.style().cbegin(), s.style().cend(), Cfg::c_center) != s.style().cend()) {
        editor->setAlignment(Qt::AlignCenter);
    }

    QFont f = editor->currentFont();
    f.setPixelSize(MmPx::instance().fromPtY(s.fontSize()));
    editor->setCurrentFont(f);

    QTextCursor cursor = editor->textCursor();
    QTextCharFormat fmt = cursor.charFormat();
    fmt.setAnchor(false);
    fmt.setAnchorHref(QString());
    fmt.setForeground(QBrush(c_textColor));
    cursor.setCharFormat(fmt);
    editor->setTextCursor(cursor);
}

//
// initDefaultFont
//

template<typename T>
void initDefaultFont(T *editor)
{
    QTextCursor c = editor->textCursor();

    QTextCharFormat fmt = c.charFormat();
    QFont f = fmt.font();
    f.setPixelSize(MmPx::instance().fromPtY(c_defaultFontSize));
    fmt.setFont(f);

    c.setCharFormat(fmt);

    editor->setTextCursor(c);
    editor->setFont(f);
    editor->document()->setDefaultFont(f);
}

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__UTILS_HPP__INCLUDED
