
/*
    SPDX-FileCopyrightText: 2025 Igor Mironchik <igor.mironchik@gmail.com>
    SPDX-License-Identifier: GPL-3.0-or-later
*/

// Prototyper include.
#include "spinbox.hpp"
#include "../constants.hpp"
#include "actions.hpp"
#include "page.hpp"
#include "properties/object_properties.hpp"
#include "properties/spinbox_properties.hpp"
#include "properties/text_style_properties.hpp"
#include "properties/ui_object_properties.h"
#include "undo_commands.hpp"
#include "utils.hpp"

// Qt include.
#include <QAction>
#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsSceneContextMenuEvent>
#include <QMenu>
#include <QPainter>
#include <QPainterPath>
#include <QSpacerItem>
#include <QUndoStack>
#include <QVBoxLayout>

namespace Prototyper
{

namespace Core
{

//
// FormSpinBoxPrivate
//

class FormSpinBoxPrivate
{
public:
    FormSpinBoxPrivate(FormSpinBox *parent,
                       const QRectF &rect)
        : q(parent)
        , m_rect(QRectF(rect.x(),
                        rect.y(),
                        rect.width(),
                        FormSpinBox::boxHeight()))
        , m_proxy(nullptr)
        , m_text(FormSpinBox::tr("1"))
        , m_properties(nullptr)
    {
    }

    //! Init.
    void init();
    //! Set rect.
    void setRect(const QRectF &rect);
    //! Connect properties signals.
    void connectProperties();
    //! Disconnect properties signals.
    void disconnectProperties();

    //! Parent.
    FormSpinBox *q;
    //! Rect.
    QRectF m_rect;
    //! Resizable proxy.
    std::unique_ptr<FormResizableProxy> m_proxy;
    //! Font.
    QFont m_font;
    //! Text.
    QString m_text;
    //! Properties.
    QPointer<QWidget> m_properties;
    //! Object properties.
    QPointer<ObjectProperties> m_objProps;
    //! Spinbox properties.
    QPointer<SpinBoxProperties> m_spinProps;
    //! Text style properties.
    QPointer<TextStyleProperties> m_textStyleProps;
}; // class FormSpinBoxPrivate

void FormSpinBoxPrivate::init()
{
    m_proxy.reset(new FormResizableProxy(q, q->parentItem(), q->page()));

    setRect(m_rect);

    m_proxy->setMinSize(q->defaultSize());

    m_font = QApplication::font();

    m_font.setPixelSize(MmPx::instance().fromPtY(c_defaultFontSize));

    q->setObjectPen(QPen(PageAction::instance()->strokeColor()), false);

    q->setObjectBrush(QBrush(PageAction::instance()->fillColor()), false);
}

void FormSpinBoxPrivate::setRect(const QRectF &rect)
{
    m_rect = rect;

    q->setPos(m_rect.topLeft());

    m_proxy->setRect(m_rect);

    m_rect.moveTopLeft(QPointF(0.0, 0.0));
}

void FormSpinBoxPrivate::connectProperties()
{
    if (m_properties) {
        m_objProps->connectProperties();
        m_spinProps->connectProperties(this);
        m_textStyleProps->connectProperties(this);
    }
}

void FormSpinBoxPrivate::disconnectProperties()
{
    if (m_properties) {
        m_objProps->disconnectProperties();
        m_spinProps->disconnectProperties();
        m_textStyleProps->disconnectProperties();
    }
}

//
// FormSpinBox
//

FormSpinBox::FormSpinBox(const QRectF &rect,
                         Page *page,
                         QGraphicsItem *parent)
    : QGraphicsObject(parent)
    , FormObject(FormObject::SpinBoxType,
                 page)
    , d(new FormSpinBoxPrivate(this,
                               rect))
{
    d->init();
}

FormSpinBox::~FormSpinBox() = default;

void FormSpinBox::updatePropertiesValues()
{
    if (d->m_properties) {
        d->disconnectProperties();
        d->m_objProps->ui()->m_x->setValue(qRound(position().x()));
        d->m_objProps->ui()->m_y->setValue(qRound(position().y()));
        d->m_objProps->ui()->m_width->setValue(qRound(rectangle().width()));
        d->m_objProps->ui()->m_height->setValue(qRound(rectangle().height()));
        d->connectProperties();
    }
}

void FormSpinBox::paint(QPainter *painter,
                        const QStyleOptionGraphicsItem *option,
                        QWidget *widget)
{
    Q_UNUSED(widget)
    Q_UNUSED(option)

    draw(painter, d->m_rect, objectPen(), objectBrush(), d->m_font, d->m_text);

    if (isSelected() && !group()) {
        d->m_proxy->show();
    } else {
        d->m_proxy->hide();
    }
}

void FormSpinBox::draw(QPainter *painter,
                       const QRectF &rect,
                       const QPen &pen,
                       const QBrush &brush,
                       const QFont &font,
                       const QString &text,
                       int dpi)
{
    painter->setPen(pen);
    painter->setBrush(brush);

    QRectF r = rect;

    if (r.height() > boxHeight(dpi)) {
        r.setHeight(boxHeight(dpi));
        r.moveTopLeft(
            QPointF(rect.topLeft().x(), rect.topLeft().y() + (rect.height() - boxHeight(dpi)) / c_halfDivider));
    }

    const qreal ro = r.height() / 10.0;

    painter->drawRoundedRect(r, ro, ro);

    const qreal h = r.height();
    const qreal w = h * 0.75;
    const qreal o = w / 5.0;
    const qreal leftX = r.x() + r.width() - w;

    painter->drawLine(QLineF(leftX, r.y(), leftX, r.y() + h));

    QPainterPath top;
    top.moveTo(leftX + o, r.y() + r.height() / c_halfDivider - o / c_halfDivider);
    top.lineTo(leftX + w - o, r.y() + r.height() / c_halfDivider - o / c_halfDivider);
    top.lineTo(leftX + w / c_halfDivider, r.y() + o);
    top.lineTo(leftX + o, r.y() + r.height() / c_halfDivider - o / c_halfDivider);

    painter->setBrush(QBrush(pen.color()));

    painter->drawPath(top);

    QPainterPath bottom;
    bottom.moveTo(leftX + o, r.y() + r.height() / c_halfDivider + o / c_halfDivider);
    bottom.lineTo(leftX + w - o, r.y() + r.height() / c_halfDivider + o / c_halfDivider);
    bottom.lineTo(leftX + w / c_halfDivider, r.y() + h - o);
    bottom.lineTo(leftX + o, r.y() + r.height() / c_halfDivider + o / c_halfDivider);

    painter->drawPath(bottom);

    QRectF textR = r;
    textR.setRight(leftX - o);

    painter->setFont(font);

    painter->drawText(textR, Qt::AlignRight | Qt::AlignVCenter, text);
}

qreal FormSpinBox::boxHeight(int dpi)
{
    if (!dpi) {
        return MmPx::instance().fromMmY(4.0);
    }

    return MmPx::instance().fromMm(4.0, dpi);
}

void FormSpinBox::setObjectPen(const QPen &p,
                               bool pushUndoCommand)
{
    FormObject::setObjectPen(p, pushUndoCommand);

    update();
}

void FormSpinBox::setObjectBrush(const QBrush &b,
                                 bool pushUndoCommand)
{
    FormObject::setObjectBrush(b, pushUndoCommand);

    update();
}

Cfg::SpinBox FormSpinBox::cfg() const
{
    Cfg::SpinBox c;

    c.set_objectId(objectId());

    Cfg::Point p;
    p.set_x(MmPx::instance().toMmX(pos().x()));
    p.set_y(MmPx::instance().toMmY(pos().y()));

    c.set_pos(p);

    Cfg::Size s;
    s.set_width(MmPx::instance().toMmX(d->m_rect.width()));
    s.set_height(MmPx::instance().toMmY(d->m_rect.height()));

    c.set_size(s);

    c.set_pen(Cfg::pen(objectPen()));
    c.set_brush(Cfg::brush(objectBrush()));

    c.set_text(text());

    c.set_z(zValue());

    return c;
}

void FormSpinBox::setCfg(const Cfg::SpinBox &c)
{
    setObjectId(c.objectId());
    setObjectPen(Cfg::fromPen(c.pen()), false);
    setObjectBrush(Cfg::fromBrush(c.brush()), false);
    d->setRect(QRectF(MmPx::instance().fromMmX(c.pos().x()),
                      MmPx::instance().fromMmY(c.pos().y()),
                      MmPx::instance().fromMmX(c.size().width()),
                      MmPx::instance().fromMmY(c.size().height())));

    setText(c.text());

    setZValue(c.z());

    update();
}

Cfg::TextStyle FormSpinBox::text() const
{
    Cfg::TextStyle textStyle = Cfg::textStyleFromFont(d->m_font);
    textStyle.style().push_back(Cfg::c_right);
    textStyle.set_text(d->m_text);

    return textStyle;
}

void FormSpinBox::setText(const Cfg::TextStyle &c)
{
    if (std::find(c.style().cbegin(), c.style().cend(), Cfg::c_normalStyle) != c.style().cend()) {
        d->m_font.setWeight(QFont::Normal);
        d->m_font.setItalic(false);
        d->m_font.setUnderline(false);
    } else {
        if (std::find(c.style().cbegin(), c.style().cend(), Cfg::c_boldStyle) != c.style().cend()) {
            d->m_font.setWeight(QFont::Bold);
        } else {
            d->m_font.setWeight(QFont::Normal);
        }

        if (std::find(c.style().cbegin(), c.style().cend(), Cfg::c_italicStyle) != c.style().cend()) {
            d->m_font.setItalic(true);
        } else {
            d->m_font.setItalic(false);
        }

        if (std::find(c.style().cbegin(), c.style().cend(), Cfg::c_underlineStyle) != c.style().cend()) {
            d->m_font.setUnderline(true);
        } else {
            d->m_font.setUnderline(false);
        }
    }

    d->m_font.setPixelSize(MmPx::instance().fromPtY(c.fontSize()));

    d->m_text = c.text();

    if (d->m_properties) {
        d->disconnectProperties();
        d->m_spinProps->ui()->m_value->setValue(d->m_text.toInt());
        d->m_textStyleProps->ui()->m_size->setValue(qRound(c.fontSize()));
        d->m_textStyleProps->ui()->m_bold->setChecked(d->m_font.weight() == QFont::Bold);
        d->m_textStyleProps->ui()->m_italic->setChecked(d->m_font.italic());
        d->m_textStyleProps->ui()->m_underline->setChecked(d->m_font.underline());
        d->connectProperties();
    }

    update();
}

QRectF FormSpinBox::boundingRect() const
{
    return d->m_rect;
}

void FormSpinBox::setPosition(const QPointF &pos,
                              bool pushUndoCommand)
{
    if (pushUndoCommand) {
        page()->undoStack()->push(new UndoMove(page(), objectId(), pos - position()));
    }

    QRectF r = boundingRect();
    r.moveTopLeft(pos);

    if (d->m_properties) {
        d->disconnectProperties();
        d->m_objProps->ui()->m_x->setValue(pos.x());
        d->m_objProps->ui()->m_y->setValue(pos.y());
        d->connectProperties();
    }

    d->setRect(r);
}

QPointF FormSpinBox::position() const
{
    return pos();
}

QRectF FormSpinBox::rectangle() const
{
    QRectF r = boundingRect();
    r.moveTopLeft(position());

    return r;
}

void FormSpinBox::setRectangle(const QRectF &rect,
                               bool pushUndoCommand)
{
    if (pushUndoCommand) {
        page()->undoStack()->push(new UndoResize(page(), objectId(), rectangle(), rect));
    }

    if (d->m_properties) {
        d->disconnectProperties();
        d->m_objProps->ui()->m_width->setValue(rect.width());
        d->m_objProps->ui()->m_height->setValue(rect.height());
        d->connectProperties();
    }

    resize(rect);

    scene()->update();
}

void FormSpinBox::resize(const QRectF &rect)
{
    d->setRect(rect);

    page()->update();
}

void FormSpinBox::moveResizable(const QPointF &delta)
{
    moveBy(delta.x(), delta.y());
}

QSizeF FormSpinBox::defaultSize() const
{
    return {MmPx::instance().fromMmX(10.0), boxHeight()};
}

QWidget *FormSpinBox::properties(QWidget *parent)
{
    d->m_properties = new QWidget(parent);
    d->m_objProps = new ObjectProperties(this, d->m_properties);
    d->m_spinProps = new SpinBoxProperties(d->m_properties);
    d->m_textStyleProps = new TextStyleProperties(d->m_properties);

    auto *l = new QVBoxLayout(d->m_properties);
    l->setContentsMargins(0, 0, 0, 0);
    l->setSpacing(0);
    l->addWidget(d->m_objProps);
    l->addWidget(d->m_spinProps);
    l->addWidget(d->m_textStyleProps);
    l->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));

    d->m_objProps->ui()->m_x->setValue(pos().x());
    d->m_objProps->ui()->m_y->setValue(pos().y());
    d->m_objProps->ui()->m_width->setValue(d->m_rect.width());
    d->m_objProps->ui()->m_height->setValue(d->m_rect.height());
    d->m_spinProps->ui()->m_value->setValue(d->m_text.toInt());
    d->m_textStyleProps->ui()->m_size->setValue(qRound(MmPx::instance().toPtY(d->m_font.pixelSize())));
    d->m_textStyleProps->ui()->m_bold->setChecked(d->m_font.weight() == QFont::Bold);
    d->m_textStyleProps->ui()->m_italic->setChecked(d->m_font.italic());
    d->m_textStyleProps->ui()->m_underline->setChecked(d->m_font.underline());

    d->m_objProps->ui()->m_width->setMinimum(defaultSize().width());
    d->m_objProps->ui()->m_height->setMinimum(defaultSize().height());

    d->connectProperties();

    return d->m_properties.data();
}

FormObject *FormSpinBox::clone() const
{
    auto *o = new FormSpinBox(rectangle(), page(), parentItem());

    o->setCfg(cfg());

    o->setObjectId(page()->nextId());

    return o;
}

} /* namespace Core */

} /* namespace Prototyper */
