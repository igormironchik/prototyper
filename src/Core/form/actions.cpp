
/*
    SPDX-FileCopyrightText: 2016-2024 Igor Mironchik <igor.mironchik@gmail.com>
    SPDX-License-Identifier: GPL-3.0-or-later
*/

// Prototyper include.
#include "actions.hpp"
#include "page.hpp"

// Qt include.
#include <QApplication>
#include <QColor>

namespace Prototyper
{

namespace Core
{

//
// PageActionPrivate
//

class PageActionPrivate
{
public:
    explicit PageActionPrivate(PageAction *parent)
        : q(parent)
        , m_mode(PageAction::Select)
        , m_page(nullptr)
        , m_fillColor(Qt::transparent)
        , m_strokeColor(Qt::black)
        , m_snap(true)
    {
    }

    //! Parent.
    PageAction *q;
    //! Mode.
    PageAction::Mode m_mode;
    //! Page.
    Page *m_page;
    //! Fill color.
    QColor m_fillColor;
    //! Stroke color.
    QColor m_strokeColor;
    //! Is snap enabled?
    bool m_snap;
    //! Flags.
    PageAction::Flags m_flags;
}; // class FormActionPrivate

//
// PageAction
//

PageAction::PageAction()
    : d(new PageActionPrivate(this))
{
}

PageAction::~PageAction() = default;

static PageAction *pageActionInstance = nullptr;

void PageAction::cleanup()
{
    delete pageActionInstance;

    pageActionInstance = nullptr;
}

PageAction *PageAction::instance()
{
    if (!pageActionInstance) {
        pageActionInstance = new PageAction;

        qAddPostRoutine(&PageAction::cleanup);
    }

    return pageActionInstance;
}

const PageAction::Flags &PageAction::flags() const
{
    return d->m_flags;
}

void PageAction::setFlags(const Flags &f)
{
    d->m_flags = f;
}

void PageAction::setFlag(Flag f)
{
    d->m_flags |= f;
}

bool PageAction::testFlag(Flag f) const
{
    return d->m_flags.testFlag(f);
}

void PageAction::clearFlag(Flag f)
{
    d->m_flags &= ~Flags(f);
}

PageAction::Mode PageAction::mode() const
{
    return d->m_mode;
}

void PageAction::setMode(Mode m)
{
    d->m_mode = m;
}

Page *PageAction::page() const
{
    return d->m_page;
}

void PageAction::setPage(Page *f)
{
    d->m_page = f;
}

const QColor &PageAction::fillColor() const
{
    return d->m_fillColor;
}

void PageAction::setFillColor(const QColor &c)
{
    d->m_fillColor = c;
}

const QColor &PageAction::strokeColor() const
{
    return d->m_strokeColor;
}

void PageAction::setStrokeColor(const QColor &c)
{
    d->m_strokeColor = c;
}

bool PageAction::isSnapEnabled() const
{
    return d->m_snap;
}

void PageAction::enableSnap(bool on)
{
    d->m_snap = on;
}

} /* namespace Core */

} /*  namespace Prototyper */
