
/*
    SPDX-FileCopyrightText: 2026 Igor Mironchik <igor.mironchik@gmail.com>
    SPDX-License-Identifier: GPL-3.0-or-later
*/

// Prototyper include.
#include "comment.hpp"
#include "../constants.hpp"
#include "../project_window.hpp"
#include "../top_gui.hpp"
#include "comments.hpp"
#include "utils.hpp"

// Qt include.
#include <QApplication>
#include <QFontMetrics>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>
#include <QPainter>

// C++ include.
#include <cmath>

namespace Prototyper
{

namespace Core
{

//
// PageCommentPrivate
//

class PageCommentPrivate
{
public:
    PageCommentPrivate(PageComment *parent)
        : q(parent)
        , m_id(0)
        , m_pressed(false)
        , m_isChanged(false)
        , m_distance(0)
    {
    }

    //! Parent.
    PageComment *q;
    //! Comments.
    QVector<QPair<QString, QString>> m_comments;
    //! Author.
    QString m_author;
    //! Id.
    int m_id;
    //! Mouse pos.
    QPointF m_pos;
    //! Mouse pressed.
    bool m_pressed;
    //! Is changed?
    bool m_isChanged;
    //! Mouse distance.
    int m_distance;
}; // class PageCommentPrivate

//
// PageComment
//

PageComment::PageComment(QGraphicsItem *parent)
    : QGraphicsObject(parent)
    , d(new PageCommentPrivate(this))
{
    connect(this, &PageComment::showComments, this, &PageComment::showCommentsImpl);

    setZValue(c_mostTopZValue - 1);
}

PageComment::~PageComment() = default;

Cfg::Comments PageComment::cfg() const
{
    Cfg::Comments c;

    c.set_id(d->m_id);

    Cfg::Point p;
    p.set_x(MmPx::instance().toMmX(pos().x()));
    p.set_y(MmPx::instance().toMmY(pos().y()));

    c.set_pos(p);

    for (const auto &s : std::as_const(d->m_comments)) {
        Cfg::Comment comment;
        comment.set_author(s.first);
        comment.set_text(s.second);

        c.comment().push_back(comment);
    }

    return c;
}

void PageComment::setCfg(const Cfg::Comments &c)
{
    setPos(QPointF(MmPx::instance().fromMmX(c.pos().x()), MmPx::instance().fromMmY(c.pos().y())));

    d->m_comments.clear();

    for (const auto &s : c.comment()) {
        d->m_comments.append(qMakePair(s.author(), s.text()));
    }

    d->m_id = c.id();

    update();
}

QString PageComment::author() const
{
    return d->m_author;
}

void PageComment::setAuthor(const QString &name)
{
    d->m_author = name;
}

void PageComment::setId(int id)
{
    d->m_id = id;

    scene()->update();
}

int PageComment::id() const
{
    return d->m_id;
}

bool PageComment::isItYou(const QPointF &point) const
{
    auto r = boundingRect();
    r.moveTopLeft(pos());

    return r.contains(point);
}

bool PageComment::isChanged() const
{
    return d->m_isChanged;
}

void PageComment::setChanged(bool on)
{
    d->m_isChanged = on;
}

QRectF PageComment::boundingRect() const
{
    QFontMetrics fm(QApplication::font());
    const auto r = fm.boundingRect(QString::number(d->m_id < 10 ? 10 : d->m_id));
    const qreal diameter = std::sqrt(r.width() * r.width() + r.height() * r.height()) + 8.0;

    return QRectF(0.0, 0.0, diameter, diameter);
}

void PageComment::paint(QPainter *painter,
                        const QStyleOptionGraphicsItem *,
                        QWidget *)
{
    painter->setFont(QApplication::font());
    painter->setPen(QColor(33, 122, 255));
    painter->setBrush(QColor(33, 122, 255));
    painter->drawEllipse(boundingRect());
    painter->setPen(Qt::white);
    painter->drawEllipse(boundingRect().adjusted(3, 3, -3, -3));
    painter->drawText(boundingRect(), Qt::AlignCenter, QString::number(d->m_id));
}

void PageComment::mousePressEvent(QGraphicsSceneMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        d->m_pressed = true;

        d->m_pos = e->pos();

        e->accept();
    } else {
        e->ignore();
    }
}

void PageComment::mouseMoveEvent(QGraphicsSceneMouseEvent *e)
{
    if (d->m_pressed) {
        d->m_distance += qRound((d->m_pos - e->pos()).manhattanLength());

        setPos(pos() - d->m_pos + e->pos());

        e->accept();
    } else {
        e->ignore();
    }
}

void PageComment::mouseReleaseEvent(QGraphicsSceneMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        d->m_pressed = false;

        if (d->m_distance < 5) {
            emit showComments();
        } else {
            emit changed();
        }

        d->m_distance = 0;

        e->accept();
    } else {
        e->ignore();
    }
}

void PageComment::showCommentsImpl()
{
    Comments dlg(d->m_author, d->m_comments, scene()->views().first());

    dlg.exec();

    if (d->m_comments != dlg.comments()) {
        setChanged();

        emit changed();

        d->m_comments = dlg.comments();
    }

    if (d->m_author != dlg.author()) {
        d->m_author = dlg.author();

        TopGui::instance()->projectWindow()->setAuthor(d->m_author);
    }
}

} /* namespace Core */

} /* namespace Prototyper */
