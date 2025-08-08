
/*
    SPDX-FileCopyrightText: 2016-2024 Igor Mironchik <igor.mironchik@gmail.com>
    SPDX-License-Identifier: GPL-3.0-or-later
*/

// Prototyper include.
#include "text_opts.hpp"
#include "../text_opts_bar.hpp"

namespace Prototyper
{

namespace Core
{

//
// FormTextOptsPrivate
//

class FormTextOptsPrivate
{
public:
    explicit FormTextOptsPrivate(FormTextOpts *parent)
        : q(parent)
        , m_bar(nullptr)
    {
    }

    //! Init.
    void init();

    //! Parent.
    FormTextOpts *q;
    //! Toolbar.
    TextOptsBar *m_bar;
}; // class FormTextOptsPrivate

void FormTextOptsPrivate::init()
{
    m_bar = new TextOptsBar(TextOptsBar::Small);

    q->setWidget(m_bar);

    FormTextOpts::connect(m_bar, &TextOptsBar::setFontSize, q, &FormTextOpts::setFontSize);
    FormTextOpts::connect(m_bar, &TextOptsBar::bold, q, &FormTextOpts::bold);
    FormTextOpts::connect(m_bar, &TextOptsBar::italic, q, &FormTextOpts::italic);
    FormTextOpts::connect(m_bar, &TextOptsBar::underline, q, &FormTextOpts::underline);
    FormTextOpts::connect(m_bar, &TextOptsBar::textColor, q, &FormTextOpts::textColor);
    FormTextOpts::connect(m_bar, &TextOptsBar::clearFormat, q, &FormTextOpts::clearFormat);
    FormTextOpts::connect(m_bar, &TextOptsBar::alignLeft, q, &FormTextOpts::alignLeft);
    FormTextOpts::connect(m_bar, &TextOptsBar::alignCenter, q, &FormTextOpts::alignCenter);
    FormTextOpts::connect(m_bar, &TextOptsBar::alignRight, q, &FormTextOpts::alignRight);
}

//
// FormTextOpts
//

FormTextOpts::FormTextOpts(QGraphicsItem *parent)
    : QGraphicsProxyWidget(parent)
    , d(new FormTextOptsPrivate(this))
{
    d->init();
}

FormTextOpts::~FormTextOpts() = default;

void FormTextOpts::updateState(const QTextCursor &cursor)
{
    d->m_bar->updateState(cursor);
}

} /* namespace Core */

} /* namespace Prototyper */
