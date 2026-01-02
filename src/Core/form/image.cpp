
/*
    SPDX-FileCopyrightText: 2026 Igor Mironchik <igor.mironchik@gmail.com>
    SPDX-License-Identifier: GPL-3.0-or-later
*/

// Prototyper include.
#include "image.hpp"
#include "image_handles.hpp"
#include "page.hpp"
#include "properties/object_properties.hpp"
#include "properties/ui_object_properties.h"
#include "undo_commands.hpp"
#include "utils.hpp"

// Qt include.
#include <QBuffer>
#include <QByteArray>
#include <QGraphicsScene>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>
#include <QUndoStack>
#include <QVBoxLayout>

namespace Prototyper
{

namespace Core
{

//
// FormImagePrivate
//

class FormImagePrivate
{
public:
    explicit FormImagePrivate(FormImage *parent)
        : q(parent)
        , m_handles(nullptr)
    {
    }

    //! Init.
    void init();
    //! Connect properties.
    void connectProperties();
    //! Disconnect properties.
    void disconnectProperties();
    //! \return Full image rect.
    QRectF imageRect() const;

    //! Parent.
    FormImage *q;
    //! Image.
    QImage m_image;
    //! Handles.
    std::unique_ptr<FormImageHandles> m_handles;
    //! Default properties.
    QPointer<ObjectProperties> m_props;
    //! Default properties top widget.
    QPointer<QWidget> m_topProps;
}; // class FormImagePrivate

void FormImagePrivate::init()
{
    m_handles.reset(new FormImageHandles(q, q->parentItem(), q->page()));
}

QRectF FormImagePrivate::imageRect() const
{
    return QRectF(q->pos(), QSizeF(m_image.size()));
}

void FormImagePrivate::connectProperties()
{
    ObjectProperties::connect(m_props->ui()->m_x, QOverload<int>::of(&QSpinBox::valueChanged), m_props, [this](int v) {
        q->setPosition(QPointF(v, q->position().y()));
        q->page()->emitChanged();
    });

    ObjectProperties::connect(m_props->ui()->m_y, QOverload<int>::of(&QSpinBox::valueChanged), m_props, [this](int v) {
        q->setPosition(QPointF(q->position().x(), v));
        q->page()->emitChanged();
    });

    ObjectProperties::connect(m_props->ui()->m_width,
                              QOverload<int>::of(&QSpinBox::valueChanged),
                              m_props,
                              [this](int v) {
                                  QRectF r = q->rectangle();

                                  if (m_handles->isKeepAspectRatio()) {
                                      r = imageRect();
                                      qreal factor = (qreal)v / r.width();
                                      r.setWidth(v);
                                      r.setHeight(qRound(factor * r.height()));
                                  } else {
                                      r.setWidth(v);
                                  }

                                  const bool was = m_handles->isKeepAspectRatio();
                                  m_handles->setKeepAspectRatio(false);
                                  q->setRectangle(r, true);
                                  m_handles->setKeepAspectRatio(was);
                                  q->page()->emitChanged();
                              });

    ObjectProperties::connect(m_props->ui()->m_height,
                              QOverload<int>::of(&QSpinBox::valueChanged),
                              m_props,
                              [this](int v) {
                                  QRectF r = q->rectangle();

                                  if (m_handles->isKeepAspectRatio()) {
                                      r = imageRect();
                                      qreal factor = (qreal)v / r.height();
                                      r.setWidth(qRound(factor * r.width()));
                                      r.setHeight(v);
                                  } else {
                                      r.setHeight(v);
                                  }

                                  const bool was = m_handles->isKeepAspectRatio();
                                  m_handles->setKeepAspectRatio(false);
                                  q->setRectangle(r, true);
                                  m_handles->setKeepAspectRatio(was);
                                  q->page()->emitChanged();
                              });
}

void FormImagePrivate::disconnectProperties()
{
    ObjectProperties::disconnect(m_props->ui()->m_x, QOverload<int>::of(&QSpinBox::valueChanged), nullptr, nullptr);

    ObjectProperties::disconnect(m_props->ui()->m_y, QOverload<int>::of(&QSpinBox::valueChanged), nullptr, nullptr);

    ObjectProperties::disconnect(m_props->ui()->m_width, QOverload<int>::of(&QSpinBox::valueChanged), nullptr, nullptr);

    ObjectProperties::disconnect(m_props->ui()->m_height,
                                 QOverload<int>::of(&QSpinBox::valueChanged),
                                 nullptr,
                                 nullptr);
}

//
// FormImage
//

FormImage::FormImage(Page *page,
                     QGraphicsItem *parent)
    : QGraphicsPixmapItem(parent)
    , FormObject(FormObject::ImageType,
                 page)
    , d(new FormImagePrivate(this))
{
    d->init();
}

FormImage::~FormImage() = default;

Cfg::Image FormImage::cfg() const
{
    Cfg::Image c;

    c.set_objectId(objectId());

    Cfg::Point p;
    p.set_x(MmPx::instance().toMmX(pos().x()));
    p.set_y(MmPx::instance().toMmY(pos().y()));

    c.set_pos(p);

    Cfg::Size s;
    s.set_width(MmPx::instance().toMmX(pixmap().width()));
    s.set_height(MmPx::instance().toMmY(pixmap().height()));

    c.set_size(s);

    c.set_keepAspectRatio(d->m_handles->isKeepAspectRatio());

    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    d->m_image.save(&buffer, "PNG");

    c.set_data(QString::fromLatin1(byteArray.toBase64().data()));

    c.set_z(zValue());

    return c;
}

void FormImage::setCfg(const Cfg::Image &c)
{
    setObjectId(c.objectId());

    const QSize s(MmPx::instance().fromMmX(c.size().width()), MmPx::instance().fromMmY(c.size().height()));

    const QByteArray data = QByteArray::fromBase64(c.data().toLatin1());

    d->m_image = QImage::fromData(data, "PNG");

    d->m_handles->setKeepAspectRatio(c.keepAspectRatio());

    setPixmap(QPixmap::fromImage(d->m_image.scaled(s,
                                                   (c.keepAspectRatio() ? Qt::KeepAspectRatio : Qt::IgnoreAspectRatio),
                                                   Qt::SmoothTransformation)));

    setPos(QPointF(MmPx::instance().fromMmX(c.pos().x()), MmPx::instance().fromMmY(c.pos().y())));

    setZValue(c.z());

    QRectF r = pixmap().rect();
    r.moveTop(pos().y());
    r.moveLeft(pos().x());

    d->m_handles->setRect(r);
}

const QImage &FormImage::image() const
{
    return d->m_image;
}

void FormImage::setImage(const QImage &img)
{
    d->m_image = img;

    setPixmap(QPixmap::fromImage(d->m_image));

    QRectF r = d->m_image.rect();
    r.moveTop(pos().y());
    r.moveLeft(pos().x());

    d->m_handles->setRect(r);
}

void FormImage::paint(QPainter *painter,
                      const QStyleOptionGraphicsItem *option,
                      QWidget *widget)
{
    QGraphicsPixmapItem::paint(painter, option, widget);

    if (isSelected() && !group()) {
        d->m_handles->show();
    } else {
        d->m_handles->hide();
    }
}

void FormImage::setPosition(const QPointF &pos,
                            bool pushUndoCommand)
{
    FormObject::setPosition(pos, pushUndoCommand);

    setPos(pos);

    QRectF r = boundingRect();
    r.moveTopLeft(pos);

    d->m_handles->setRect(r);
}

QPointF FormImage::position() const
{
    return pos();
}

QRectF FormImage::rectangle() const
{
    QRectF r = pixmap().rect();
    r.moveTopLeft(position());

    return r;
}

void FormImage::setRectangle(const QRectF &rect,
                             bool pushUndoCommand)
{
    FormObject::setRectangle(rect, pushUndoCommand);

    resize(rect);

    scene()->update();

    updatePropertiesValues();
}

void FormImage::resize(const QRectF &rect)
{
    setPos(rect.topLeft());

    setPixmap(QPixmap::fromImage(
        d->m_image.scaled(QSize(qRound(rect.width()), qRound(rect.height())),
                          (d->m_handles->isKeepAspectRatio() ? Qt::KeepAspectRatio : Qt::IgnoreAspectRatio),
                          Qt::SmoothTransformation)));

    QRectF r = pixmap().rect();
    r.moveTop(pos().y());
    r.moveLeft(pos().x());

    d->m_handles->setRect(r);

    page()->update();
}

void FormImage::moveResizable(const QPointF &delta)
{
    moveBy(delta.x(), delta.y());
}

FormObject *FormImage::clone() const
{
    auto *o = new FormImage(page(), parentItem());

    o->setCfg(cfg());

    o->setObjectId(page()->nextId());

    return o;
}

QWidget *FormImage::properties(QWidget *parent)
{
    d->m_topProps = new QWidget(parent);
    QVBoxLayout *layout = new QVBoxLayout(d->m_topProps);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    d->m_props = new ObjectProperties(this, d->m_topProps);

    layout->addWidget(d->m_props);
    layout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));

    d->m_props->ui()->m_width->setValue(rectangle().width());
    d->m_props->ui()->m_width->setMinimum(minimumSize().width());
    d->m_props->ui()->m_height->setValue(rectangle().height());
    d->m_props->ui()->m_height->setMinimum(minimumSize().height());

    d->m_props->ui()->m_x->setValue(position().x());
    d->m_props->ui()->m_y->setValue(position().y());

    d->connectProperties();

    return d->m_topProps.data();
}

void FormImage::updatePropertiesValues()
{
    if (d->m_props) {
        d->disconnectProperties();

        d->m_props->ui()->m_width->setValue(qRound(rectangle().width()));
        d->m_props->ui()->m_height->setValue(qRound(rectangle().height()));

        d->m_props->ui()->m_x->setValue(qRound(position().x()));
        d->m_props->ui()->m_y->setValue(qRound(position().y()));

        d->connectProperties();
    }
}

} /* namespace Core */

} /* namespace Prototyper */
